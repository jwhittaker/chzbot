# PS3 Bluetooth Game Controller Installation

After the Raspberry Pi Zero W is setup, proceed to install bluetooth game controller support.

https://github.com/RetroPie/sixad/blob/master/INSTALL

https://pimylifeup.com/raspberry-pi-playstation-controllers/

https://retropie.org.uk/forum/topic/12945/wireless-battery-indicator-ps3

http://qtsixa.sourceforge.net/manual.pdf


With the PS3 navigation **game controller** connected, this sets its **bluetooth system controller MAC** to your Pi's address. The word "controller" is starting to sound ambiguous. There is game controller you hold in your hands to send button presses, and there are controller systems the computer interacts with. In computer terminology, it refers to a subsystem (maybe its own chip) that is dedicated to managing some type of data flow.


## Initialize the PS3 Controller

Connect the PS3 controller to the Pi USB hub with a USB male-A mini cable. 

### Verify USB Connection

The effect can be observed in `dmesg` output. If it was one of the most recent things to occur, it will be at the tail end of the output. `sudo dmesg | tail`.

**Example dmesg text of a PS3 controller USB connected:**
```
[ 9516.455395] usb 1-1.2: new full-speed USB device number 6 using dwc_otg
[ 9516.707182] usb 1-1.2: New USB device found, idVendor=054c, idProduct=042f
[ 9516.707208] usb 1-1.2: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[ 9516.707219] usb 1-1.2: Product: Navigation Controller
[ 9516.707229] usb 1-1.2: Manufacturer: Sony
```

### Extra USB Device Information

The `lsusb` program can also show a lot of verbose information.

Here is a script alias ask for extra verbose USB information on any device that is labeled for playstations.

```
alias sony="for device in $(lsusb | grep -i playstation | \
  cut -d\  -f2,4 | tr -d ':' | tr -s ' ' ':'); \
  do sudo lsusb -vv -s $device; done"
```

Now just type `sony` to invoke our new alias command and it will display all of its USB information.

## sixad

### Install Library Dependencies

```
sudo apt update && \
sudo apt-get -qq -y install \
  joystick \
  libbluetooth-dev \
  libbluetooth3 \
  bluez \
  bluez-hcidump \
  libusb-dev \
  pkg-config \
  build-essential \
  check install \
  dialog \
  python-dbus
```

### Download Source Code

```
wget -O "${HOME}/sixad.zip" "https://github.com/RetroPie/sixad/archive/master.zip" && \
unzip "${HOME}/sixad.zip" -d "${HOME}" && \
rf -f "${HOME}/sixad.zip"
```

### Compile and Install

```
cd "${HOME}/sixad-master" && \
make && sudo make install
```

While writing this procedure it worked better to reboot after the install. But let us add groups to the user first, then reboot.

### Add User to input & bluetooth Groups

sixpair, sixad, jstest, and other bluetooth/input (joystick) using programs will no longer have to be ran with sudo invoked.

`sudo usermod -aG input mse`
`sudo usermod -aG bluetooth mse`

You will have to exit and log back in for it to take effect.

`sudo reboot now`


### Pair Controller

Connect the PS3 controller to the Pi USB hub or OTG cable with a USB male-A mini cable.

Get the BT addr

`sixpair`

Start the `sixad` daemon. `sixad --start`. Leave this alone and open a second terminal window to your Pi.

*sixad will completely take over the Bluetooth adapter (only PS3 controllers will work)*

If you get the error message `Can't open HIDP control socket` try rebooting if you had not already.

You should see the daemon's text in your first terminal:

```
sixad-bin[804]: started
sixad-bin[804]: sixad started, press the PS button now
Watching... (5s)
```
Unplug the navigation controller from the Pi's USB.

Press the Playstation button on the navigation controller to connect via bluetooth.

```
sixad-sixaxis[1079]: started
sixad-sixaxis[1079]: Connected 'PLAYSTATION(R)3 Controller (00:06:F7:5B:84:B5)' [Battery 05]
```

The light on the navigation controller will stay solid red.

### Verify Pairing

In your second terminal window the connection can be verified a few ways.

`sudo dmesg | tail`

```
[  156.398850] usb 1-1.2: USB disconnect, device number 4
[  158.000359] input: PLAYSTATION(R)3 Controller (00:06:F7:xx:xx:xx) as /devices/virtual/input/input4
```

