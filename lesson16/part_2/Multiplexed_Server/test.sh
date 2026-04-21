#!/bin/bash

CLIENTS_COUNT=5

echo "Starting $CLIENTS_COUNT clients..."

for i in $(seq 1 $CLIENTS_COUNT)
do
    ./client_tcp &
    ./client_udp & 
done

wait
echo "All clients finished."