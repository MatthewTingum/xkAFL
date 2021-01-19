#! /bin/bash

./xkAFL-KVM/kAFL-Fuzzer/kafl_fuzz.py \
    -v \
    -vm_dir /home/user/Documents/ \
    -work_dir ./work \
    --purge \
    -seed_dir ./seed \
    -vm_dir . \
    -mem 64 \
    -ip0 0x00000000-0x0x03FFFFFF

# if working with a debug kernel
# -m 128
# -ip0 0x00000000-0x07FFFFFF
