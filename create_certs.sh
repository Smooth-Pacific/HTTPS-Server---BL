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

echo "Generated root certificates"
# Generate server certificates: 

openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr

cat > server.ext << EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = server
EOF


openssl x509 -req -in server.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial -out server.crt -days 825 -sha256 -extfile server.ext

echo "Generated server certificates"
# Generate document signing certificates

openssl genrsa -out document.key 2048
openssl req -new -key document.key -out document.csr
openssl x509 -req -in document.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial -out X509Certificate.crt -days 365 -sha256

echo "Generated document certificates"