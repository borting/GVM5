#! /bin/sh
#
# $1:	pid
# $2:	filename

# check pid in list, and get private port if pid exist
retval=`awk -v str="$1" '$1 == str { print $2 }' $2`

if test -z $retval ; then
    echo -1  # pid not in list
else
    echo $retval  # pid in list, return private port
fi
