#!/bin/bash
set -e

testAlias+=(
	[hellard:trusty]='hellard'
)

imageTests+=(
	[hellard]='
		rpcpassword
	'
)
