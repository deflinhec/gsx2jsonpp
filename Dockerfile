FROM alpine:latest
LABEL maintainer "deflinhec <deflinhec@gmail.com>"

RUN apk --no-cache add bash \
    git linux-headers \
    cmake make gcc g++ \
    libc-dev \
    libressl-dev \
	zlib-dev \
    supervisor

ENV CC=gcc CXX=g++ TZ=Asia/Taipei
EXPOSE 8080

COPY . /project
WORKDIR /project
RUN /bin/bash scripts/build.sh Release

COPY supervisor.conf /etc/supervisor.conf
CMD ["supervisord", "-c", "/etc/supervisor.conf"]

WORKDIR /workspace
VOLUME /workspace