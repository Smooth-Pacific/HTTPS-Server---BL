#!/bin/sh

# Script to build and run Docker image

dockerfile="Dockerfile"

imageName="dockerdev"

contextPath="."


if [[ $1 = "build" ]]; then
	docker build -f $dockerfile -t $imageName $contextPath
	docker network create --ipv6 --subnet="2001:db8:1::/64" --gateway="2001:db8:1::1" mynetv6-1

elif [[ $1 = "exec" ]]; then
	export CONTAINER=`docker ps | tail -1 | awk '{print $1}'`
	docker exec -it $CONTAINER /bin/zsh

else
	docker run -it -v "$(pwd)":/home --env-file env.list --network mynetv6-1 $imageName bash
fi