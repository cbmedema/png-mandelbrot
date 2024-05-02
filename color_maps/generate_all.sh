#!/bin/bash

bits=$1
range=$((2**bits))
factor=$((255/($range-1)))
inc=$factor

for((i=0; i<$((range*factor));i+=inc)); do
	for((j=0; j<$((range*factor)); j+=inc)); do
		for((k=0; k<$((range*factor)); k+=inc)); do
			echo "$i, $j, $k"
		done
	done
done
