#!/bin/bash

grep -P "^[a-z]{8,12}$" "$1" | while read word; do
    while read tld; do
        echo "www.$word.$tld"
    done < "$2" 
done
