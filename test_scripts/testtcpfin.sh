#!/bin/bash

for i in `seq 1 50`
do
 timeout 2 nc localhost 12345
done
