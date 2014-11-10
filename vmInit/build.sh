#!/bin/bash
gcc VM_init.c ds_lib/ds_tcp_sock.c -o VM_init

rm VM_init.sh
echo '#!/bin/bash'		>> VM_init.sh
echo "cd $(pwd)"		>> VM_init.sh
echo "./IP_setting.sh"		>> VM_init.sh
echo "./IP_sending.sh $1 $2"	>> VM_init.sh

cp interfaces.bak /etc/network/interfaces.bak

sudo chmod +x VM_init.sh
sudo echo "$(pwd)/VM_init.sh" >> /etc/init.d/rc.local
