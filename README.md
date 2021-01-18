# xkAFL

xkAFL is an original Xbox kernel fuzzer utilizing
[kAFL](https://github.com/IntelLabs/kAFL)
and
[XQEMU](https://github.com/xqemu/xqemu)

## Getting Started

**Requirements**
- A computer running Linux
- An Intel CPU supporting Intel Processor Trace
- Python 3

`git clone --recurse-submodules -j `nproc` https://github.com/MatthewTingum/xkAFL.git`

### Building XQEMU
XQEMU has been patched to support kAFL.

Install XQEMU build dependencies:
```sh
sudo apt-get update
sudo apt-get install git build-essential pkg-config libsdl2-dev \
libepoxy-dev zlib1g-dev libpixman-1-dev
```

Build XQEMU with the following commands:
```sh
cd xqemu-kafl
./build.sh --python=/usr/bin/python3
```


### Installing kAFL
Instructions for installing kAFL can be found in the `xkAFL-KVM` directory.
For our purposes, we will need to install the kAFL KVM module. The process
to do this involves compiling and installing a Linux kernel so it may take
some time.

Run the following commands:
```sh
cd xkAFL-KVM
./install.sh deps
./install.sh perms
./install.sh linux

# Follow additional steps to install the kernel
./install.sh note

# Reboot
sudo reboot
```

Once the system has rebooted, check that the your processor is supported
```sh
$ dmesg|grep VMX
 [VMX-PT] Info:   CPU is supported!
```

`cd` back into the `xkAFL-KVM` directory and run
```sh
kAFL-Fuzzer/kafl_fuzz.py
```

Install python dependencies as needed.

# TODO: Set relative path to xqemu binary in kAFL ini
# TODO: Set relative paths in modified common.py


### Building the test harness
# TODO: This has not been merged to main yet


### XQEMU Required Files
XQEMU requires an MCPX boot rom image, a kernel image, and a hard disk image.
You will need to source these on your own.
See the [XQEMU Getting Started](https://xqemu.com/getting-started/#required-files)
guide for details.

This repo contains dummy (empty) files in place of the XQEMU required files.
Once you have obtained the required files, overwrite the dummy files with the real ones.
The files needing to be replaced are:
```
xkAFL-vm/mcpx_1.0.bin
xkAFL-vm/bios.bin
xkAFL-vm/xbox_hdd.qcow2
```


### Generating a snapshot
Create an overlay file of the xbox hard disk image:
```sh
qemu-img create -f qcow2 -b xkAFL-vm/xbox_hdd.qcow2 xkAFL-vm/overlay_0.qcow2
```

Assuming everything has been built and configured correctly,
the sample harness can be tested.


Run `demo_snapshot.sh`. XQEMU will boot with GUI enabled. After running through the boot
animation, the sample harness program will run and automatically take a snapshot.
XQEMU will close by itself.

Veryify that a *snapshot* has been created.


### Fuzzing
With a snapshot created, fuzzing can begin.

Run `demo_fuzz.sh`. A kAFL banner will display in the terminal.
Open another terminal and run:
```sh
./kAFL/kAFL-Fuzzer/kafl_gui.py ./work/
```

A GUI will appear displaying fuzzing statistics.
As crashes are found, the payload used to trigger them will appear in
`./work/corpus/crashes/`

The sample should generate two unique crashes:
- A payload beginning with the character `X`
- A payload beginning with the characters `BX`
