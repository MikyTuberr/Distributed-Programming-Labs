#!/bin/bash

commands=(
    "wget -O large_file https://ping.virtua.cloud/100MB.bin"
    "ping -c 10 facebook.com"
    "sleep 5 && echo ENDED WAITING"
    "openssl speed -elapsed -evp sha256"
)

pids=()

for command in "${commands[@]}"; do
    eval "$command" &
    pids+=($!)
done

for pid in ${pids[*]}; do
    wait $pid
    echo "PROCESS $pid HAS FINISHED."
done

echo "ALL CHILD PROCESSES HAVE FINISHED."
