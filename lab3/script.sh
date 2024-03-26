#!/bin/bash

commands=(
    "wget -O large_file https://ash-speed.hetzner.com/100MB.bin"
    "ping -c 10 facebook.com"
    "sleep 5 && echo ENDED WAITING"
    "openssl speed -elapsed -evp sha256"
)

for command in "${commands[@]}"; do
    eval "$command" &
done

wait
echo "ALL CHILD PROCESSES HAVE FINISHED."
