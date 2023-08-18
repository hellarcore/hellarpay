FROM debian:stretch
LABEL maintainer="Hellar Developers <dev@hellar.io>"
LABEL description="Dockerised HellarCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
