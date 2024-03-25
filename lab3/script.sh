#!/bin/bash

commands=(
    "wget -O large_file https://ash-speed.hetzner.com/100MB.bin"
    "ping -c 10 facebook.com"
    "sleep 5"
    "openssl speed -elapsed -evp sha256"
)

execute_command_parallel() {
    "$@" &
}

for command in "${commands[@]}"; do
    execute_command_parallel $command
done

wait

echo "ALL CHILD PROCESSES HAVE FINISHED."
