Hellard for Docker
================

[![Docker Stats](http://dockeri.co/image/hellarcore/hellard)](https://hub.docker.com/r/hellarcore/hellard/)

[![Build Status](https://travis-ci.org/hellarcore/docker-hellard.svg?branch=master)](https://travis-ci.org/hellarcore/docker-hellard/)


Docker image that runs the Hellar hellard node in a container for easy deployment.


Requirements
------------

* Physical machine, cloud instance, or VPS that supports Docker (i.e. Vultr, Digital Ocean, KVM or XEN based VMs) running Ubuntu 18.04 or later (*not OpenVZ containers!*)
* At least 40 GB to store the block chain files
* At least 2 GB RAM + 2 GB swap file


Really Fast Quick Start
-----------------------

One liner for Ubuntu 18.04 LTS machines with JSON-RPC enabled on localhost and adds upstart init script:

    curl https://raw.githubusercontent.com/hellarcore/hellard/master/bootstrap-host.sh | sh -s bionic


Quick Start
-----------

1. Create a `hellard-data` volume to persist the hellard blockchain data, should exit immediately.  The `hellard-data` container will store the blockchain when the node container is recreated (software upgrade, reboot, etc):

        docker volume create --name=hellard-data
        docker run -v hellard-data:/hellar --name=hellard-node -d \
            -p 7778:7778 \
            -p 127.0.0.1:7788:7788 \
            hellarcore/hellard

2. Verify that the container is running and hellard node is downloading the blockchain

        $ docker ps
        CONTAINER ID        IMAGE                         COMMAND             CREATED             STATUS              PORTS                                              NAMES
        d0e1076b2dca        hellarcore/hellard:latest          "hellar_oneshot"      2 seconds ago       Up 1 seconds        127.0.0.1:7788->7788/tcp, 0.0.0.0:7778->7778/tcp   hellard-node

3. You can then access the daemon's output thanks to the [docker logs command]( https://docs.docker.com/reference/commandline/cli/#logs)

        docker logs -f hellard-node

4. Install optional init scripts for upstart and systemd are in the `init` directory.


Documentation
-------------

* To run in testnet, add environment variable `TESTNET=1` to `docker run` as such:

        docker run -v hellard-data:/hellar --name=hellard-node -d \
            --env TESTNET=1 \
            -p 7778:7778 \
            -p 127.0.0.1:7788:7788 \
            hellarcore/hellard

* Additional documentation in the [docs folder](docs).

Credits
-------

Original work by Kyle Manna [https://github.com/kylemanna/docker-bitcoind](https://github.com/kylemanna/docker-bitcoind).
Modified to use Hellar Core instead of Bitcoin Core.

