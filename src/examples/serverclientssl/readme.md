https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example

openssl genrsa -des3 -out server.key 2048
openssl req -new -key server.key -out server.csr
openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt

cp server.key server.key.secure
openssl rsa -in server.key.secure -out server.key

openssl dhparam -out dh1024.pem 1024

g++ client.cpp -o client -lssl -lcrypto

