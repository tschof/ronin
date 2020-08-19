#!/bin/sh
max=10000

for((start = 0; start < $max; start++))
do
        nc 10.106.96.90 7007 < nc_log_sender
done
