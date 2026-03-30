#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>

std::mutex console_mutex;

struct Transaction {
    int id;
    std::string name;
    double amount;
    std::string timestamp;
};

void processChunk(const std::vector<std::string>& chunk, int threadId) {
    int processedCount = 0;
    double threadTotalAmount = 0.0;

    for (const auto& line : chunk) {
        // Skip empty lines and CSV headers
        if (line.empty() || line.find("transaction_amount") != std::string::npos) {
            continue;
        }

        std::stringstream ss(line);
        std::string item;
        Transaction tx;

        try {
            std::getline(ss, item, ','); tx.id = std::stoi(item);
            std::getline(ss, item, ','); tx.name = item;
            std::getline(ss, item, ','); tx.amount = std::stod(item);
            std::getline(ss, item, ','); tx.timestamp = item;

            threadTotalAmount += tx.amount;
            processedCount++;
        } catch (...) {
            // Drop malformed rows to maintain throughput
            continue;
        }
    }

    std::lock_guard<std::mutex> lock(console_mutex);
    std::cout << "[Thread " << threadId << "] Processed " << processedCount 
              << " rows. Subtotal: $" << threadTotalAmount << "\n";
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string filename = "data.csv";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not locate " << filename << "\n";
        return 1;
    }

    std::vector<std::string> allLines;
    std::string line;

    // Ingest dataset
    while (std::getline(file, line)) {
        allLines.push_back(line);
    }
    file.close();

    // Workload distribution
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    std::vector<std::thread> threads;
    size_t chunkSize = allLines.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int startIdx = i * chunkSize;
        int endIdx = (i == numThreads - 1) ? allLines.size() : startIdx + chunkSize;

        std::vector<std::string> chunk(allLines.begin() + startIdx, allLines.begin() + endIdx);
        threads.push_back(std::thread(processChunk, chunk, i));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = end_time - start_time;

    std::cout << "Ingestion pipeline completed in " << ms_double.count() << " ms\n";
    return 0;
}