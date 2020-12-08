#!/bin/sh

export PHASER_HOME=/home/romserver/test/phaser
export SYBASE=/usr/local/lib
export FREETDS=/usr/local/etc/freetds.conf

LD_LIBRARY_PATH=$SYBASE:${PHASER_HOME}

export LD_LIBRARY_PATH
cd /home/romserver/test/phaser

echo $LD_LIBRARY_PATH

cgdb --args ./dartglob glob.dat glob.conf oats.dat dest_route_tbl.map

echo $FREETDS
