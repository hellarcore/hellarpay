#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

DOCKER_IMAGE=${DOCKER_IMAGE:-hellar/hellard-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/hellard docker/bin/
cp $BUILD_DIR/src/hellar-cli docker/bin/
cp $BUILD_DIR/src/hellar-tx docker/bin/
strip docker/bin/hellard
strip docker/bin/hellar-cli
strip docker/bin/hellar-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
