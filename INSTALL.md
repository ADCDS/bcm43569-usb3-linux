# Install and remove

These commands are for Debian on x86-64. Run them from the repository root.
The released source archive contains `usb_quirk/Makefile`, `dkms.conf`,
`bcm43569_config_quirk.c`, and `modprobe.conf`.

## Prerequisites

Install `build-essential`, `dkms`, matching kernel headers, and
`firmware-brcm80211`. Debian may require the `non-free-firmware` repository
component for the firmware package.

```sh
sudo apt update
sudo apt install build-essential dkms "linux-headers-$(uname -r)" firmware-brcm80211
```

## Install module

```sh
version=0.1.0
sudo install -d -m 0755 "/usr/src/bcm43569-usb3-quirk-$version"
sudo install -m 0644 usb_quirk/Makefile usb_quirk/dkms.conf \
  usb_quirk/bcm43569_config_quirk.c "/usr/src/bcm43569-usb3-quirk-$version/"
sudo dkms add -m bcm43569-usb3-quirk -v "$version"
sudo dkms build -m bcm43569-usb3-quirk -v "$version" -k "$(uname -r)" -j 2
sudo dkms install -m bcm43569-usb3-quirk -v "$version" -k "$(uname -r)"
sudo install -m 0644 usb_quirk/modprobe.conf \
  /etc/modprobe.d/bcm43569-usb3-quirk.conf
sudo modprobe bcm43569_config_quirk
```

Check `dkms status -m bcm43569-usb3-quirk` and
`modprobe --show-depends brcmfmac`; the quirk should appear before
`brcmfmac`. Physically unplug and reconnect the adapter to trigger a fresh
USB 3.0 enumeration. On the tested host, software reset or port cycling did
not replace a physical replug reliably.

## Remove module

Arrange another network connection first and disconnect the adapter. Then:

```sh
sudo rm /etc/modprobe.d/bcm43569-usb3-quirk.conf
sudo modprobe -r bcm43569_config_quirk
sudo dkms remove -m bcm43569-usb3-quirk -v 0.1.0 --all
```

DKMS removal does not delete the source copy under
`/usr/src/bcm43569-usb3-quirk-0.1.0`; remove that exact directory separately
only if you no longer want to keep the installed source.
