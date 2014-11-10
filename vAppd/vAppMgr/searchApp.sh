#! /bin/sh
#
# $1:	pid
# $2:	filename

# check pid in list
retval=`awk -v str="$1" '$1 == str { print $2 }' $2 | wc -l`

if [ "$retval" -ne "1" ] ; then
    echo -1  # pid not in list
else
    echo 0  # pid in list
fi
