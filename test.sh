#!/bin/bash

for i in {1..10}
do
    ./bin/$1
    if diff input.txt output.txt > /dev/null ; then
        echo "[$i] Files are the same"
    else
        echo "[$i] Files are different"
        exit 0
    fi
done

echo "All files are the same"