#!/bin/sh
cd ..
make
cd -
time ./audio_filter $@
