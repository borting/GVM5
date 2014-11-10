#! /bin/sh
#
# 1: pid
# 2: cmd

# search pid and return command name
EXECCMD=`ps axo pid,cmd | awk -v str=$1 '$1 == str { print $2 }'`

if test -z $EXECCMD ; then
    echo -2	# pid not existed
elif [ "$EXECCMD" == $2 ]; then
    echo 0	# expected cmd
else
    echo -1	# not expected cmd
fi
