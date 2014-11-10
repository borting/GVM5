#!/bin/bash

PWD=`pwd`
cd /home/ds/gvm5/vAppd

IP=`ifconfig eth1 | grep 'inet addr:'| cut -d: -f2 | awk '{ print $1}'`

./vAppMgr $IP 10001 10000 192.168.2.217 11605 &> err_Mgr &
sleep 1
./vAppd 10000 10001 &> err_vAppd &

cd $PWD
