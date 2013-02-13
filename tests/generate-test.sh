#!/bin/bash

N=$1

if [ -z $1 ]; then
    N=4
fi

echo 1
echo $N

for x in `seq $(($N-1))`; do
    tmp=`expr 1 + $x`
    echo "$x $tmp"
done
