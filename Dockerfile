FROM debian:bullseye-slim
LABEL maintainer "deflinhec <deflinhec@gmail.com>"

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils
RUN apt-get install -y --no-install-recommends \
	gcc \
	g++ \
	git \
	make \
	cmake \
	libc6-dev \
	libssl-dev \
	zlib1g-dev \
    apt-transport-https \
	ca-certificates \
	supervisor \
	&& rm -rf /var/lib/apt/lists/* \
	&& mkdir -p /var/log/supervisor \
	&& mkdir -p /etc/supervisor/conf.d

ENV CC=gcc
ENV CXX=g++
EXPOSE 8080
ARG BUILD_TYPE=Release

COPY . /project
WORKDIR /project
RUN cd build && /bin/bash build.sh $BUILD_TYPE

ENV TZ=Asia/Taipei
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime
RUN echo $TZ > /etc/timezone

WORKDIR /workspace
VOLUME /workspace

COPY supervisor.conf /etc/supervisor.conf
CMD ["supervisord", "-c", "/etc/supervisor.conf"]
