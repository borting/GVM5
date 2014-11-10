#! /bin/sh
#
# $1:	pid
# $2:	state
# $3:	filename

# search line contains pid and update its third column (state)
sed -i '/^'$1'[ ]/{s/ [^ ]*/ '$2'/2}' $3

echo 0
