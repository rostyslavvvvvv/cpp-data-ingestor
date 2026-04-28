# C++ Multithreaded Data Ingestor

A high-performance, lock-free data processing pipeline built in C++17. This project demonstrates "Mechanical Sympathy"—optimizing software to work in harmony with the underlying hardware for maximum throughput.

## Performance
* **Throughput:** Parses and aggregates 50,000 transaction records in **~16.5ms** (on Apple Silicon).
* **Efficiency:** Achieved a **3x speedup** over the original lock-based version by moving to a lock-free architecture.
* **Optimization:** Compiled via CMake with `-O3` flags to maximize CPU instruction efficiency.

## Tech Stack
* **Language:** C++17
* **Build System:** CMake
* **Infrastructure:** Docker (Multi-stage builds)

## Engineering Decisions
* **Lock-Free Synchronization:** Based on senior engineer feedback, I pivoted from `std::mutex` to `std::atomic<uint64_t>` using `std::memory_order_relaxed`. This eliminates thread stalls and context-switching overhead.
* **Dynamic Workload Distribution:** The engine detects hardware concurrency and divides the dataset into equal chunks, ensuring 100% CPU utilization without resource contention.
* **Multi-Stage Docker Build:** Implemented a two-stage build process. The final production image contains only the binary and necessary data, stripping away the build toolchain (GCC/CMake) to reduce the attack surface and image size.
* **Fault Tolerance:** Robust `try-catch` parsing ensures malformed CSV rows are dropped safely to maintain pipeline integrity without crashing.

## How to Run (Docker)

```bash
# Clone the repository
git clone [https://github.com/rostyslavvvvvv/cpp-data-ingestor.git](https://github.com/rostyslavvvvvv/cpp-data-ingestor.git)
cd cpp-data-ingestor

# Build the container image
docker build -t cpp-data-engine .

# Run the ingestion engine
docker run cpp-data-engine