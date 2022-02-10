#!/bin/sh

# Build Docker image

dockerfile="Dockerfile"

imageName="dockerdev"

contextPath="."

docker build -f $dockerfile -t $imageName $contextPath
