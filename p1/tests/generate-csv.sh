#!/bin/bash

OUTPUT=results.csv
TMP=`mktemp`

echo "Vertices,Threads,Method,Time" > $TMP
for file in `ls tests/*.in | grep -v vert`; do
    name=`basename $file | sed 's/\.in//'`
    vertices=`echo $name | awk -F_ '{print $1}'`
    threads=`echo $name | awk -F_ '{print $2}'`
    for i in `seq 4`; do
        msec=`./wtc $i $file | grep Time | awk '{print $2}'`
        echo "$vertices,$threads,$i,$msec" >> $TMP
    done
done

sort -n $TMP > $OUTPUT
rm -f $TMP
