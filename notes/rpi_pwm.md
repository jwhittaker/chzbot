# PWM Notes

A lot of information here is pasted or paraphrased from [pigpio](http://abyz.me.uk/rpi/pigpio/)

## Hardware vs Software PWM

The Broadcom SoC BCM2835 supports two hardware PWM channels using pigpio. GPIO/Pin selection will depend on which other hardware features you do not want to overlap (SPI, i2c, etc). By using h/w PWM, you give up the Pi's built-in analog audio ouput. But that's ok! It is terrible quality, and a $2 USB sound card will actually sound better.

## RPi Model PWM GPIOs Table

| GPIO | PWM CH |      Models     |
|-----:|-------:|:---------------:|
|   12 |      0 | All but A and B |
|   13 |      1 | All but A and B |
|   18 |      0 | All             |
|   19 |      1 | All but A and B |
|   40 |      0 | Compute module  |
|   41 |      1 | Compute module  |
|   45 |      1 | Compute module  |
|   52 |      0 | Compute module  |
|   53 |      1 | Compute module  |

## Python

Demo for accessing the `pigpio` daemon with python.

```
sudo pigpiod
python
import pigpio
pi=pigpio.pi()

# 800Hz 25% dutycycle
pi.hardware_PWM(12, 800, 1e6*0.25)
```

Frequencies above 30 MHz are unlikely to work.

## PWM Driver I2C Interfaces

These I/O expanders take commands over the I2C bus to control actual PWM hardware. This solution is much more precise and reliable than software loop timers but it costs more to add hardware to the project. A good middle ground is the Arduino Pro Mini. It is small, inexpensive, programmable, and has 6 analog (PWM) inputs (10-bit ADC) and 6 analog outputs (10-bit DAC).

- [Adafruit PWM Driver PCA96855](http://www.adafruit.com/products/815)
- [SparkFun PWM Shield TLC5940](https://www.sparkfun.com/products/10615)
- [Arduino Pro Mini ATMega328](https://store.arduino.cc/usa/arduino-pro-mini)

## RPi PWM Interfacing with pigpio

The BCM2835 contains 2 independent PWM channels (0 and 1), each of which be connnected to a limited subset of GPIO pins. The following GPIO pins may be connected to the following PWM channels:

Both PWM channels are driven by the same PWM clock, whose clock divider can be varied using `bcm2835_pwm_set_clock()`

The PWM clock can be set to control the PWM pulse widths. The PWM clock is derived from a `19.2 MHz` clock. You can set any divider, but some common ones are provided by the `bcm2835PWMClockDivider`

Each PWM channel can run in either **Balanced** or **Mark-Space** mode.

```
typedef enum {
    BCM2835_PWM_CLOCK_DIVIDER_2048  = 2048,    /*!< 2048 = 9.375kHz */
    BCM2835_PWM_CLOCK_DIVIDER_1024  = 1024,    /*!< 1024 = 18.75kHz */
    BCM2835_PWM_CLOCK_DIVIDER_512   = 512,     /*!< 512 = 37.5kHz */
    BCM2835_PWM_CLOCK_DIVIDER_256   = 256,     /*!< 256 = 75kHz */
    BCM2835_PWM_CLOCK_DIVIDER_128   = 128,     /*!< 128 = 150kHz */
    BCM2835_PWM_CLOCK_DIVIDER_64    = 64,      /*!< 64 = 300kHz */
    BCM2835_PWM_CLOCK_DIVIDER_32    = 32,      /*!< 32 = 600.0kHz */
    BCM2835_PWM_CLOCK_DIVIDER_16    = 16,      /*!< 16 = 1.2MHz */
    BCM2835_PWM_CLOCK_DIVIDER_8     = 8,       /*!< 8 = 2.4MHz */
    BCM2835_PWM_CLOCK_DIVIDER_4     = 4,       /*!< 4 = 4.8MHz */
    BCM2835_PWM_CLOCK_DIVIDER_2     = 2,       /*!< 2 = 9.6MHz, fastest you can get */
    BCM2835_PWM_CLOCK_DIVIDER_1     = 1        /*!< 1 = 4.6875kHz, same as divider 4096 */
} bcm2835PWMClockDivider;
```

You'll probably need to set the PWM mode to **Mark-Space mode** for most use cases and sanity reasons as described above.
In Mark-Space mode, the hardware sets the output `HIGH` for DATA clock pulses wide, followed by LOW for RANGE-DATA clock pulses.

In this mode, the duration that each 'pulse' is HIGH vs LOW is determined by the ratio of the PWM data to the PWM range - this is irrespective of the PWM clock speed.

The **PWM range is effectively the resolution** or number of possible 'divisions' of each pulse. The more divisions the higher the resolution and thus more states encodable for a given pulse width. *(See **Servo PWM Chart** section at the bottom)*

The 'duty cycle' is the ratio of PWM data to PWM range expressed as a percentage. A PWM range of 10 with PWM data of 8 is an 80% duty cycle.

To calculate your required frequency, pick a clock divisor that make sense for your application - I chose 16, which equates to 1.2 MHz. 

(1.2 MHz / 440 Hz)

`const A4_RANGE = 1.2e6 / 440;`

(2727.27 / 2 ) = 1363.636

`A4Data = A4_RANGE / 2;`

(1.2 MHz / 370 Hz)

`const F4S_RANGE = 1.2e6 / 370;`

`F4SData = F4S_RANGE / 2;`

(1.2 MHz / 277 Hz)

`const C4S_RANGE = 1.2e6 / 277;`

`C4SData = C4S_RANGE / 2;`

If you wanted to drive a servo at say `50Hz` the same range calculation holds true:

PWM Range = `PWM frequency / Desired Output Frequency`

So if range is 20,000 us and data is 1,000 us you get 5% duty. 50Hz is 20,000 ms long pulses

- 1.000 ms reverse (5.00% duty)
- 1.500 ms idle (7.50% duty)
- 2.000 ms forwards (10.0% duty)

### Codecubox Blog

[Hardware PWM With RPi Zero](http://www.codecubix.eu/index.php/2018/05/29/hardware-pwm-with-raspberry-pi-zero/)

Several software interfaces are available to manage directly these hardware components:

– Native C code (direct register access)
– BCM2835 library
– PIGPIO library

[pigio](http://abyz.co.uk/rpi/pigpio/index.html) is simple and nicely documented

The PWM pulses are timed using the DMA controller on the Pi, so the resulting signal is much more accurate than software PWM (stuff you get with `RPi.GPIO`), and more than sufficient for smooth servo control.

[Video: H/W PWM RPio pigpio](https://www.youtube.com/watch?v=aMx6ySzFOzc)



### Servo Examples

http://abyz.me.uk/rpi/pigpio/cif.html#gpioServo

```
gpioServo(17, 1000); // Move servo to safe position anti-clockwise.
gpioServo(23, 1500); // Move servo to centre position.
gpioServo(25, 2000); // Move servo to safe position clockwise.
```

This function updates servos at 50Hz.

### Frequencies Other than 50Hz

If you wish to use a different update frequency you will have to use the PWM functions. 

```
PWM Hz    50   100  200  400  500
1E6/Hz 20000 10000 5000 2500 2000
```
Firstly set the desired PWM frequency using gpioSetPWMfrequency. 
All 6 sample rates will handle 50Hz.
5us sample rate can do up to 8kHz
1us can do up to 40kHz

Then set the **PWM range** using `gpioSetPWMrange` to 1E6/frequency

Selects the dutycycle range to be used for the GPIO. Subsequent calls to gpioPWM will use a dutycycle between 0 (off) and range (fully on).  If PWM is currently active on the GPIO its dutycycle will be scaled to reflect the new range. 

user_gpio: 0-31
    range: 25-40000

The real range, the number of steps between fully off and fully on for each frequency, is given in the following table. 

```
  25,   50,  100,  125,  200,  250,  400,   500,   625,
 800, 1000, 1250, 2000, 2500, 4000, 5000, 10000, 20000
```

The real value set by gpioPWM is `(dutycycle * real range) / range`

`100% * 40kHz / 20000 = 2`

If a hardware clock or hardware PWM is active on the GPIO the reported range will be 1000000 (1M). 

```
gpioSetPWMrange(24, 2000); // Now 2000 is fully on
                           //     1000 is half on
                           //      500 is quarter on, etc.
```

The actual number of steps beween off and fully on is the integral part of `250M/PWMfreq`

The actual frequency set is 250 million / steps.

There will only be a million steps for a PWMfreq of 250. Lower frequencies will have more steps and higher frequencies will have fewer steps. PWMduty is automatically scaled to take this into account.

So if we want a freq of 100kHz (10us) or 1 MHz (1us)
we get 2500 steps or with 1MHz 250 steps.

The PWM and servo pulses are timed using the DMA and PWM peripherals. 
This use was inspired by Richard Hirst's servoblaster kernel module. 
See https://github.com/richardghirst/PiBits/tree/master/ServoBlaster

int gpioSetMode(unsigned gpio, unsigned mode)
gpioSetMode(23, PI_ALT0);  // Set GPIO23 (13)
gpioSetMode(1, PI_ALT5); // Set GPIO1 (18)

gpio: 0-53
mode: 0-7

The operational mode of a GPIO, normally INPUT or OUTPUT. 

PI_INPUT 0
PI_OUTPUT 1
PI_ALT0 4
PI_ALT1 5
PI_ALT2 6
PI_ALT3 7
PI_ALT4 3
PI_ALT5 2

Starts PWM on the GPIO, dutycycle between 0 (off) and range (fully on). Range defaults to 255. 
This and the servo functionality use the DMA and PWM or PCM peripherals to control and schedule the pulse lengths and dutycycles. 
user_gpio: 0-31
dutycycle: 0-range

The gpioSetPWMrange function may be used to change the default range of 255. 

Example

```
gpioPWM(17, 255); // Sets GPIO17 full on.
gpioPWM(18, 128); // Sets GPIO18 half on.
gpioPWM(23, 0);   // Sets GPIO23 full off.
```

If a hardware clock is active on the GPIO the reported dutycycle will be 500000 (500k) out of 1000000 (1M). 
If hardware PWM is active on the GPIO the reported dutycycle will be out of a 1000000 (1M). 

int gpioSetPWMrange(unsigned user_gpio, unsigned range)
Selects the dutycycle range to be used for the GPIO. Subsequent calls to gpioPWM will use a dutycycle between 0 (off) and range (fully on). 

user_gpio: 0-31
    range: 25-40000


Returns the real range for the given GPIO's frequency if OK, otherwise PI_BAD_USER_GPIO or PI_BAD_DUTYRANGE. 

If PWM is currently active on the GPIO its dutycycle will be scaled to reflect the new range. 

The real range, the number of steps between fully off and fully on for each frequency, is given in the following table. 

```
                       1MHz  ~~~.
                                V
  25,   50,  100,  125,  200,  250,  400,   500,   625,
 800, 1000, 1250, 2000, 2500, 4000, 5000, 10000, 20000
                         /\
                          \___ 100kHz
```

The real value set by gpioPWM is `(dutycycle * real range) / range` 

### Example

`gpioSetPWMrange`

```
gpioSetPWMrange(24, 2000); // Now 2000 is fully on
                           //     1000 is half on
                           //      500 is quarter on, etc.
```

Each GPIO can be independently set to one of 18 different PWM frequencies. 
The selectable frequencies depend upon the sample rate which may be 1, 2, 4, 5, 8, or 10 microseconds (default 5). 

The frequencies for each sample rate are: 

```
      μs:                Hertz

       1: 40000 20000 10000 8000 5000 4000 2500 2000 1600
           1250  1000   800  500  400  250  200  100   50
       2: 20000 10000  5000 4000 2500 2000 1250 1000  800
            625   500   400  250  200  125  100   50   25
       4: 10000  5000  2500 2000 1250 1000  625  500  400
            313   250   200  125  100   63   50   25   13
       5:  8000  4000  2000 1600 1000  800  500  400  320
            250   200   160  100   80   50   40   20   10
       8:  5000  2500  1250 1000  625  500  313  250  200
            156   125   100   63   50   31   25   13    6
      10:  4000  2000  1000  800  500  400  250  200  160
            125   100    80   50   40   25   20   10    5
```
## Arduino PWM

[Arduino Pro Mini Pinout](https://www.engineersgarage.com/electronic-components/arduino-pro-mini-pinout)

### Online Interfacing Study Resources

**Remotebits Blog**

- [Arduino Reading RC Signals](http://www.remotebits.com/index.php/2017/12/06/how-to-read-the-traxxas-radio-pwm-signals-using-an-arduino/)
- [Traxxas Arduino](http://www.remotebits.com/index.php/2017/12/04/controlling-a-traxxas-esc-with-an-arduino/)
- [Compass driven RC Car](http://www.remotebits.com/index.php/2017/12/10/arduino-rc-car-with-a-compass/)

**Camelsoftware Blog**

- [Reading PWM Signals from An RC Receiver with Arduino](http://www.camelsoftware.com/2015/12/25/reading-pwm-signals-from-an-rc-receiver-with-arduino/)

Hardware interrupts
- `attachInterrupt()`
- `micros()` on `Timer2_Counter Library`

- [Timer2](https://github.com/ElectricRCAircraftGuy/eRCaGuy_TimerCounter)
- [Timer2 blog](https://www.electricrcaircraftguy.com/2014/02/Timer2Counter-more-precise-Arduino-micros-function.html)
- Is there a fork of this somewhere that doesn't use 8 different font sizes, warnings about not working, and one commit branch?

```
#define CHANNEL_1 34

volatile unsigned long timer_start;
volatile int last_interrupt_time;

void
calcSignal(void) {
  last_interrupt_time = micros(); 
  if(digitalRead(CHANNEL_1) == HIGH) {
    timer_start = micros();
  }
  else {
    if(timer_start != 0) { 
      pulse_time = ((volatile int)micros() - timer_start);
      timer_start = 0;
    }
  } 
} 
 
void
setup(void) {
    timer_start = 0; 
    attachInterrupt(CHANNEL_1, calcSignal, CHANGE);
    Serial.begin(115200);
} 
 
void
loop(void) {
  Serial.println(pulse_time);
  delay(20);
}
```

## Servo Models

- Traxxas 2056
- Traxxas 2075
- Futaba S3305
- Savox 0251
- Hitecs 5625

## Servo PWM Chart

Compare 500 to 2500μs pulse widths with DACs.

500μs = 0.5ms

-  **8-bit** - only good for about `100μs` steps
- **10-bit** - 4x the resolution of 8-bit, good for about `25μs` steps
- **12-bit** - 8x the resolution of 8-bit, good for about `5μs` steps

Any more precision and you will be dealing with signal transmission noise & jitter.

|  µs  | Degrees |   Duty | 8-bit | 10-bit | 12-bit |   1M   |
|-----:|--------:|-------:|------:|-------:|-------:|-------:|
|  500 |     0.0 |  2.50% |     6 |     26 |    102 |  25000 |
|  550 |     4.5 |  2.75% |     7 |     28 |    113 |  27500 |
|  600 |     9.0 |  3.00% |     8 |     31 |    123 |  30000 |
|  650 |    13.5 |  3.25% |     8 |     33 |    133 |  32500 |
|  700 |    18.0 |  3.50% |     9 |     36 |    143 |  35000 |
|  750 |    22.5 |  3.75% |    10 |     38 |    154 |  37500 |
|  800 |    27.0 |  4.00% |    10 |     41 |    164 |  40000 |
|  850 |    31.5 |  4.25% |    11 |     44 |    174 |  42500 |
|  900 |    36.0 |  4.50% |    12 |     46 |    184 |  45000 |
|  950 |    40.5 |  4.75% |    12 |     49 |    195 |  47500 |
| 1000 |    45.0 |  5.00% |    13 |     51 |    205 |  50000 |
| 1050 |    49.5 |  5.25% |    13 |     54 |    215 |  52500 |
| 1100 |    54.0 |  5.50% |    14 |     56 |    225 |  55000 |
| 1150 |    58.5 |  5.75% |    15 |     59 |    236 |  57500 |
| 1200 |    63.0 |  6.00% |    15 |     61 |    246 |  60000 |
| 1250 |    67.5 |  6.25% |    16 |     64 |    256 |  62500 |
| 1300 |    72.0 |  6.50% |    17 |     67 |    266 |  65000 |
| 1350 |    76.5 |  6.75% |    17 |     69 |    276 |  67500 |
| 1400 |    81.0 |  7.00% |    18 |     72 |    287 |  70000 |
| 1450 |    85.5 |  7.25% |    19 |     74 |    297 |  72500 |
| 1500 |    90.0 |  7.50% |    19 |     77 |    307 |  75000 |
| 1550 |    94.5 |  7.75% |    20 |     79 |    317 |  77500 |
| 1600 |    99.0 |  8.00% |    20 |     82 |    328 |  80000 |
| 1650 |   103.5 |  8.25% |    21 |     84 |    338 |  82500 |
| 1700 |   108.0 |  8.50% |    22 |     87 |    348 |  85000 |
| 1750 |   112.5 |  8.75% |    22 |     90 |    358 |  87500 |
| 1800 |   117.0 |  9.00% |    23 |     92 |    369 |  90000 |
| 1850 |   121.5 |  9.25% |    24 |     95 |    379 |  92500 |
| 1900 |   126.0 |  9.50% |    24 |     97 |    389 |  95000 |
| 1950 |   130.5 |  9.75% |    25 |    100 |    399 |  97500 |
| 2000 |   135.0 | 10.00% |    26 |    102 |    410 | 100000 |
| 2050 |   139.5 | 10.25% |    26 |    105 |    420 | 102500 |
| 2100 |   144.0 | 10.50% |    27 |    108 |    430 | 105000 |
| 2150 |   148.5 | 10.75% |    28 |    110 |    440 | 107500 |
| 2200 |   153.0 | 11.00% |    28 |    113 |    451 | 110000 |
| 2250 |   157.5 | 11.25% |    29 |    115 |    461 | 112500 |
| 2300 |   162.0 | 11.50% |    29 |    118 |    471 | 115000 |
| 2350 |   166.5 | 11.75% |    30 |    120 |    481 | 117500 |
| 2400 |   171.0 | 12.00% |    31 |    123 |    492 | 120000 |
| 2450 |   175.5 | 12.25% |    31 |    125 |    502 | 122500 |
| 2500 |   180.0 | 12.50% |    32 |    128 |    512 | 125000 |

Tables made with assistance using [Tables Generator](https://www.tablesgenerator.com/markdown_tables)
