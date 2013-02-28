#!/bin/bash

N=$1
THREADS=$2

if [ -z $1 ]; then
    N=4
fi

if [ -z $2 ]; then
    THREADS=1
fi

echo $THREADS
echo $N

for x in `seq $(($N-1))`; do
    tmp=`expr 1 + $x`
    echo "$x $tmp"
done
