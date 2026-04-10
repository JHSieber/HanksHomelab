# Photo Workflow
A place to document pieces of my photography workflow from capture/offload to final delivery.

## Overview

1. Capture photos
2. Offload SD onto MacBook in Lightroom
3. Create new dated event folder
4. Automatically sync across Mac, Desktop, and Server
5. Automatically group, process, and tag HDR photos
6. Cull through photos on Mac
7. Edit photos on Desktop
8. Export Final versions into pre-generated folder
9. Automatically sync new edits to phone for social media sharing
10. Automatically link export folder to file-sharing container on server for easy client sharing


## Syncing
My full photo/video library now takes up around 4.5TB on a good day. This posed an issue with the total parity route I used to use across my PC and my Mac (via an external SSD). With the opportunity to rework how I approach data, I decided to tailor the sync across 4 devices depending on their needs and capacities. 

### [Unraid Server](Server.stignore)
This one is pretty straightforward. My server is always the 'Golden Copy' retaining every file in my photo and video library with it's 28TB of storage. With it's 14TB parity drive, I have some peace of mind for my data stored here, although I plan to introduce another bulk cold storage backup server for an extra layer of assurance. 
- Total capacity: `28TB HDD Pool`
- Photos tracked: `All`
- Videos tracked: `All`

### [Desktop PC](PC.stignore)
My Desktop used to also be a golden copy mostly stored on an external 4TB SSD that I would take with me to use with my laptop when I traveled. But having finally outgrown that space I had to pare back what was stored a little bit. I decided to still keep all photographs but for videos, which take up the bulk of the space, I only retain the most recent two years, along with any active editing projects/footage. 
- Total Capacity: `4TB SSD + 6TB HDD`
- Photos tracked: `All`
- Videos tracked: `Most recent 2 years + Active projects`


### [Macbook Pro](Mac.stignore)
My laptop serves as my offload + edit station when I am traveling, so I wanted it to be able to fit into the broader system without always having to delete files before every offload. With it's 2TB of storage (refurbished deals ftw), I am able to store a good amount of my library on it while having space for new shoots. The setup ends up similar to my PC, but limiting the photos to 2 years of recency, and only active video projects making the cut. This lets me finish and deliver client videos on the go without clogging the drive with too much video. And with my Tailscale VPN tunnel setup, I can remotely connect to my server if I come across wifi on the road. This lets me sync new media with the rest of my sync chain for added redundancy and ease of sharing.  
- Total Capacity: `2TB SSD`
- Photos tracked: `Most recent 2 years`
- Videos tracked: `Active projects`


### Iphone
Lastly my phone. It has very little space compared to all the other devices, but also my needs on it are very different. I do not need to do any offloading, editing, or viewing on my phone so I do not need to worry about storing large RAW and MP4 files on it. What I do use it for is posting the finished products on social media. Because the final edits are the only part of the process I am concerned with, I only sync the master "Edits" folder for my photos. This means that as soon as I finish exporting from my PC or mac, I can open my phone to access the high quality media without needed to airdrop, text, or email them to myself which was my previous method. 
- Total Capacity: `512GB SSD`
- Photos tracked: `Only final JPG edits`
- Videos tracked: `none`