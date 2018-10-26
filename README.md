# chzbot
Cheesebot -- Linux Raspberry Pi Robotics

## Initial Project Goal

Control a scale car's steering servomotor and electronic speed controller with 50 Hz PWM signals using a Sony Bluetooth joystick.

## Development In Progress

**Switch to the develop branch!**

I am currently working on the `develop` branch until solid demos are ready. They will be merged into `master` branch when near completion.

### Demo Goals

- Write-up for starting with a new Raspberry Pi
- Write-up for setting up sound playback with an external audio amp
- Write-up for installing the `pigpio` library for GPIO control using C
- Write-up for installing the Sony PS3 Navigation remote controller with Bluetooth using the `sixad` library
- Write-up for testing the Linux `joystick` API
- Demonstration of the joystick triggering sound sample playback
- Demonstration of the joystick controlling GPIO PWM output
- Demonstration of scriptable macros for multi-step action sequences
- Demonstration of programmatic control limitations for easier driving
- Measuring and documenting power draw of components

The progress of these things can be found sitting in the develop branch.

`git clone -b develop git@github.com:jwhittaker/chzbot.git`
