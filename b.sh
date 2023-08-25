#!/bin/sh
export FORCE_UNSAFE_CONFIGURE=1

op="`pwd`/OpenWRT-LEDE"
cd op
./scripts/build.sh qsinas_transcode build 1.1.0.6 1.1.0.6
