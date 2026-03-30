# Build environment
FROM ubuntu:22.04

# Non-interactive build
ENV DEBIAN_FRONTEND=noninteractive

# Build tools
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/data-ingestor

# Source code
COPY . .

# Production build
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# Run
CMD ["./build/DataIngestor"]