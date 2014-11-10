#!/bin/bash

#ifdown eth1
#service network-manager restart
#service networking restart
#/etc/init.d/networking restart

cp /etc/network/interfaces.bak /etc/network/interfaces

ifconfig eth0 down
ifconfig eth0 up

PUB_SUBNET="192.168.20."
VM_PVT_IP=$(ifconfig eth0 | grep 'inet addr:'| cut -d: -f2 | awk '{ print $1}')
NUM=${VM_PVT_IP##*.}
VM_PUB_IP=$PUB_SUBNET$NUM

#ifup eth1
#service network-manager restart
#service networking restart
#sudo /etc/init.d/networking restart

ifconfig eth1 $VM_PUB_IP netmask 255.255.255.0
route add default gw 192.168.20.1 eth1

echo "" 				>> /etc/network/interfaces
echo "auto eth1" 			>> /etc/network/interfaces
echo "iface eth1 inet static" 		>> /etc/network/interfaces
echo "	address $VM_PUB_IP" 		>> /etc/network/interfaces
echo "	netmask 255.255.255.0" 		>> /etc/network/interfaces
echo "	gateway 192.168.20.1" 		>> /etc/network/interfaces
echo "	dns-nameservers 8.8.8.8" 	>> /etc/network/interfaces

ifdown eth1
ifup eth1

#ifconfig eth1 down
#ifconfig eth1 up
