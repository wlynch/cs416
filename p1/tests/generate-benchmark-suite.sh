#!/bin/bash

vertices=32
threads=2

while [ $vertices -le 1024 ]
do
    while [ $threads -le $vertices ]
    do
        sh ./generate-test.sh $vertices $threads >  "${vertices}_${threads}.in"
        threads=$(( threads * 4 ))
    done
    threads=2
    vertices=$(( vertices + 32 ))
done
