#! /bin/bash

# VM_IMAGE
cd /home/rtes/GVM5/CMND_VM

echo "Restore ubuntu_pv_0"
#sudo cp ubuntu_pv_0.bak ubuntu_pv_0.img

echo "Restore ubuntu_pv_1"
#sudo cp ubuntu_pv_1.bak ubuntu_pv_1.img

echo "Restore ubuntu_pv_2"
#sudo cp ubuntu_pv_2.bak ubuntu_pv_2.img

# PM VM FILE
cd /home/rtes/GVM5/CMND/CMND_daemon

echo "Creating VM_FILE"
#cp VM_FILE.bak VM_FILE.txt

echo "Creating PM_FILE"
#cp PM_FILE.bak PM_FILE.txt

sudo 	./CMND_daemon	\
	192.168.2.218	\
	30010		\
	2		\
	16		\
	2048		\
	192.168.2.217	\
	11605		\
	19900108	\
	PM_FILE.txt	\
	VM_FILE.txt
