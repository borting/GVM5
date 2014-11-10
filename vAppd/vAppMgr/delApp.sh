#! /bin/sh
#
# $1:	pid
# $2:	filename

# delete line contains pid
sed -i '/^'$1'[ ]/d' $2

echo 0
