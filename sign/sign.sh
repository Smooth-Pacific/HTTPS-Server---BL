#!/bin/bash

# Script to test document signing
# Creates and verifies signature of sample file

# File to sign
file=$1


openssl dgst -sha256 -sign /home/server_ca/private/document.key -out ${file}.signature $file
echo "Signature created"
openssl x509 -in /home/server_ca/certs/document.crt
openssl dgst -sha256 -verify <(openssl x509 -in /home/server_ca/certs/document.crt -pubkey -noout) -signature ${file}.signature $file


# openssl dgst -sha256 -verify <(openssl x509 -in /home/server_ca/certs/document.crt -pubkey -noout) -signature example.txt.signature example.txt