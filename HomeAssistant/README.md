# Home Assistant
My HomeAssistant instance is being run as a Virtual machine on my Unraid server. Here I will document the my setup including the [VM config](VM_Config), general homeAssistant configs, specific scripts, automations, and dashboards I have created, and hardware sensors I have built for my network.

## Software: 

### VM
I have my VM configured with 2 pinned cpu cores and 2GBs of RAM. I have enabled passthrough for video, audio, and USB interfaces. This is so I can configure it as a endpoint to play music out of and allows me to connect a custom [Thread](https://threadgroup.org/) router board to enable non-native Matter support for my server.


### Services
- **Spotify** API integration
- Local weather API
- Google Calendar
- Apple Homekit


### States:
**Away mode:** Boolean for when noone is home so that automations will not turn on any unnecessary devices. Currently manual but I plan to integrate it into presence detection and which devices are connected to the wifi.

**Phase of Day:** [Morning, Afternoon, Evening, Night] Used to change the behavior of the same automation/triggers based on the phase of the day it is currently in. 

**Timelapse Active:** Boolean to start and stop/process a timelapse from exterior security cameras to capture projects and yardwork. 


### [Automations](automations.yaml):
### Water main heater
- After waking up one too many times to a frozen water main in the norther Vermont winters, I decided dripping faucets wasn't enough. I set up this automation using the water main leak sensor I already had installed and a smart plug connected to a space heater. I set up the automation so that when the temperature reported by the leak sensor dropped below 35 degrees, then the space heater turns on until the temp goes above 37 degrees.  
### bedside charging lamp
- All my lights turn on around sunset, and they all turn off at 1 am. When I go to bed before 1 am (which is most of the time), I wanted a way to turn off all the lights without having to click through my app on my phone. So I plugged in my bedside table lamp/wireless charger into an energy monitoring smart plug to act as a control. Since it is a lamp as well as a wireless charger, I could have 2 power states I measured for. I set it up so that when I am getting ready for bed, I can turn on the lamp and the plug will detect current use of < 1 Watt which turns off all lights outside my room and all but one light in my room. Then when I am going to sleep I put my phone on the wireless charger which gives a current use of ~8 Watts which turns off the rest of the lights. 

### Shell Scripts
- [Process timelapse](../ShellScripts/Timelapse_script) on GPU

## Devices
What is a smart home hub without smart home devices. I have a variety of connected devices ranging from smart products already in my home, consumer smart home devices that meet my requirements for local-first operation, and DIY micro controllers I have build, configured, and programmed to provide extra sensor data and controls where I need. 

### Hubs:
- **YoLink** Hub 
- **DIY** Matter router


### Controls:
- **TP-Link** Energy monitoring smart plugs
- **Tuya** Smart lightbulbs
- **Samsung** Smart TV
- **Custom ESP32** Control panel
- **Custom ESP32** Doorbell


### Sensors:
- **YoLink** Water Leak Sensor
- **YoLink** Door Sensor
- **Asus** Wifi Router
- **Reolink** POE Cameras
- **TP-Link** Energy monitoring smart plugs (both control and sensor)
- **ESP32** Temperature/Humidity sensors
- **ESP32** Motion detector
- **ESP32** IP camera
- **ESP32** Air quality sensor


## Selected Projects:
### Timelapse  
After installing a set of Reolink POE cameras around the house I was working on, I got the idea to use them as ready to go timelapse cameras to capture work weekends, maple syrup season, and other outdoor projects and activities we did in the yard.
- **The Goal:** Utilize secure security cameras as remote triggered timelapse cameras to capture ongoing work on the property.
- **The Hardware:** Reolink POE HD security cameras. After a little bit of futzing with the Reolink software, I was able to get the cameras to happily exist on their own silo'd VLAN only able to talk to my server.
- **Home Assistant:**   On Home assistant I wrote some [shell commands](configuration.yaml) to connect to the cameras local API and grab the most recent frame stored there. I had to set the camera to high quality mode first to ensure it was usable. 
- I was then able to call the script from HA automations so that I could start and stop a continuous timelapse with a button. I also added automations to take a picture 3 times a day at sunrise, noon, and sunset for a long term timelapse project tracking the seasons. 
- **Unraid Server:** The last piece of this was to write a script that would be triggered from HA when the timelapse session is ended, but it had to be run on my server to utilize it's (very old) GPU. This [script](../ShellScripts/Timelapse_script) uses ffmpeg to take all the snapshots in the session folder and process them into a video using the appropriate settings for my setup and storing the resulting video in home assistant's media file so it could be played back on dashboards. Finally it clears the folder that stored the snapshots so it is ready for the next one. 
### [Doorbell](../Arduino/Doorbell/doorbell_V1.ino):
- My apartment on the second floor does not have a doorbell. I missed a package 2 days in a row because you cannot hear knocking at the door. For day 3, I did the only logical thing and built a DIY doorbell and installed it outside. It consisted of an ESP32 board with a button and an LED. I set it up so that once the button was pressed, it would publish to an MQTT topic which would trigger a home assistant automation. This automation uses the Spotify integration to begin playing a playlist of doorbell sounds on my PC speakers. I got my package.