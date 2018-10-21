# Setting Up A Raspberry Pi

**TODO: Create cheatsheet of steps at the end**

Setup documentation performed with the following:

**Hardware Model:** Zero W V1.1

![Raspberry Pi Zero W](images/rpi_zero_w.jpg)

**Linux Kernel:** `Linux raspberrypi 4.14.70+ #1144 Tue Sep 18 17:20:50 BST 2018 armv6l GNU/Linux`

**Workstation:** Windows 7 64-bit PC running PuTTY.

## Prepare a microSD card image

[Download a Raspbian image](https://www.raspberrypi.org/downloads/raspbian/)

![Raspbian Lite Download](images/raspbian_lite_dl.png)

This guide was written is using **Raspbian Stretch Lite** which is based on **Debian 9**.

```
Release date: 2018-06-27
Kernel version: 4.14.50+
```

Lite was chosen because we do not interact through a UI or need all of the extra software taking memory and CPU cycles.

Download the `.zip` and unzip the `.img`.

Install [etcher](https://etcher.io/) to simply flash the `.img` to the the card. If it is the only card and flash drive inserted, etcher will automatically choose it.

![Flash using etcher](images/etcher_flash.png)

Mac: try `diskutil list`

Mac and Linux do not need extra software, use: `sudo dd if=~/2018-06-27-raspbian-stretch-lite.img of=/dev/disk1`


## Headless Shortcuts

Again, as this is the Lite version of Raspbian, it would be more convenient to have ssh access from the beginning. This keeps you from having to initially buy a bunch of dongles or be hunched over a crummy spare keyboard and monitor typing stuff in with no ability to paste from this procedure. This procedure will also instruct how to use the mouse less in Windows.

### auto-enable ssh server

Placing an empty file named `ssh` on the `/boot` partition (top level) of the SD card is a Pi boot feature that will let the system know to enable the ssh server and allow connections for the default `pi` user.

#### Linux and Mac

Open terminal and `cd` to the directory where the volume is mounted. (One example would be `/media/sd/boot`).

`touch ssh` to place the file there.

`-------- SCREENSHOT: TERMINAL, MOUNT, CD DIR ----------`

#### Windows

Check the drive letter of the SD card in computer.

![SD Card Drive in Windows 7](images/sd_drive_letter.png)

Open command prompt with `Win+R`, type `cmd`, press enter.

If the SD card drive letter is `D` then type `d:` to jump there.

Input `touch>ssh`. There may be an error message but it works. Type `dir` to check.

![touch ssh file in Windows](images/ssh_file.png)

### Wifi Pre-configure

Wifi may be setup with the `wpa_supplicant.conf` text file file in the same directory the `ssh` file was placed.

**As new as it is, RPi Zero W does not support 5GHz wifi--use an SSID on 2.4GHz.**

#### Contents of the config file

Change the `ssid` and `psk` (passkey) to your own access point or router. Country is the two-letter code. There are no typos, this is the actual file contents verified to work:

```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=US

network={
    ssid="mySSID"
    psk="myWifiPassword"
    key_mgmt=WPA-PSK
}
```

![Files on card](images/verify_files.png)

A USB [NIC](https://en.wikipedia.org/wiki/Network_interface_controller) that's confirmed compatible with Linux should auto-setup with [DHCP](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol) and be on the network. I did not have to do anything extra for this to succeed.

`-------- PHOTO USB NIC ----------`

[ARP](https://en.wikipedia.org/wiki/Address_Resolution_Protocol) your network beforehand, so you can compare the difference and see what IP gets added after the Pi has booted up. Or you can log into your router and look at the DHCP client leases table.

Windows, Linux, and Mac should all be able to run `arp -a` in the terminal or cmd. Do that now and keep the window open or copy/screenshot it.

`-------- SCREENSHOT: mac/linux arp -a result ----------`

## Attach Peripherals

Skip this section if you're following along with headless ssh.

Power is connected last.

### Adding USB and Network

#### OTG Cable

To break out the USB ports into something usable on the Pi Zero, use something like an [OTG cable](https://en.wikipedia.org/wiki/USB_On-The-Go#OTG_micro_cables), which is just the standard for *Micro USB Male-B* to *USB Female-A*. Then to that a  USB 2.0 hub is connected. An externally powered hub is even better--or may even be required if your devices are high-powered (flash drives, tv tuners, wifi). A USB 3.0 hub will not add any performance benefit. The Pi USB Host Controller is 2.0.

`-------- PHOTO OTG CABLE ----------`

`-------- PHOTO USB HUB ----------`

#### Or jumper a USB PCI bracket

*If you lack an OTG cable*, there is another method for hacking one together. If you have spare parts from building computers, you can take a dual USB PCI bracket and jump the pins together using some solid wire or paper clips. Take the [0.1" female USB header](http://pinoutguide.com/Motherboard/usb_2_1_header_pinout.shtml) and connect each pair of the five rows pins together. This links both of the *Female-A* ports to each other. One port is for a micro B cable to let you connect to the Pi Zero, and the other for the male-A USB hub. I have done this myself and it works fine.

#### Or solder a splice

Sacrifice a spare micro USB cable to connect into the Zero and splice its other end to your USB hub. This needs to be done somewhat neatly or it may not work. You are breaking the shielding a normal cable would have.

### Video Output

You will need a *male mini-HDMI* (not micro) to *HDMI (male or female)* adapter. A female HDMI adapter allows you to then use a standard HDMI cable of a preferred length. Adding more interconnects in between the HDMI signal has a chance of not working. Do not daisy chain too many.

`-------- PHOTO HDMI ADAPTER ----------`

If you do not have this adapter, then there is also composite analog TV out on the Pi Zero. A two pin header can be soldered there to wire up a yellow RCA jack. I might cover that later. Video output is not necessary with the headless **ssh** setup mentioned earlier.

`-------- PHOTO OF TV PIN ----------`

`-------- Write-up on /boot dir TV OUT ENABLE ----------`

The following can be added to the `boot.txt` file to enable monochrome composite NTSC video out.

[Boot Config Source](https://www.raspberrypi.org/documentation/configuration/config-txt/video.md)

```
# Enable NTSC
sdtv_mode=0
sdtv_aspect=1
sdtv_disable_colourburst=1
```


### Connect Power

An 800 - 2000 mA, 5.0 - 5.2V charger is the ideal choice for powering the setup. A computer USB port is limited to 500mA--it *works* for testing, but results may vary. As the Raspberry Pi has no power switch, power is connected last.

#### Peripheral Checklist

You only need an SD card if doing this headless over pre-configured wireless!

- microSD card flashed and inserted
- USB adapted
- USB keyboard connected
- USB NIC connected and ethernet connected to a network switch

Now connect the power.

## Login to the Pi

Upon first boot up it will resize the file system to fit the SD card then reboot for changes to take effect.

Wait a couple of minutes after first power on. Run `arp -a` again on your workstation computer to see what new IP has shown up. That will probably be the Pi just joining your network with a DHCP lease.

![ARP result before and after](images/arp.png)

Here I left the window open to run arp again after the Pi booted.

Or check your router web UI. Because I did both ethernet and Wifi to write this, my router shows `raspberrypi` host IPs for both LAN and Wifi in the client table.

There are tools like `nmap` and `Angry IP Scanner` to scan your subnet. But at this point, it is probably the Pi that's unable to connect.

### Still need the IP?

With all of the peripherals connected, physically at the Pi login prompt you will have:

**Login as:** `pi`

**Password:** `raspberry`

Check and see if you have an IP address

`ifconfig`

`-------- SCREENSHOT: RPI IFCONFIG ----------`

If you do not see a `192.168.0.x` something then worry about troubleshooting this a little later and skip the following connection steps.

### SSH Connection

Windows will need a terminal program which supports ssh, such as [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).

Mac & Linux will have ssh already available from the terminal.

#### Linux and Mac Terminal

`ssh pi@192.168.0.6` or whatever the IP turned out to be.

`-------- SCREENSHOT: OPEN TERMINAL ON MAC OR UBUNTU ----------`

**Password:** `raspberry`

#### Windows with PuTTY

You can avoid the mouse even though it's a GUI program! Look at the underlined letters.

Press `Alt+S` to radio toggle SSH

Press `Alt+N` to activate the host name field

Type in the IP here

Port 22 is left alone for a fresh default Pi

Press `Alt+O` to *open* the connection

`-------- SCREENSHOT: PUTTY CONNECTION ----------`

**Login as:** `pi`

**Password:** `raspberry`


## Customize the Environment

The guides found on google searches will say to traditionally use `sudo raspi-config` and then proceed to explain how to use each menu option. I prefer using a sequence that can be copy and pasted in one move without interactivity. If we know what we want out of the computer, then let's tell it directly to go there. This will be demonstration of each part.

### Basic Commandline Familiarity

Test sudo: `sudo echo hello` and get the password prompt. The password is `raspberry` (default from the image). Future `sudo` commands will not prompt for now until a time elapses. **Sudo** is *super user do*, or run things as a user with unlimited power (`root` user). This is for security and safety reasons. It is very easy to "sudo-ruin-everything" on even the fanciest Linux computer. `dd` and `rm` are some of those commands. You can always reflash your SD card and start over.

If you type something wrong you can press `Ctrl+C` instead of cruise controlling the backspace key. Or press `home` and prepend the line with `#` to comment it out and have the text still on screen to study.

The `Up Arrow key` will scroll through previous commands and the `history` command shows them listed.

`Shift+PageUp` and `Shift+PageDown` will let you scroll back through the past output. You can start typing to jump back or press `Ctrl+C`.

Exiting the session can be done with `Ctrl+D` (when on the starting tty) or inputting `exit`.

Pasting text into the terminal from your workstation's clipboard depends on the software being used.

- Mac is usually `Cmd+V` to paste, `Cmd+C` to copy.
- Linux might be `Ctrl+Shift+V` to paste, `Ctrl+Shift+C` to copy.
- Windows using PuTTY is a mouse `right-click` to paste, and selecting with the mouse automatically copies.

New lines get interpreted as a command input, so when pasting scripts from this, the last line will not be sent through until you press enter for it. Spaces are separators 

### Locale Change

The Pi starts default with the`en_GB.UTF-8` locale and `UK` keyboard. Here is how to change it for the US `en_US.UTF-8`.

Paste this script and press enter.

```
sudo sed -i '/^#/! s/^/# /g' /etc/locale.gen && \
sudo sed -i 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/g' /etc/locale.gen
export LANGUAGE=en_US.UTF-8
export LANG=en_US.UTF-8
export LC_MESSAGES="en_US.UTF-8"
export LC_ALL=en_US.UTF-8
sudo /usr/sbin/locale-gen en_US.UTF-8
sudo update-locale en_US.UTF-8
```

### Keyboard Layout Change

US 104 key. If you're not logged in through ssh, some of the symbols you input will be wrong. ssh can send the keyboard layout for its connected terminal so you will not see this.

Paste this script and press enter

```
KYBD="/etc/default/keyboard"
sudo sed -i 's/XKBMODEL=.*/XKBMODEL="pc104"/g' "$KYBD" && \
sudo sed -i 's/XKBLAYOUT=.*/XKBLAYOUT="us"/g' "$KYBD" && \
sudo sed -i 's/BACKSPACE=.*/BACKSPACE="guess"/g' "$KYBD" && \
sudo sed -i 's/XKBOPTIONS=.*/XKBOPTIONS=""/g' "$KYBD"
```

### Hostname Change

Having a unique hostname will make owning multiple networked Pi's less confusing.

Here is a script to randomize it to `zero-1234` something. The change is seen after relogging in.

```
NEWNAME=zero-$RANDOM && \
sudo sed -i "s/$HOSTNAME/$NEWNAME/" /etc/hostname
sudo sed -i "s/$HOSTNAME/$NEWNAME/" /etc/hosts
sudo hostname "$NEWNAME" && \
```

Confirm the hostname changed with `hostnamectl`.

### Add Your User

Removing the `pi` user will add some more security and remove the ssh warnings. First a replacement user must be created.

Add your new user with `sudo` permissions. I will call my user on this system `mse`.

If you hit enter through every prompt it keeps the fields empty. You will want a password to have ssh access with it.

```
NEWUSER="mse"
sudo adduser "$NEWUSER"
sudo usermod -aG sudo "$NEWUSER"
```

The user `mse` we made can change or set a password with `sudo passwd mse`.

**Login again:** Terminate the session with `exit`. Log back in as the new user to let you **delete** the `pi` user. This will also updates the hostname from earlier.


### Remove the pi User

If you are not using wifi and do not intend for the Pi to go back on the network after the setup is complete, you may want to keep the `pi` user and the default password so you do not have to track another set of credentials.

```
sudo userdel pi
```

Bots and crackers running scripts that search around for default Raspberry Pi's will not simply gain access if there are security holes in your network. There are other easy changes like this to defend against these malicious things. An example would be to change the ssh port from 22 to something above 1024.

### Remove MotD

I like to clean up the message of the day for login.

```
sudo rm /etc/motd
touch ~/.hushlogin
```

## Run Updates

The Pi will need to be on the network to reach out for updates.

`sudo apt update -qq && sudo apt-get upgrade -y`

Should that fail, and you're networked, it may be that `/etc/apt/sources` stupidly has the CDROM as the only repo source. I do not think this will happen with the Raspbian images. Remove the `-qq` switches (quiet) for more output to help troubleshoot.

# Install PWM Control

## GPIO Setup

```
sudo gpio_alt -p 13 -f 0
sudo gpio_alt -p 18 -f 5

Set pin 13 to alternative-function 0
Set pin 18 to alternative-function 5
```

# PowerBlock

ATTiny85 microcontroller as a power controller for the RPi.

https://github.com/petrockblog/PowerBlock

https://github.com/vpanarello/RpiPowerController

Set the board as "ATtiny25/45/85", processor as ATtiny85
Clock as 1MHz and programmer as "Arduino as ISP".

## Install

`wget -O - https://raw.githubusercontent.com/petrockblog/PowerBlock/master/install.sh | sudo bash`

Additional packages installed totals **151 MB**

```
  build-essential
  cmake
  cmake-data
  doxygen
  g++-4.9
  git
  git-man
  libarchive13
  libclang1-3.9
  liberror-perl
  libjsoncpp1
  libllvm3.9
  liblzo2-2
  libstdc++-4.9-dev
  libuv1
```

**statuspin:** Raspberry BCM pin used for status signaling (default: 17) connects to S2 on PowerBlock
**shutdownpin:** Raspberry BCM pin used for shutdown signaling (default: 18) connects to S1 on Powerblock

PowerBlock driver logging `/var/log/powerblock.log`
When the driver observes a shutdown signal from the PowerBlock, a shutdown Bash script is called.
You can find and edit it at `/etc/powerblockswitchoff.sh`

### Need to change the pinout to not stomp on the HW PWM
sudo sed -i 's/"shutdownpin": 18/"shutdownpin": 27/g' /etc/powerblockconfig.cfg

### Need to change the pinout to not stomp on the HW PWM
sudo sed -i 's/"shutdownpin": 18/"shutdownpin": 27/g' /etc/powerblockconfig.cfg

### dmesg

```
2018-10-12 20:38:30.711 INFO  [282] [main@70] Starting PowerBlock driver, version 1.5.0
2018-10-12 20:38:30.731 INFO  [282] [PowerBlockConfiguration::initialize@57] Shutdown is ACTIVATED
2018-10-12 20:38:30.732 INFO  [282] [PowerBlockConfiguration::initialize@77] Shutdown Pin is 27
2018-10-12 20:38:30.732 INFO  [282] [PowerBlockConfiguration::initialize@78] Status Pin is 17
2018-10-12 20:38:30.745 INFO  [282] [PowerSwitch::setPowerSignal@75] Setting RPi status signal to HIGH
```

## ATTiny85 Code

`https://github.com/vpanarello/RpiPowerController/blob/master/sketch_nov16a.ino`


# Misc

## HDMI Audio Output

`amixer cset numid=3 2`

0 = auto
1 = analog
2 = hdmi


Optional additional option (incase it is not working)
`/boot/config.txt`
`hdmi_drive=2`



```
hardware interfaces
#dtparam=i2c_arm=on
#dtparam=i2s=on
#dtparam=spi=on
```

https://www.raspberrypi.org/documentation/configuration/device-tree.md

Device Trees are usually written in a textual form known as Device Tree Source (DTS) and stored in files with a .dts suffix. DTS syntax is C-like, with braces for grouping and semicolons at the end of each line. Note that DTS requires semicolons after closing braces: think of C structs rather than functions.



# Appendices

### SD Card Space

A 4GB Class 10 or UHS-1 card is suitable.

After setup run 
`df -m /`

results in `1048 MB` or `1.1 GB` of used space for this setup.

vim adds another `30.2 MB`

We could install tlp, but there are a ton of dependencies (56.4 MB).
      
`sudo apt-get install tlp`

Nope, that broke my wifi management. Had to start the system over. But it let me practice a cheat sheet and find some bugs in the procedure.

### Battery

Model: LIS1442
Type: Lion
Size: 16x44 mm
Voltage: 3.7V
Capacity: 490mA
Space Inside: 22x57 mm with modding


### Memory Card Adapter

[Kingston FCR-HS4 Memory card reader](https://www.amazon.com/dp/B00VAGX6MW)

I use the Kingston FCR-HS3, which I bought in 2015 for $17. There is now a FCR-HS4 model for $17. This is probably the best memory card reader you can get. The cards I use for my camera will read and write in this 100 MB/s no problem. It is truly a USB 3.0 device. It works in 2.0 as well. I never have quirky issues or compatibility problems with it. Also it has its own micro SD slot, so you do not need SD adapters. All four slots show up as seperate drives, so you can have two micro SD cards inserted at once if you do use an adapter.

Here is an image backing up the microSD card the RPI was using. These are 16GB UHS-1 and were only $8. Worth the extra few dollars to have it perform 10x faster, even if the Pi itself will not see it.

![SD card speed](images/sd_reader_speed.png)

### Pi Zero W Kit Pricing

[$14 Pi Zero WH with header soldered](https://www.adafruit.com/product/3708)
[$34.50 for the starter pack on adafruit](https://www.adafruit.com/product/3410)
[$40 for the ad blocking pi zero W no soldering!](https://www.adafruit.com/product/3973)

What do you really need?

- $10 Pi Zero W
- OTG Cable
- $8 8 or 16GB UHS-1 or Class 10 uSD (usually they come with full size adapters)
- Micro USB cell charger (check a thrift store if you do not have one laying around)
- Double row of 0.1" header pins maybe
- SD card reader if you don't have any computers with one (it's 2018, how could you!)

You can 3d print a case or get creative.


### More Software

`tmux` is only **561 kB**.

running the powercontrol script installs git and stuff.