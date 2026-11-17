# Building and running sedutil-cli on a Raspberry Pi

This is how sedutil-cli was brought up on a Raspberry Pi 4 (Raspbian GNU/Linux 13 "trixie",
armv7l, gcc 14.2) in November 2026 to test a SATA SED through a USB-SATA bridge, recovered
from that Pi's shell history.

## Build

```bash
sudo apt update && sudo apt install -y git build-essential autoconf automake libtool gawk
git clone https://github.com/Drive-Trust-Alliance/sedutil.git
cd sedutil
autoreconf -i
./configure --enable-silent-rules
# `make Version.h` / linux/Version.sh did not produce Common/Version.h on this tree; write it by hand:
echo "#define GIT_VERSION \"$(git describe --always --dirty)\"" > Common/Version.h
make sedutil-cli -j4
sudo cp sedutil-cli /usr/local/bin
```

## Let libata pass TCG commands through

The kernel refuses ATA TRUSTED SEND/RECEIVE unless libata is told to allow them.
Append the parameter to the kernel command line and reboot:

```bash
sudo sed -i 's/$/ libata.allow_tpm=1/' /boot/firmware/cmdline.txt
sudo reboot
cat /sys/module/libata/parameters/allow_tpm    # expect 1
```

## Run

```bash
sudo sedutil-cli --scan
sudo sedutil-cli --query /dev/sda
```

On master as of 5af6caf, `--query` segfaulted in `DtaDev::discovery0()`: the Discovery0
header's length was trusted and the feature walk ran off the end of the 2048-byte buffer.
The branch carrying this file bounds that length. `gdb -batch -ex "run --query /dev/sda"
-ex bt ./sedutil-cli` is what located it; `hdparm -I /dev/sda` is a useful cross-check of
what the drive reports through the bridge.

## Network

The Pi had no LAN of its own: it was cabled to a Mac sharing its connection (macOS Internet
Sharing, bridge100, 192.168.2.0/24), where it came up at 192.168.2.2.
