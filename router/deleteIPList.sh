#! /bin/sh

# check then remove file
if [ -f $1 ]; then
	rm -f $1
	echo 0
else
	echo 1	# file not existed
fi
