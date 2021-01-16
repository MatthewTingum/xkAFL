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


### Preparing XQEMU

# TODO: building hard drive
# TODO: bios / mcpx


### Generating a snapshot
# TODO:


### Fuzzing
# TODO:
