# 🖥️ Virtual Machine: Home Assistant

> KVM/QEMU virtual machine running on Unraid — Home Assistant OS

---

## Overview

| Property | Value |
|----------|-------|
| **Name** | _____ |
| **UUID** | `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxx` |
| **Type** | KVM (HVM) |
| **Template** | Linux (`linux.png`) |
| **OS** | Linux (Home Assistant OS) |
| **Machine Type** | `pc-q35-7.1` |
| **Architecture** | `x86_64` |

---

## Hardware

###  Memory

| Property | Value |
|----------|-------|
| **Total RAM** | 2 GB (2,097,152 KiB) |
| **Memory Sharing** | Disabled (`nosharepages`) |

###  CPU

| Property | Value |
|----------|-------|
| **vCPUs** | 2 |
| **Placement** | Static |
| **Mode** | Host Passthrough |
| **Topology** | 1 socket × 2 cores × 1 thread |
| **Cache Mode** | Passthrough |
| **CPU Pinning** | vCPU 0 → Host CPU 0, vCPU 1 → Host CPU 1 |

###  Storage

| Device | Type | Bus | Boot Order | Path |
|--------|------|-----|------------|------|
| `hdc` | QCOW2 (with backing store) | SATA | 1st | `/mnt/user/domains/haos_ova-12.3.rc2.pre17.0 update--generateqcow2` |

**Backing Store:** `/mnt/user/domains/haos_ova-12.3.rc2.qcow2`  
**Disk Cache:** `writeback`

###  Network

| Property | Value |
|----------|-------|
| **Interface Type** | Bridge |
| **Bridge** | `br0` |
| **MAC Address** | `xx:xx:xx:xx:xx:xx` |
| **Model** | `virtio-net` |
| **Target Device** | `vnet0` |

---

## Firmware & Boot

| Property | Value |
|----------|-------|
| **Firmware** | UEFI (OVMF Pure EFI) |
| **Loader** | `/usr/share/qemu/ovmf-x64/OVMF_CODE-pure-efi.fd` *(read-only, pflash)* |
| **NVRAM** | `/etc/libvirt/qemu/nvram/xxxxxx-xxxx-xxxx-xxxx-xxxxxxx_VARS-pure-efi.fd` |

---

## Display & Input

| Property | Value |
|----------|-------|
| **Video Model** | QXL |
| **Video RAM** | 64 MB |
| **VGA Memory** | 16 MB |
| **Heads** | 1 |
| **VNC Port** | `5900` (autoport) |
| **VNC WebSocket** | `5700` |
| **VNC Listen** | `0.0.0.0` (all interfaces) |
| **Keymap** | `en-us` |
| **Audio** | None |
| **Input Devices** | USB Tablet, PS/2 Mouse, PS/2 Keyboard |

---

## Lifecycle & Features

| Event | Action |
|-------|--------|
| **Power Off** | Destroy |
| **Reboot** | Restart |
| **Crash** | Restart |
| **Watchdog** | iTCO — Reset |

**Features enabled:** ACPI, APIC

---

## Clock Configuration

| Timer | Policy |
|-------|--------|
| RTC | `catchup` |
| PIT | `delay` |
| HPET | Disabled |

---

## Other Devices

- **USB Controller:** ICH9 EHCI + UHCI (1/2/3)
- **VirtIO Serial:** Enabled (for QEMU guest agent)
- **QEMU Guest Agent:** Connected via Unix socket at `/run/libvirt/qemu/channel/1-Janice/org.qemu.guest_agent.0`
- **Memory Balloon:** VirtIO
- **Emulator:** `/usr/local/sbin/qemu`

---

## Security

| Property | Value |
|----------|-------|
| **Security Model** | DAC (dynamic) |
| **Label** | `+0:+100` |
| **Image Label** | `+0:+100` |