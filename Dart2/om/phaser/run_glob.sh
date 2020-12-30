#!/bin/sh

if [ "x$1" != "x--" ]; then
$0 -- 1> /home/romserver/test/phaser/cron.log 2> ~/test/phaser_cron.log &
exit 0
fi

. /home/romserver/dart_2_paths

ulimit -c unlimited
export PHASER_HOME=/home/romserver/test/phaser
export FREETDS=/usr/local/etc/freetds.conf

LD_LIBRARY_PATH=/usr/local/lib:${PHASER_HOME}

export LD_LIBRARY_PATH
cd /home/romserver/test/phaser


echo $LD_LIBRARY_PATH

onload --profile=latency ./dartglob glob.dat glob.conf oats.dat dest_route_tbl.map

echo $FREETDS
