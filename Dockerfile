FROM alpine:latest as build
LABEL maintainer "deflinhec <deflinhec@gmail.com>"

RUN apk --no-cache add bash \
    git linux-headers \
    cmake make gcc g++ \
    libc-dev \
    libressl-dev \
	zlib-dev \
    supervisor

COPY . /project
WORKDIR /project
RUN export CC=gcc CXX=g++ && \
    /bin/bash scripts/build.sh Release

FROM alpine:latest as staging
COPY --from=build /project/bin/Gsx2Jsonpp /bin/Gsx2Jsonpp

RUN apk --no-cache add \
    linux-headers \
    libstdc++ \
    libc-dev \
    libressl-dev \
	zlib-dev

EXPOSE 8080
ENV TZ=Asia/Taipei

WORKDIR /workspace
ENTRYPOINT [ "/bin/Gsx2Jsonpp" ]
CMD ["--host=0.0.0.0" , "--port=8080"]