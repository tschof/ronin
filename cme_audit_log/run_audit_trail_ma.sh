#!/bin/bash
cd /home/romserver/test/cme_audit_log/
destdir="/mnt/w/DART_Administration/Dart_Operations/Operations/AuditLogData/CME_Globex"
for i in {0..1}
do
day=$(date -d "-$i days" +%Y%02m%02d)
year=$(date -d "-$i days" +%Y)
mkdir -p "$destdir/$year/GLBX_DEV/"
scp -v -i ~/.ssh/id_rsa "ddrt-orl-lrom1:~/test/globex/MasterAuditLog$day.*LOG" "$destdir/$year/GLBX_DEV/"
done
for i in {0..0}
do
day=$(date -d "-$i days" +%Y%02m%02d)
perl cme_audit_logger_ma.pl $day RON "_DEV"
#rm *.txt
#perl cme_audit_logger.pl $day RON "_gs"
#perl archive.pl
rm *.txt
#mv *.gz /mnt/ronin_compliance_output/CMEAudit/
done
