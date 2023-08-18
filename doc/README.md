Hellar 0.17.0
====================

Copyright (c) 2009-2018 Bitcoin Developers
Copyright (c) 2014-2019 Dash Core Developers
Copyright (c) 2015-2020 Hellar Developers

This is the official reference wallet for Hellar digital currency and comprises the backbone of the Hellar peer-to-peer network. You can [download Hellar Core](https://www.hellar.io/downloads/) or [build it yourself](#building) using the guides below.

Running
---------------------
The following are some helpful notes on how to run Hellar on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/hellar-qt` (GUI) or
- `bin/hellard` (headless)

### Windows

Unpack the files into a directory, and then run hellar-qt.exe.

### OS X

Drag Hellar-Qt to your applications folder, and then run Hellar-Qt.

### Need Help?

* See the [Hellar documentation](https://docs.hellar.io)
for help and more information.
* Ask for help on [Hellar Nation Discord](http://hellarchat.io)
* Ask for help on the [Hellar Forum](https://hellar.io/forum)

Building
---------------------
The following are developer notes on how to build Hellar Core on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [OS X Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The Hellar Core repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- Source Code Documentation ***TODO***
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [Travis CI](travis-ci.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Shared Libraries](shared-libraries.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)

### Resources
* Discuss on the [Hellar Forum](https://hellar.io/forum), in the Development & Technical Discussion board.
* Discuss on [Hellar Nation Discord](http://hellarchat.io)

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
