#! /bin/bash
echo "test"

mkdir -p build

g++ server.cpp -o build/server
g++ client.cpp -o build/client