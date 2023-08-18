#!/bin/bash
# use testnet settings,  if you need mainnet,  use ~/.hellarcore/hellard.pid file instead
hellar_pid=$(<~/.hellarcore/testnet3/hellard.pid)
sudo gdb -batch -ex "source debug.gdb" hellard ${hellar_pid}
