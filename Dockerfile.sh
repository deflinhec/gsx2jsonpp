#!/bin/sh

# 1. build docker images
docker build --rm -t deflinhec/gsx2jsonpp .
docker stop gsx2jsonpp
docker rm gsx2jsonpp

# 2. launch container
docker run -it -d -p 5000:8080 \
-v ${PWD}/bin/volume:/workspace \
--name gsx2jsonpp deflinhec/gsx2jsonpp

# 3. bash entry
docker exec -it gsx2jsonpp bash
