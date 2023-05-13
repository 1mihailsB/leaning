#! /bin/bash
mkdir -p build

g++ src/server.cpp -o build/server
g++ src/client.cpp -o build/client