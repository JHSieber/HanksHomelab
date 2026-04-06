# HanksHomelab

This is a repo to document my homelab server and my ongoing quest to wrestle back control over my own network and data. As well as to build out alternative cloud infrastructure for my family and friends, to reduce reliance on large tech corporations and being beholden to their ever changing prices and policies.


- [HanksHomelab](#hankshomelab)
    - [How It all started:](#how-it-all-started)
- [Defining my Homelab Mission:](#defining-my-homelab-mission)
  - [Local Control:](#local-control)
  - [Data ownership](#data-ownership)
  - [Privacy/Security](#privacysecurity)
  - [The Plan](#the-plan)
    - [Local Control:](#local-control-1)
    - [Data Ownership:](#data-ownership-1)
  - [The Hardware:](#the-hardware)
  - [The Software:](#the-software)


### How It all started: 
In 2022 I was living in my first out of college apartment and working remotely. I started getting interested in some smart home products to automate devices in the home I was now spending all my time in. However, having just taken cybersecurity class and done some research into data privacy, I had some concerns with many off the shelf products and their required cloud services. 
This led me to setting up my first server, a raspberry pi running homeAssistant OS. I loved the local only aspect of it and learned a lot about the home assistant ecosystem, but I quickly found the limitations of such a small system. I wanted to add storage to the mix, so I started messing around with an old NAS I found from 2014. This allowed me to incorporate data flows, backups, and media into my homelabbing. But since it was running proprietary software, the docker container version of Home Assistant was more limited to my full OS install. So in 2023, I began scouring Facebook marketplace for cheap PC parts until I could finally build my first real server. 

# Defining my Homelab Mission:
## Local Control:
**The Problem**:

I have a lifelong love of gadgets and technology. So naturally the idea of a smarthome that is custom made for my schedule and life has always been a dream. However, while getting my Computer Science degree I began working with embedded systems. This made me thing about the multitude of off the shelf gadgets from no-name brands that all are wifi enabled and have varying levels of insight into your life. I learned about the common vulnerability is the cheap boards used to drive these devices, the cloud-connected video cameras that store your footage at unprotected urls, and the potential of one weak security link in your system allowing a bad actor into the more critical pieces. This has inspired me to ditch the Alexa, and any other major brands smart-home apparatus and try to build the smart home of my dreams while abiding by some rules to keep my privacy and control intact.

**Goals**:
  - Make no compromises on the features and functionality I want in my smart home.
  - All components need to at least have a local only option of operation (Should be able to operate when the internet is down)
  - Partition my network using VLANs to isolate more vulnerable devices from my core network. 
## Data ownership
  **The Problem:** 

In the past decade or so, there has been an alarming shift towards cloud-first/cloud-only subscription based services. These companies justify charging a recurring fee for things that used to be one time, and changing the consumer-company contract to retain ownership over the product you pay for. The move to cloud based software allows users to run services on lesser hardware, but it means that there is less and less that we truly own. And companies can at any time (which we have seen on many occasions) decide to stop supporting a product that you bought, remove content without warning, and always increase what you pay for while degrading the experience with ads and more paywalls. 

**The Goals:** 
  - Host all of my own data with a secure and redundant backup strategy
  - Host much of the media that I consume transferring my cds and dvds onto my own media server
  - Build in as many of the tools that makes the cloud-based solutions compelling:
      - Remote data access.
      - easy data sharing to friends and clients.
      - stream media to any device anywhere.
## Privacy/Security
**The Problem:**
      
Data privacy goes hand in hand with data ownership, and both are critical. Storing personal files on third-party servers introduces real risks, not just from potential breaches, but from how that data gets used. Privacy policies have grown increasingly opaque, and the default assumption for most services is that your data is an asset to be leveraged. The result is systems built to predict and influence your behavior, based on your own information, that someone else profits from. Tradeoffs are inevitable in the modern digital landscape, but I think the bar most people have accepted is too low. This homelab is my attempt to raise that bar for myself.

**The Goals:**
  - Securely store personal data locally
  - Reduce my digital footprint:
      Location tracking
      Internet activity monitoring
      Purchase information
  - Maintain a comparable ease of use for all services
  - Reduce exposure to data breaches and compromised companies
            

## The Plan
### Local Control:
**Smart home core:** [Home Assistant](https://www.home-assistant.io/)
  - While there are some other promising self-hosted smart home platforms out there, Home Assistant was an easy choice to make. Produced by the [Open Home Foundation](https://www.openhomefoundation.org/), Home Assistant (or HA) is an open source project that has been making steady improvements to keep up with the propriety smart home ecosystem. With a tremendous community supporting the effort, I have been able to find support/guides to connect pretty much anything I can think of in one way or another to their software.
  - With Home Assistant, the possibilities of the functionality is really just up to the user. It is incredibly versatile allowing complex automation flows, scripted actions, and a wealth of supported third-party devices. The tradeoff spending a lot more time pouring through forums and documentation to find the combination of tools to suit your situation.

**Putting the 'Local' in Local Control:**
  - **The Devices:**
  There are two pieces necessary to have a functioning smart home that works offline. The first is the devices you want to use need to be ok without talking to their cloud and operating solely on your LAN. Commercial smart home devices range from cloud-reliant (need a constant internet connection to function), cloud-preferred (get updates from the cloud or have some services that rely on an internet connection), to cloud-optional (where they are just as happy with or without a WAN connection to their cloud services). There are also some devices that are designed to be offline only with no cloud features built in whether internet connected or not. Most of the devices I have come across are in the first two categories, which has led me to opt for building my own offline-native devices where I cannot find a suitable off the shelf solution (Or I want to save a buck).  

  - **The Infrastructure:**
    The second thing I need to do is provide an alternate control network that can be siloed and offline while communicating with all of the IOT (Internet of Things) devices. For this, I turned to a VLAN (Virtual Local Area Network) on my main router that can segment the traffic that goes through one or more of the ethernet ports and prevents it from communicating with the rest of the ports (unless I specify otherwise). From this VLAN designated port, I connected a network switch, which feeds any wired in devices for this network. The switch then plugs into to the main router of a set of three 2.4Ghz mesh routers. This allows me to have a wide area of coverage for the IOT network, and since it is 2.4Ghz, compatibility with cheap or older devices that do not support 5Ghz is not an issue. The other devices wired into the VLAN switch are a wifi hub for leak sensers, and of course my server running Home Assistant. (more on this in the network section)

### Data Ownership:  
**Hosting my data:**
- **Storage:** Self hosting data can be very easy or pretty involved depending on how you define it. You could say that having a laptop with all your files on it is retaining control of your data, but it does not give you the assurance and protections that cloud backups offer. For me I wanted to have essential parity with cloud services. This means following the 3-2-1 backup rule of thumb: 3 copies of your important data, on 2 different storage mediums, with 1 copy offsite. The backbone of my data storage comes from my server running UnraidOS. My configuration is similar to RAID 4, where there is one dedicated parity disk that allows any single drive in the system to completely fail with no data loss (if the failed drive can be replaced before another drive goes down). One small difference with Unraid's implementation is that the data is not striped across all drives. This means in the event of a second drive failing, then only the data stored specifically on that drive is lost instead of all the data across all drives. This comes at the cost of not gaining performance benefits for random read/writes as more disks are added to the array. 
 
- **Backup & Sync:** For standard device backups, I have scheduled weekly documents and time machine backups from my Desktop PC and my Mac Laptop. But the majority of the data I want backed up by volume is my photographs and videos from personal and professional work which I want to handle a little differently. Because of how I have structured my photo workflow between my Mac laptop, Desktop PC, and Mobile phone, I wanted more granular control over what gets synced, to which devices, and when it happens. To achieve this, I am running [Syncthing](https://syncthing.net/) which checks all my boxes. It is open-source, private/local only, encrypted communications, with strong cryptographic authentication for each device you share data with. Syncthing allows me to offload an SD card into lightroom on my Mac and do an initial pass of culling, rating, and tagging images. Then open lightroom on my PC to complete the edits on a large color accurate monitor. Then back on my Mac export the edits for easy sharing. As well as syncing the export folder to my Phone for social media sharing. And crucially, with ignore patterns, I can ensure that each device only store relevant/necessary folders locally to prevent my phone or laptop from being filled up by one folder of 4k video from a shoot. (more on this setup in the full photo flow section).  

**Hosting Media**:

- **The Server:** Like many people who grew up before the advent of the smartphone, I have a large library of CDs, DVDs, tapes, and other physical or digital media that I purchased before age of streaming. As I have been on this mission to both reduce my recurring costs, and claw back ownership/control over my digital live, my media streaming subscriptions were on the chopping block. Since I had most of the media I wanted to access already, the main solution I needed was a way to access the media in a netflix-esque way. I first looked to [Plex](https://watch.plex.tv/), as I have heard many people extolling its utility for years. I went through the configuration process on all the relevant devices, moved around a lot of media, and got it all working! I was very happy with its sleek design and overall ease of use. This list ended however, when I prepared for an upcoming trip and decided to tackle the remote access portion of the implementation which I had been putting off. Plex is a self-hosting mainstay, but they also offer their own streaming platform which can be combined with your local instance for increased utility at a cost. One of the utilities is being able to access your local media through their secure servers and connection. I had come accross many guides and forums discussing how to access your own plex server remotely using vpn tunnels, reverse proxies, and the likes. However, it seems that recently Plex has been pushing their subscription side a bit more by detecting remote connections and preventing you from accessing your media while not on your local network. After many attempts to bypass this with nxginx proxy manager, tailscale VPN on my server, wireguard vpn on my router, and so on. I ended up switched to a popular plex alternative called [JellyFin](https://jellyfin.org/). JellyFin certainly lacks some of the polish of plex, but makes up for it with open source developement, 100% free model, and an act ive contributing community around it.
 
- **Remote Connection:** Once I had switched from Plex to Jellyfin, I was able to configure my remote connections. I chose to use [Tailscale](https://tailscale.com/) because they offer a free personal plan that met all my needs and they are deeply integration with UnraidOS. Once I had created my Tailnet and added my devices (Server, PC, Mac, Smartphone, ... ) I configured my server to the be an exit node for the net enabling access to my LAN as if I were a local device. This allows all of my locally hosted services to work on a remote device as if it were there, and lets me control/manage my other server operations on the go.
 
- **File Sharing**: This was an important one for me. For years I have relied on Google drive as a quick and easy way to upload files from any device, and share them with friends, family, or clients. I wanted a self-hosted alternative that was a clean professional experience for the end user. I did not want a client to have to connect to my server via a VPN or do some other convoluted process to access their photos. The Tailnet VPN worked well for media streaming, since it is only being accessed by myself and a few close friends. But the complexity of the initial setup is not worth it if all that is needed is to download some files one time. I still wanted privacy and security to be core to my setup, and if possible to avoid exposing my server to the wide wide web out there. Given my needs and aided by the fact that I already own a website domain for my professional website, I settled on using [Nginx Proxy Manager](https://nginxproxymanager.com/)(NPM). What Nginx allows me to do, is securely redirect a subdomain of my website to send traffic to a specified port on my server. Using a free SSL certificate from [Let's Encrypt](https://letsencrypt.org/), I can open up a docker container to the internet without exposing my local IP address or having direct connections to my network. And for service at the end of the tunnel, I chose to use [FileBrowser Quantum](https://github.com/gtsteffaniak/filebrowser). It is forked from [File Browser](https://github.com/filebrowser/filebrowser) which I had been using originally. While the functionality was solid, the project had entered maintenance-only mode, and FileBrowser Quantum offered a much sleeker UI for the end-user. Also, since I am using a reverse proxy, I can either create a login per client (if I will be sharing ongoing work), or I can generate a share link that expires automatically, so people can download the edit from an event by just going to a link. This was one of the core uses I wanted to get finished and polished for my professional needs, and think I have definitely reached or surpassed parity with Google drive for my needs.  




## The Hardware:
- Motherboard: MSI B360M BAZOOKA
- CPU: Intel Core i5-8400 CPU
- GPU: NVIDIA GTX 750ti
- RAM: 16GB DDR4
- PSU: EVGA Supernova 550W Gold
- Case: Fractal Design 7
- Storage:
    - Storage Array:
        - Parity: 14TB HDD
        - Disk 1: 14TB HDD
        - Disk 2: 2TB HDD
        - Disk 3: 6TB HDD
        - Disk 4: 6TB HDD
    - Cache pool:
        - 4TB NVMe SSD
    - Boot device:
        - 128GB USB drive

## The Software:
**OS:** Unraid OS Plus

**Virtual Machines:**
- Home Assistant OS

**Docker Containers:**
- Media:
  - JellyFin
- Data: 
  - File Browser Quantum
  - Nginx Proxy Manager
  - Syncthing
- Services:
  - Nginx website hosting
  - Immich
  - ffmpeg
  - NextCloud  
