#! /bin/sh
#
# $1:	pid
# $2:	port
# $3:	filename

# search line contains pid and update its second column (port)
sed -i '/^'$1'[ ]/{s/ [^ ]*/ '$2'/1}' $3

echo 0
