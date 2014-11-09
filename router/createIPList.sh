#! /bin/sh

# check whether file is existed
if [ -f $1 ]; then
	echo 1		# file existed
else
	echo 0
fi

# truncate a new file
:> $1
