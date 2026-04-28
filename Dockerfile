# STAGE 1: Build environment
FROM ubuntu:22.04 AS builder

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Compile the optimized binary
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# STAGE 2: Runtime environment (The "Lean" image)
FROM ubuntu:22.04

WORKDIR /root/
# Only copy the compiled binary and data from the builder stage
COPY --from=builder /app/build/DataIngestor .
COPY --from=builder /app/data.csv .

# Run the binary
CMD ["./DataIngestor"]