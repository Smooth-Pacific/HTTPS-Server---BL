#!/bin/bash

# Script to test document signing
# Creates and verifies signature of sample file

# File to sign
file=$1
# Output signature
signature=$2

openssl dgst -sha256 -sign document.key -out $signature $file
echo "Signature created"
openssl x509 -in document.crt
openssl dgst -sha256 -verify <(openssl x509 -in document.crt -pubkey -noout) -signature $signature $file