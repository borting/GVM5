#!/bin/bash

CMND_IP=192.168.2.218
CMND_PORT=30011

VM_PVT_IP=$(ifconfig eth0 | grep 'inet addr:'| cut -d: -f2 | awk '{ print $1}')
VM_PUB_IP=$(ifconfig eth1 | grep 'inet addr:'| cut -d: -f2 | awk '{ print $1}')

./VM_init $1 $2 $VM_PVT_IP $VM_PUB_IP
