# Base image optimized for C++ build environments
FROM ubuntu:22.04

# Suppress interactive prompts during dependency installation
ENV DEBIAN_FRONTEND=noninteractive

# Install core build toolchain and clean up apt cache
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/data-ingestor

# Copy source payload
COPY . .

# Compile binary in Release mode
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# Execute ingestion pipeline
CMD ["./build/DataIngestor"]