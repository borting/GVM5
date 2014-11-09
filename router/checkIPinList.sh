#! /bin/sh

result=`grep $1 $2 | wc -l`

if test $result = "1"; then
    echo 0  # IP in list
else
    echo 1  # IP not in list
fi

