#!/bin/bash
suffix=$(date +%Y%02m)
day=$(date +%Y%02m%02d)

echo "Date= $suffix"
echo "Day= $day"
cd /home/romserver/prod/globex6/

perl cme_audit_logger.pl $day

