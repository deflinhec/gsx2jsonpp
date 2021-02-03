# 1. build images
docker build --rm -t gsx2jsonpp ..

# 2. cleanup container
docker stop gsx2jsonpp
docker rm gsx2jsonpp

# 3. launch container
docker run -it -d -p 5000:8080 `
-v ${PWD}\..\bin\volume:/workspace `
--name gsx2jsonpp gsx2jsonpp

# 4. bash entry
docker exec -it gsx2jsonpp bash
