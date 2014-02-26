#!/bin/bash

shuf "$1" | head -n "$2" | while read domain; do
    tr -cd '[:alnum:]' < /dev/urandom | fold -w80 | head -n "$3" | while read localpath; do
        echo "$domain/$localpath"
    done
done
