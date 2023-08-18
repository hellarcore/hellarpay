Masternode config
=======================

The multi masternode config allows to control multiple masternodes from a single wallet. The wallet needs to have a valid collaral output of 1000 coins for each masternode. To use this, place a file named masternode.conf in the data directory of your install:
 * Windows: %APPDATA%\Hellar\
 * Mac OS: ~/Library/Application Support/Hellar/
 * Unix/Linux: ~/.hellar/

`masternode.conf` is a space separated text file. Each line consists of an alias, IP address followed by port, masternode private key, collateral output transaction id and collateral output index.

Example:
```
mn1 127.0.0.2:19989 93HaYBVUCYjEMeeH1Y4sBGLALQZE1Yc1K64xiqgX37tGBDQL8Xg 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a67c 0
mn2 127.0.0.3:19989 93WaAb3htPJEV8E9aQcN23Jt97bPex7YvWfgMDTUdWJvzmrMqey aa9f1034d973377a5e733272c3d0eced1de22555ad45d6b24abadff8087948d4 0 7gnwGHt17heGpG9Crfeh4KGpYNFugPhJdh:33
mn3 127.0.0.4:19989 92Da1aYg6sbenP6uwskJgEY2XWB5LwJ7bXRqc3UPeShtHWJDjDv db478e78e3aefaa8c12d12ddd0aeace48c3b451a8b41c570d0ee375e2a02dfd9 1 7gnwGHt17heGpG9Crfeh4KGpYNFugPhJdh
```

In the example above:
* the collateral of 1000 HEL for `mn1` is output `0` of transaction [7603c20a05258c208b58b0a0d77603b9fc93d47cfa403035f87f3ce0af814566](https://test.explorer.dash.org/tx/7603c20a05258c208b58b0a0d77603b9fc93d47cfa403035f87f3ce0af814566)
* the collateral of 1000 HEL for `mn2` is output `1` of transaction [5d898e78244f3206e0105f421cdb071d95d111a51cd88eb5511fc0dbf4bfd95f](https://test.explorer.dash.org/tx/5d898e78244f3206e0105f421cdb071d95d111a51cd88eb5511fc0dbf4bfd95f)

_Note: IPs like 127.0.0.* are not allowed actually, we are using them here for explanatory purposes only. Make sure you have real reachable remote IPs in you `masternode.conf`._

The following RPC commands are available (type `help masternode` in Console for more info):
* list-conf
* start-alias \<alias\>
* start-all
* start-missing
* start-disabled
* outputs
