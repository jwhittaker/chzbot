# chzbot
Cheesebot -- Linux Raspberry Pi Robotics

## Initial Project Goal
Control a scale car's steering servomotor and electronic speed controller with 50 Hz PWM signals using a Sony Bluetooth joystick.

## You're on Develop Branch!

Here is where the code, libraries, write-ups live! Try at your own risk, as things are not yet done!

`git clone -b develop git@github.com:jwhittaker/chzbot.git`

## Demo Goals

- [Write-up for starting with a new Raspberry Pi](https://github.com/jwhittaker/chzbot/blob/develop/notes/rpi_setup.md)
- [Write-up for setting up sound playback with an external audio amp][https://github.com/jwhittaker/chzbot/blob/develop/notes/audio.md]
- [Write-up for installing the pigpio library for GPIO control using C](https://github.com/jwhittaker/chzbot/blob/develop/notes/rpi_pwm.md)
- [Write-up for installing the Sony PS3 Navigation remote controller with Bluetooth using the sixad library](https://github.com/jwhittaker/chzbot/blob/develop/notes/ps3_btl.md)
- Write-up for testing the Linux joystick API
- Demonstration of the joystick triggering sound sample playback
- Demonstration of the joystick controlling GPIO PWM output
- Demonstration of scriptable macros for multi-step action sequences
- Demonstration of programmatic control limitations for easier driving
- Measuring and documenting power draw of components

### Partial Demos

Proof of concept & my learning of the libraries.

- [GPIO PWM Demo](https://github.com/jwhittaker/chzbot/tree/develop/src/gpio/servo_test)
- [Joystick Demo](https://github.com/jwhittaker/chzbot/tree/develop/src/joystick/jstest_mod)

### Cheatsheets

Sparse documentation with mostly commands: [setup cheatsheet](https://github.com/jwhittaker/chzbot/blob/develop/notes/pi_cheatsheet.md)

