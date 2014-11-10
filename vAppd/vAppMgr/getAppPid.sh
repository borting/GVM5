#! /bin/sh
#
# $1:	pvt_port
# $2:	filename

# search pid in list by private port
retval=`awk -v str="$1" '$2 == str { print $1 }' $2`

if test -z $retval ; then
    echo -1  # pid not in list
else
    echo $retval  # return pid
fi
