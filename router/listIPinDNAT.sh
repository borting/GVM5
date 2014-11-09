#!/bin/sh

# List all DNAT ports used by a particular IP
result=`iptables -t nat -L PREROUTING -n | sed -e '1,2d' | \
	grep ":$1:" | awk -F'[ :]+' '{print $8, $11}'`

echo "$result"

