#!/bin/sh


dockerfile="Dockerfile"

imageName="dockerdev"

contextPath="."

docker build -f $dockerfile -t $imageName $contextPath
