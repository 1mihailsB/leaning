#! /bin/bash
mkdir -p build

g++ src/server.cpp -Wall -o build/server
g++ src/client.cpp -Wall -o build/client