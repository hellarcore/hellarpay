GENERIC BUILD NOTES
====================
Some notes on how to build Hellar Core based on the [depends](../depends/README.md) build system.

Note on old build instructions
------------------------------
In the past, the build documentation contained instructions on how to build Hellar with system-wide installed dependencies
like BerkeleyDB 4.8, boost and Qt. Building this way is considered deprecated and only building with the `depends` prefix
is supported today.

Required build tools and environment
------------------------------------
Building the dependencies and Hellar Core requires some essential build tools to be installed before. Please see
[build-unix](build-unix.md), [build-osx](build-osx.md) and [build-windows](build-windows.md) for details.

Building dependencies
---------------------
Hellar inherited the `depends` folder from Bitcoin, which contains all dependencies required to build Hellar. These
dependencies must be built before Hellar can actually be built. To do so, perform the following:

```bash
$ cd depends
$ make -j4 # Choose a good -j value, depending on the number of CPU cores available
$ cd ..
```

This will download and build all dependencies required to build Hellar Core. Caching of build results will ensure that only
the packages are rebuilt which have changed since the last depends build.

It is required to re-run the above commands from time to time when dependencies have been updated or added. If this is
not done, build failures might occur when building Hellar.

Please read the [depends](../depends/README.md) documentation for more details on supported hosts and configuration
options. If no host is specified (as in the above example) when calling `make`, the depends system will default to your
local host system. 

Building Hellar Core
---------------------

```bash
$ ./autogen.sh
$ ./configure --prefix `pwd`/depends/<host>
$ make
$ make install # optional
```

Please replace `<host>` with your local system's `host-platform-triplet`. The following triplets are usually valid:
- `i686-pc-linux-gnu` for Linux32
- `x86_64-pc-linux-gnu` for Linux64
- `i686-w64-mingw32` for Win32
- `x86_64-w64-mingw32` for Win64
- `x86_64-apple-darwin11` for MacOSX
- `arm-linux-gnueabihf` for Linux ARM 32 bit
- `aarch64-linux-gnu` for Linux ARM 64 bit

If you want to cross-compile for another platform, choose the appropriate `<host>` and make sure to build the
dependencies with the same host before.

If you want to build for the same host but different distro, add `--enable-glibc-back-compat` when calling `./configure`.


ccache
------
The depends system also contains [ccache](https://ccache.samba.org/), which caches build results on source->object
level. `./configure` of Hellar Core will autodetect the presence of ccache and enable use of it. To disable ccache, use
`./configure --prefix=<prefix> --disable-ccache`.

The default maximum cache size is 5G, which might not be enough to cache multiple builds when switching Git branches
very often. It is advised to increase the maximum cache size:

```bash
$ ./depends/<host>/native/bin/ccache -M20G
```

Additional Configure Flags
--------------------------
A list of additional configure flags can be displayed with:

```bash
./configure --help
```
Ubuntu 18.04 tested Configuing Pre-Install
--------------------------
sudo apt-get update

sudo apt-get -y install software-properties-common

sudo apt-get -y install build-essential gcc make perl dkms

sudo reboot

sudo apt-get install -y git

sudo apt-get install -y curl
 
sudo apt-get -y install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils
 
sudo apt-get -y install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev
 
sudo apt-get -y install libboost-all-dev
 
sudo add-apt-repository ppa:bitcoin/bitcoin
 
sudo apt-get update
 
sudo apt-get -y install libdb4.8-dev libdb4.8++-dev
 
sudo apt-get -y install libminiupnpc-dev
 
sudo apt-get -y install libzmq3-dev
 
sudo apt-get -y install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler
 
sudo apt-get -y install libqt4-dev libprotobuf-dev protobuf-compiler
 
sudo apt-get -y install openssl1.0

sudo apt-get -y install libssl1.0-dev

sudo apt-get -y install libqrencode-dev

sudo apt-get -y install build-essential cmake 
