#!/bin/bash

for i in `seq 1 50`
do
 timeout 2 openssl s_client -connect localhost:4432
done
