#!/bin/bash

# Script to create certificates if not already made
dir=server_ca/certs

if [[ -e $dir ]]; then
	cp /home/server_ca/certs/root.crt /usr/local/share/ca-certificates/root.crt
	update-ca-certificates
else
	# Create certificate directory
	mkdir ./server_ca
	cd ./server_ca
	mkdir private
	mkdir certs
	mkdir csr

	# Generate root certificates
	openssl genrsa -des3 -out private/root.key 2048
	openssl req -x509 -new -config ../configs/root.conf -nodes -key private/root.key -sha256 -days 365 -out certs/root.crt


	# Move and update certificate store
	cp /home/server_ca/certs/root.crt /usr/local/share/ca-certificates/root.crt
	update-ca-certificates

	echo "Generated root certificates"
	# Generate server certificates: 

	openssl genrsa -out private/server.key 2048
	openssl req -new -config ../configs/server.conf -key private/server.key -out csr/server.csr

	cat > server.ext << EOF
	authorityKeyIdentifier=keyid,issuer
	basicConstraints=CA:FALSE
	keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
	subjectAltName = @alt_names
	[alt_names]
	DNS.1 = localhost
	IP.3 = ::1
EOF


	openssl x509 -req -in csr/server.csr -CA certs/root.crt -CAkey private/root.key -CAcreateserial -out certs/server.crt -days 365 -sha256 -extfile server.ext

	cat certs/server.crt certs/root.crt > server-chain-bundle.cert.pem

	echo "Generated server certificates"
	# Generate document signing certificates

	openssl genrsa -out private/document.key 2048
	openssl req -new -config ../configs/document.conf -key private/document.key -out csr/document.csr
	openssl x509 -req -in csr/document.csr -CA certs/root.crt -CAkey private/root.key -CAcreateserial -out certs/document.crt -days 365 -sha256

	cat certs/document.crt certs/root.crt > document-chain-bundle.cert.pem

	echo "Generated document certificates"


	openssl verify -CAfile certs/root.crt certs/server.crt
	openssl verify -CAfile certs/root.crt certs/document.crt
fi
