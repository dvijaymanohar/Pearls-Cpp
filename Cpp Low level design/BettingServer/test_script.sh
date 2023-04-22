#!/bin/bash
TRY=0
while (( $TRY < 40)); do
    echo "Client: $TRY"
    ./betting_client 172.30.12.129 2222 &
    (( TRY = $TRY + 1))
done

