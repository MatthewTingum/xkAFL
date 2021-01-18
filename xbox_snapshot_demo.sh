#! /bin/bash

xqemu-kafl/i386-softmmu/qemu-system-i386 \
    -cpu pentium3 \
    -machine xbox,accel=kvm:hax:whpx,kernel_irqchip=off,bootrom=./xkAFL-vm/mcpx_1.0.bin \
    -m 64 \
    -bios ./xkAFL-vm/bios.bin \
    -drive file=./xkAFL-vm/overlay_0.qcow2,index=0,media=disk,locked \
    -drive index=1,media=cdrom,file=./harness/demo/demo.iso \
    -display sdl
