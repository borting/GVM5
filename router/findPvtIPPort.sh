#!/bin/sh

# List all DNAT ports used by a particular IP
result=`iptables -t nat -L PREROUTING -n | sed -e '1,2d' | \
	grep "dpt:$1" | awk -F'[ :]+' '{print $10, $11}'`

if test -z $result; then
    echo 0.0.0.0 0
else
    echo "$result"
fi