Now we have its MAC address (mine xx-redacted). I did an [OUI lookup](https://www.wireshark.org/tools/oui-lookup.html) of the BT MAC address `00:06:F7` and it's not a Sony device but by `Alps Electric Co.,Ltd.`

We should also see a **joystick 0** `js0` in input devices with `ls -l /dev/input`

```
drwxr-xr-x  2 root root      80 Oct  6 18:55 by-id
drwxr-xr-x  2 root root      80 Oct  6 18:55 by-path
crw-rw----  1 root input 13, 64 Oct  6 18:53 event0
crw-rw----  1 root input 13, 65 Oct  6 18:53 event1
crw-rw----  1 root input 13, 66 Oct  6 18:55 event2
crw-rw----  1 root input 13,  0 Oct  6 18:55 js0
crw-rw----  1 root input 13, 63 Oct  6 18:52 mice
```

Test that the controller is working and paired properly:

```
jstest /dev/input/js0
```

Move the analog stick and press buttons!

`Ctrl+C` to quit

### Mapping

On my navigation controller I found 10 buttons and 2 axes mapped this way from `jstest`:

- **axis 0** = X-axis negative left, positive right (16-bit signed -32767 to 32767)
- **axis 1** = Y-axis negative up, positive down (16-bit signed -32767 to 32767)
- **button 1** = L3 (analog stick clicks down)
- **button 4** = D-Pad up
- **button 5** = D-Pad right
- **button 6** = D-Pad down
- **button 7** = D-Pad left
- **button 8** = L2
- **button 10** = L1
- **button 13** = O
- **button 14** = X
- **button 16** = PS

With `sudo hcidump -x` I also observed that L1 and L2 buttons support 8-bit pressure sensitivity (0-255).

### Other sixad Config

`cat /var/lib/sixad/profiles/default`

```
enable_leds 1
enable_joystick 1
enable_input 0
enable_remote 0
enable_rumble 1
enable_timeout 1
timeout_mins 10
led_n_auto 1
led_n_number 0
led_anim 1
enable_buttons 1
enable_sbuttons 0
enable_axis 1
enable_accel 0
enable_accon 0
enable_speed 0
enable_pos 0
enable_safety_timeout 0
```

### Re-Pair Behavior

After a timeout period the remote will sleep to save power and disconnect. sixad will show a message:

```
sixad-sixaxis[1079]: Inactive timeout reached, disconnecting...
sixad-sixaxis[1079]: Disconnected "00:06:F7:xx:xx:xx"
```

Press the PS button again on the navigation controller and it will re-pair.

```
sixad-sixaxis[11172]: started
sixad-sixaxis[11172]: Connected 'PLAYSTATION(R)3 Controller (00:06:F7:xx:xx:xx)' [Battery 05]
```

### Auto-start and Persist After Reboot

Simply run sixad with the `--boot-yes` flag. `sixad --help` shows this is an option.

```
sudo sixad --boot-yes
```

Now after reboot all you have to do is press the PS button to re-pair.

After rebooting my Pi and pairing, I can confirm that sixad autostarted itself with `ps -ef | grep six`. Also this is evidenced by seeing the `js0` input device return. `ls -l /dev/input`

```
root       361     1  0 20:13 ?        00:00:00 /bin/bash /usr/bin/sixad start
root       539   361  0 20:13 ?        00:00:00 /usr/sbin/sixad-bin 0 0 0
root       694   539  0 20:14 ?        00:00:00 /usr/sbin/sixad-sixaxis 00:06:F7:xx:xx:xx 0
```

After we compiled awhile back it was the install script ran with `sudo make install` that placed the programs in the `/usr/sbin` dir. If you type `env` you will see that `/usr/sbin` is part of the `PATH` variable--which is why the program can be ran from anywhere without prepending the directory or with `./` when in the current directory.


### Other

Give control to the BT controller again
`sudo sixad -r`

Press ps3 button on controller to pair

Have it start on bootup. This part may not be needed:

`sudo update-rc.d sixad defaults`

For troubleshooting purposes, these commands will disable sixad.

`sudo update-rc.d sixad disable`
`sudo service sixad stop`


`hidraw` to supplement BT joystick
http://www.pabr.org/sixlinux/sixlinux.en.html#inertial



