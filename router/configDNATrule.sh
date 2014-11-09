#!/bin/sh

iptables -t nat -$1 PREROUTING -p tcp -i $2 --dport $3 -j DNAT --to $4\:$5

# return iptables exit code
echo $?

