
# Options

You now have two options, these being 
1) Compile and install `sixad` to handle the connection between the PS3 controller
2) Use `bluetoothctl` to make a connection to the PS3 Controller

## bluetoothctl

This is tricky for me. My apt unit has about 50 bluetooth  devices in range and I cannot determine which one is my Sony controller. So I have not made this work yet.

`sudo bluetoothctl`

```
agent on
default-agent
scan on
```

Press the ps3 button to pair and get the MAC

```
[NEW] Device B8:27:EB:A4:59:08 B8-27-EB-A4-59-08
[CHG] Device B8:27:EB:A4:59:08 Connected: no
[DEL] Device B8:27:EB:A4:59:08 B8-27-EB-A4-59-08
```

`connect YOUR_MAC_ADDRESS`

```
Attempting to connect to B8:27:EB:A4:59:08
[CHG] Device B8:27:EB:A4:59:08 Modalias: usb:v054Cp0268d0100
[CHG] Device B8:27:EB:A4:59:08 UUIDs:
        00001124-0000-1000-8000-00805f9b34fb
        00001200-0000-1000-8000-00805f9b34fb
```

`trust YOUR_MAC_ADDRESS`

```
[CHG] Device B8:27:EB:A4:59:08 Trusted: yes
Changing B8:27:EB:A4:59:08 trust succeeded
```