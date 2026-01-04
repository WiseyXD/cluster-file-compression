FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libzookeeper-mt-dev \
    zookeeper \
    protobuf-compiler \
    libprotobuf-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p generated \
    && protoc -I src/core/proto --cpp_out=generated src/core/proto/huffman.proto

RUN rm -rf build \
    && cmake -S . -B build \
    && cmake --build build -j$(nproc)


CMD ["./build/app"]

