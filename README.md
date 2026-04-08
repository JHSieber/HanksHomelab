# HanksHomelab
A self-hosted home server built to replace cloud services with local infrastructure. Covering smart home automation, embedded systems sensors, media streaming, file storage, and secure remote access. Built on UnraidOS with a focus on local control, data ownership, and network security.


## How It Started
In 2022, while working remotely in my first apartment, I started exploring smart home automation. A background in embedded systems and a recent cybersecurity course made me wary of the security and privacy tradeoffs in most off-the-shelf products. I started with a Raspberry Pi running Home Assistant, outgrew it quickly, and by 2023 had built a full fledged Linux server from secondhand parts.


## Goals
**Local Control —** A smart home that works without an internet connection, with IoT devices isolated on a dedicated VLAN away from my core network devices.

**Data Ownership —** Self-hosted storage following the 3-2-1 backup rule, with a media server to replace streaming subscriptions and a file sharing solution for professional use.

**Privacy & Security —** Personal data stored locally, minimal digital footprint, and no dependence on third-party servers or opaque privacy policies.



## Architecture Overview

- OS: [UnraidOS](https://unraid.net/) Plus
- Program management: [Docker](https://www.docker.com/)
- Smart Home: [Home Assistant](https://www.home-assistant.io/) OS (VM)
- Media Server: [JellyFin](https://jellyfin.org/)
- File Sync: [Syncthing](https://syncthing.net/)
- File Sharing: [FileBrowser Quantum](https://github.com/gtsteffaniak/filebrowser) + [Nginx Proxy Manager](https://nginxproxymanager.com/)
- Remote Access:: [Tailscale](https://tailscale.com/)
- Photo Management: [Immich](https://immich.app/)

### Network
IoT devices run on a dedicated 2.4GHz mesh on a VLAN, isolated from the core network via a managed switch. Home Assistant runs as a VM with direct access to the IoT VLAN. Remote access is handled through [Tailscale](https://tailscale.com/), with the server configured as an exit node so all locally hosted services are accessible remotely without exposing the network directly.
For external file sharing, [Nginx Proxy Manager](https://nginxproxymanager.com/)(NPM) routes a subdomain to specific container ports using a [Let's Encrypt](https://letsencrypt.org/) SSL certificate and front end managed with [FileBrowser Quantum](https://github.com/gtsteffaniak/filebrowser). No local IP exposure, no VPN required for end users, google drive for my friends.

### Storage
Unraid array configured operates similar to a RAID 4 array. One dedicated parity disk allows any single drive to fail without data loss. Unlike traditional RAID, data is not striped, so a second drive failure only affects data on that specific disk. This sacrifices speed of writing across all disks at ones, but the cache pool makes up for this with fast read/writes before data moves to the array.

- Parity: 14TB HDD
- Array: 14TB + 6TB + 6TB + 2TB HDD
- Cache: 4TB NVMe SSD

Weekly backups from Mac and PC via Time Machine and scheduled jobs. Photos and video sync across devices using Syncthing with per-device ignore patterns to control what gets stored where.

### Hardware:

Component Spec
- Motherboard: MSI B360M Bazooka
- CPU: Intel Core i5-8400
- RAM: 16GB DDR4
- GPU: NVIDIA GTX 750Ti
- PSU: EVGA Supernova 550W Gold
- Case: Fractal Design 7
- Boot: 128GB USB

Full Writeup
For the complete breakdown of each component, configuration decisions, and lessons learned including the full smart home setup, photo workflow, and media pipeline checkout [Hank's Homelab](https://hank.solutions/programming/) on my website. Or read the [Directors Cut Readme](DIRECTORSCUTREADME.md) here.
