#! /bin/sh

# count number 0f line before sed
n1=`wc -l $2 | awk '{print $1}'`

# delete line with pattern
sed -i '/'$1'/d' $2

# count number 0f line before sed
n2=`wc -l $2 | awk '{print $1}'`

result=`expr $n1 - $n2`
if test $result -eq 1; then
    echo 0  # IP deleted
else
    echo 1  # IP not in list
fi

