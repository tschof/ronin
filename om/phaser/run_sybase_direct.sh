#!/bin/sh

export PHASER_HOME=/home/romserver/test/phaser
export FREETDS=/usr/local/etc/freetds.conf
export SYBASE=/opt/sybase
export SYBASE_OCS=/opt/sybase/OCS-12_5
export SYBASE_OCS_B=/opt/sybase/OCS-12_5/bin
export SYBPLATFORM=nthread_linuxamd64

LD_LIBRARY_PATH=${SYBASE_OCS}/lib:${PHASER_HOME}

export LD_LIBRARY_PATH
cd /home/romserver/test/phaser

echo $LD_LIBRARY_PATH

onload --profile=latency ./dartglob glob.dat glob.conf oats.dat dest_route_tbl.map

echo $FREETDS
