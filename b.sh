#!/bin/bash

echo building @ $(date +%H:%M)
gcc -Wall -Wextra -O2 -g server.c common.c -o server
gcc -Wall -Wextra -O2 -g client.c common.c -o client
echo "============================================================================="
