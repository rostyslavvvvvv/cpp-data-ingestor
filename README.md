# C++ Multithreaded Data Ingestor

A high-throughput data processing pipeline built in C++17. I designed this project to handle large CSV datasets by dynamically distributing the parsing workload across available CPU cores. 

The goal was to demonstrate core systems engineering concepts: concurrency, thread safety, build optimization, and containerized deployment.

## Performance
* **Throughput:** Parses and aggregates 50,000 transaction records in ~40ms (tested on Apple Silicon).
* **Optimization:** Compiled via CMake with `-O3` flags to maximize CPU instruction efficiency.

## Tech Stack
* **Language:** C++17
* **Build System:** CMake
* **Infrastructure:** Docker, Ubuntu 22.04

## Engineering Decisions
* **Dynamic Thread Allocation:** Instead of a blocking single-threaded loop, the program detects the host machine's hardware concurrency (CPU cores) and divides the dataset into equal chunks for parallel processing using `std::thread`.
* **Thread Safety:** Implemented `std::mutex` locks during the aggregation and logging phases to prevent race conditions and output scrambling.
* **Fault Tolerance:** Added robust error handling (`try-catch`) within the parsing loop. If a CSV row is malformed or corrupted, the engine safely drops the row rather than crashing the entire ingestion pipeline.
* **Portability:** Containerized with Docker using a strict Ubuntu base image to ensure the C++ build environment is identical regardless of where it is deployed.

## How to Run (Docker)

You don't need a local C++ compiler to run this. The entire pipeline is containerized.

```bash
# Clone the repository
git clone [https://github.com/rostyslavvvvvv/cpp-data-ingestor.git](https://github.com/rostyslavvvvvv/cpp-data-ingestor.git)
cd cpp-data-ingestor

# Build the container image
docker build -t cpp-data-engine .

# Run the ingestion engine
docker run cpp-data-engine