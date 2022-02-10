#!/bin/bash

# Runs Docker

imageName="dockerdev"

docker run -it -v "$(pwd)":/home $imageName bash
