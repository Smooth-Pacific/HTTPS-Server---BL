#!/bin/bash

imageName="dockerdev"

docker run -it -v "$(pwd)":/home $imageName bash
