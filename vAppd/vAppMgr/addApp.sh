#! /bin/sh
#
# $1:	pid
# $2:	pvt_port
# $3:	state
# $4:	filename

# append new App's pid, port, state to list
echo "$1 $2 $3" >> $4

echo 0
