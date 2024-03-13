# Debugging

## Things to Check

* RAM utilization -- hellard is very hungry and typically needs in excess of 1GB.  A swap file might be necessary.
* Disk utilization -- The hellar blockchain will continue growing and growing and growing.  Then it will grow some more.  At the time of writing, 2GB+ is necessary.

## Viewing hellard Logs

    docker logs hellard-node


## Running Bash in Docker Container

*Note:* This container will be run in the same way as the hellard node, but will not connect to already running containers or processes.

    docker run -v hellard-data:/hellar --rm -it hellarcore/hellard bash -l

You can also attach bash into running container to debug running hellard

    docker exec -it hellard-node bash -l


