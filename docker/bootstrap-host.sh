#!/bin/bash
#
# Configure broken host machine to run correctly
#
set -ex

HELLAR_IMAGE=${HELLAR_IMAGE:-hellarcore/hellard}

distro=$1
shift

memtotal=$(grep ^MemTotal /proc/meminfo | awk '{print int($2/1024) }')

#
# Only do swap hack if needed
#
if [ $memtotal -lt 2048 -a $(swapon -s | wc -l) -lt 2 ]; then
    fallocate -l 2048M /swap || dd if=/dev/zero of=/swap bs=1M count=2048
    mkswap /swap
    grep -q "^/swap" /etc/fstab || echo "/swap swap swap defaults 0 0" >> /etc/fstab
    swapon -a
fi

free -m

if [ "$distro" = "trusty" -o "$distro" = "ubuntu:14.04" ]; then
    curl https://get.docker.io/gpg | apt-key add -
    echo deb http://get.docker.io/ubuntu docker main > /etc/apt/sources.list.d/docker.list

    # Handle other parallel cloud init scripts that may lock the package database
    # TODO: Add timeout
    while ! apt-get update; do sleep 10; done

    while ! apt-get install -y lxc-docker; do sleep 10; done
fi

if [ "$distro" = "bionic" -o "$distro" = "ubuntu:18.04" ]; then
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add -
    add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"

    # Handle other parallel cloud init scripts that may lock the package database
    # TODO: Add timeout
    while ! apt-get update; do sleep 10; done

    while ! apt-get install -y docker-ce; do sleep 10; done
fi


# Always clean-up, but fail successfully
docker kill hellard-node 2>/dev/null || true
docker rm hellard-node 2>/dev/null || true
stop docker-hellard 2>/dev/null || true

# Always pull remote images to avoid caching issues
if [ -z "${HELLAR_IMAGE##*/*}" ]; then
    docker pull $HELLAR_IMAGE
fi

# Initialize the data container
docker volume create --name=hellard-data
docker run -v hellard-data:/hellar --rm $HELLAR_IMAGE hellar_init

# Start hellard via systemd and docker
curl https://raw.githubusercontent.com/hellarcore/docker-hellard/master/init/docker-hellard.service > /lib/systemd/system/docker-hellard.service
systemctl start docker-hellard

set +ex
echo "Resulting hellar.conf:"
docker run -v hellard-data:/hellar --rm $HELLAR_IMAGE cat /hellar/.hellarcore/hellar.conf
