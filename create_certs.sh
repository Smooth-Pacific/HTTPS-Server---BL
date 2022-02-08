#!/bin/bash

# Script to create certificates if not already made

# Create certificate directory
mkdir ./certs
cd ./certs

# Generate root certificates
openssl genrsa -des3 -out myCA.key 2048

openssl req -x509 -new -nodes -key myCA.key -sha256 -days 1825 -out myCA.pem


# Move and update certificate store
cp ~/certs/myCA.pem /usr/local/share/ca-certificates/myCA.crt
update-ca-certificates


# Generate server certificates: 

openssl genrsa -out server.test.key 2048

openssl req -new -key server.test.key -out server.test.csr

cat > server.test.ext << EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = server.test
EOF


openssl x509 -req -in server.test.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial -out server.test.crt -days 825 -sha256 -extfile server.test.ext


#