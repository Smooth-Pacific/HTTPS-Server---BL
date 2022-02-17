#!/bin/bash

# Runs Docker

imageName="dockerdev"

docker run -it -v "$(pwd)":/home --env-file env.list $imageName bash
