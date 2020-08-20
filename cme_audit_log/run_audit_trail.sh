#!/bin/bash
cd /home/romserver/prod/cme_audit_log/
destdir="/mnt/w/DART_Administration/Dart_Operations/Operations/AuditLogData/CME_Globex"
for i in {0..1}
do
day=$(date -d "-$i days" +%Y%02m%02d)
year=$(date -d "-$i days" +%Y)
mkdir -p "$destdir/$year/GLBX6/"
scp -v -i ~/.ssh/id_rsa "pdrt-orl-lrom1:~/prod/globex6/Glbx6Log$day.*LOG" "$destdir/$year/GLBX6/"
mkdir -p "$destdir/$year/GLBX6_OPT/"
scp -v -i ~/.ssh/id_rsa "pdrt-orl-lrom1:~/prod/globex6_opt/Glbx6oLog$day.*LOG" "$destdir/$year/GLBX6_OPT/"
mkdir -p "$destdir/$year/GLBX6XM/"
scp -v -i ~/.ssh/id_rsa "pdrt-orl-lrom1:~/prod/globex6xm/Glbx6xmLog$day.*LOG" "$destdir/$year/GLBX6XM/"
mkdir -p "$destdir/$year/GLBX_GS/"
scp -v -i ~/.ssh/id_rsa "pdrt-orl-lrom1:~/prod/globex_gs/Glbx_gsLog$day.*LOG" "$destdir/$year/GLBX_GS/"
mkdir -p "$destdir/$year/GLBX_GSXM/"
scp -v -i ~/.ssh/id_rsa "pdrt-orl-lrom1:~/prod/globex_gsxm/Glbx_gsxmLog$day.*LOG" "$destdir/$year/GLBX_GSXM/"
done
for i in {0..0}
do
day=$(date -d "-$i days" +%Y%02m%02d)
perl cme_audit_logger.pl $day RON "6"
rm *.txt
perl cme_audit_logger.pl $day RON "_gs"
perl archive.pl
rm *.txt
mv *.gz /mnt/ronin_compliance_output/CMEAudit/
done
