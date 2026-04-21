#!/bin/bash

# Количество одновременно запускаемых клиентов
CLIENTS_COUNT=5

echo "Starting $CLIENTS_COUNT clients..."

for i in $(seq 1 $CLIENTS_COUNT)
do
    ./client & 
done

# Ждем завершения всех фоновых процессов
wait
echo "All clients finished."