FROM debian:bullseye-slim
LABEL maintainer "deflinhec <deflinhec@gmail.com>"

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
	gcc \
	g++ \
	git \
	make \
	cmake \
	libssl-dev \
	zlib1g-dev \
	apt-utils \
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
RUN /bin/bash scripts/build.sh $BUILD_TYPE

ENV TZ=Asia/Taipei
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime
RUN echo $TZ > /etc/timezone

WORKDIR /workspace
VOLUME /workspace

COPY supervisor.conf /etc/supervisor.conf
CMD ["supervisord", "-c", "/etc/supervisor.conf"]
