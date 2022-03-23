#!/bin/sh

# Script to build and run Docker image

dockerfile="Dockerfile"

imageName="dockerdev"

contextPath="."

# Builds docker image
if [[ $1 = "build" ]]; then
	docker build -f $dockerfile -t $imageName $contextPath
# execs docker container in separate tab
elif [[ $1 = "exec" ]]; then
	export CONTAINER=`docker ps | tail -1 | awk '{print $1}'`
	docker exec -it $CONTAINER /bin/zsh
# creates ipv6 network for docker
elif [[ $1 = "ipv6" ]]; then
	docker network create --ipv6 --subnet="2001:db8:1::/64" --gateway="2001:db8:1::1" mynetv6-1
# runs docker container
else
	docker run -it -v "$(pwd)":/home --env-file env.list --network mynetv6-1 $imageName bash
fi



