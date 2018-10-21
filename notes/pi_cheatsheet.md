# Cheatsheet

### Flash sd card with Raspbian

### touch ssh file on card

### Add wifi config file

`wpa_supplicant.conf`

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

### Change locale to US

I swapped the order of some commands. Need to review.

```
sudo sed -i '/^#/! s/^/# /g' /etc/locale.gen && \
sudo sed -i 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/g' /etc/locale.gen
sudo /usr/sbin/locale-gen en_US.UTF-8
sudo update-locale en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
```


### Change keyboard to US 104

```
KYBD="/etc/default/keyboard"
sudo sed -i 's/XKBMODEL=.*/XKBMODEL="pc104"/g' "$KYBD" && \
sudo sed -i 's/XKBLAYOUT=.*/XKBLAYOUT="us"/g' "$KYBD" && \
sudo sed -i 's/BACKSPACE=.*/BACKSPACE="guess"/g' "$KYBD" && \
sudo sed -i 's/XKBOPTIONS=.*/XKBOPTIONS=""/g' "$KYBD"
```

### Change hostname

```
NEWNAME=zero-$RANDOM && \
sudo sed -i "s/$HOSTNAME/$NEWNAME/" /etc/hostname
sudo sed -i "s/$HOSTNAME/$NEWNAME/" /etc/hosts
sudo hostname "$NEWNAME" && \
```

### Add your user

```
NEWUSER="mse"
sudo adduser "$NEWUSER" && \
sudo usermod -aG sudo "$NEWUSER"
```

### Update

```
sudo apt update -qq && sudo apt-get upgrade -y
```

### Install sixad

Dependencies

```
sudo apt-get -qq -y install \
  joystick \
  libbluetooth-dev \
  libbluetooth3 \
  bluez \
  bluez-hcidump \
  libusb-dev \
  pkg-config \
  build-essential \
  check-install \
  dialog \
  python-dbus
```

Source and compile

```
wget -O "${HOME}/sixad.zip" "https://github.com/RetroPie/sixad/archive/master.zip" && \
unzip "${HOME}/sixad.zip" -d "${HOME}" && \
rm -f "${HOME}/sixad.zip"
cd "${HOME}/sixad-master" && \
make && sudo make install
```


### OR Install sixad binaries

wget from github or scp off my server.

```
cd ~ && \
scp joseph@192.168.1.135:/mnt/data/projects/costumes/mouse_droid/electronics/rpi_0_w/sixad_bin.tar && \
tar -xf sixad_bin.tar && rm -f sixad_bin.tar && \
cd sixad_bin && sudo make install
```

### Get PS3 Controller

Plug in

`sudo sixpair`

`sudo sixad --boot-yes`

Reboot

`sudo reboot now`

Tests

`sudo jstest /dev/input/js0`

`sudo hcidump -x`