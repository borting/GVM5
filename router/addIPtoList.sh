#! /bin/sh

# check IP in list
retval=`grep $1 $2 | wc -l`

# add IP to list
if test $retval -eq 0; then
	echo "$1" >> $2
	echo 0
else
	echo 1	# IP is already in list
fi
