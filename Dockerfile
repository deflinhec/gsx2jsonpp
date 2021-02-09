FROM alpine:latest
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

EXPOSE 8080
ENV TZ=Asia/Taipei \
    ARGUMENTS="--host 0.0.0.0"

COPY supervisor.conf /etc/supervisor.conf
CMD ["supervisord", "-c", "/etc/supervisor.conf"]

WORKDIR /workspace
VOLUME /workspace