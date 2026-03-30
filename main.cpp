#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>

// Protects standard output from race conditions during multithreaded logging
std::mutex console_mutex; 

struct Transaction {
    int id;
    std::string name;
    double amount;
    std::string timestamp;
};

// Worker thread logic: parses data chunk and aggregates transaction metrics
void processChunk(const std::vector<std::string>& chunk, int threadId) {
    int processedCount = 0;
    double threadTotalAmount = 0.0;
    
    for (const auto& line : chunk) {
        // Bypass empty rows and cvs headers
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
            // Silently drop malformed rows to maintain pipeline throughput
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
        std::cerr << "Fatal Error: Could not locate data payload (" << filename << ")\n";
        return 1;
    }

    std::vector<std::string> allLines;
    std::string line;
    
    // Load dataset into memory (optimization note: use memory mapping for files > RAM capacity)
    while (std::getline(file, line)) {
        allLines.push_back(line);
    }
    file.close();

    // Dynamically allocate workload based on available system cores
    int numThreads = std::thread::hardware_concurrency(); 
    if (numThreads == 0) numThreads = 4; // Fallback for restrictive environments

    std::vector<std::thread> threads;
    int chunkSize = allLines.size() / numThreads;

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