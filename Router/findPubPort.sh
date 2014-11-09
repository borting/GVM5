#!/bin/sh

# List all DNAT ports used by a particular IP
result=`iptables -t nat -L PREROUTING -n | sed -e '1,2d' | \
	grep "to:$1:$2" | awk -F'[ :]+' '{print $8}'`

if test -z $result; then
    echo 0
else
    echo $result
fi

