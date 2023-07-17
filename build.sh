#! /bin/bash
mkdir -p build

g++ src/server.cpp -Wall -Wextra -Werror -o build/server
g++ src/http_server.cpp -Wall -Wextra -Werror -o build/server
g++ src/client.cpp -Wall -Wextra -Werror -o build/client