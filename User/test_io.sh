#!/bin/sh

set -e

#start_time=$(date +%s)

declare -a array=(/usr /home /etc /bin /usr /home /etc /)
declare -a fileprob=(0.5 0.8 0.9 0.3 0.9 0.9 0.5 0.3)
declare -a dirfactor=(0.7 0.8 0.8 0.6 0.8 0.8 0.7 0.6)

#count=${#array[@]}
count=8
mkdir -p output

for (( i=0; i< $count; i++ ))
do
    (echo $BASHPID > output/$i.pid
    while true
        do
        randread  ${array[$i]} $i$RANDOM ${fileprob[$i]} ${dirfactor[$i]} 2>/dev/null >/dev/null
    done)&
done
