#!/bin/bash
# 1. pull image
docker pull deflinhec/gsx2jsonpp

# 2. cleanup container
docker stop gsx2jsonpp-latest
docker rm gsx2jsonpp-latest

# 3. launch container
docker run -it -d -p 5000:8080 \
-v ${PWD}/bin/volume:/workspace \
--name gsx2jsonpp-latest deflinhec/gsx2jsonpp

# 4. bash entry
docker exec -it gsx2jsonpp bash
