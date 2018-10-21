# Hardware vs Software PWM

Raspberry Pi’s SoC supports hardware PWM, accessible via pi.hardware_PWM() using pigpio. Only a handful of specific GPIO pins are usable for PWM:

12 PWM channel 0 All models but A and B
13 PWM channel 1 All models but A and B
18 PWM channel 0 All models
19 PWM channel 1 All models but A and B
40 PWM channel 0 Compute module only
41 PWM channel 1 Compute module only
45 PWM channel 1 Compute module only
52 PWM channel 0 Compute module only
53 PWM channel 1 Compute module only

"Frequencies above 30 MHz are unlikely to work."


```
sudo pigpiod
python
import pigpio
pi=pigpio.pi()
pi.hardware_PWM(12, 800, 1e6*0.25) # 800Hz 25% dutycycle
```


Lots refer to this hardware. Maybe support it.
[Adafruit 16-Channel 12-bit PWM/Servo Driver – I2C interface – PCA96855](http://www.adafruit.com/products/815)


Pi blaster vs ServoBlaster? 2013 article.

The BCM2835 contains 2 independent PWM channels (0 and 1), each of which be connnected to a limited subset of GPIO pins. The following GPIO pins may be connected to the following PWM channels:

  GPIO PIN    RPi pin  PWM Channel    ALT FUN
  12                    0             0
  13                    1             0         <--
  18          1-12      0             5         <--
  19                    1             5
  40                    0             0
  41                    1             0
  45                    1             0
  52                    0             1
  53                    1             1

Both PWM channels are driven by the same PWM clock, whose clock dvider can be varied using  bcm2835_pwm_set_clock()

The PWM clock can be set to control the PWM pulse widths. The PWM clock is derived from a 19.2MHz clock. You can set any divider, but some common ones are provided by the bcm2835PWMClockDivider

Each PWM channel can run in either Balanced or Mark-Space mode.

```
typedef enum
{
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

You'll probably need to set the PWM mode to 'Mark-Space' mode for most use cases and sanity reasons as described above.
In Mark-Space mode, the hardware sets the output HIGH for DATA clock pulses wide, followed by LOW for RANGE-DATA clock pulses.

In this mode, the duration that each 'pulse' is HIGH vs LOW is determined by the ratio of the PWM data to the PWM range - this is irrespective of the PWM clock speed.

The PWM range is effectively the 'resolution' or number of possible 'divisions' of each pulse. The more divisions the higher the resolution and thus more states encodable for a given pulse width.

The 'duty cycle' is the ratio of PWM data to PWM range expressed as a percentage. A PWM range of 10 with PWM data of 8 is an 80% duty cycle.

To calculate your required frequency, pick a clock divisor that make sense for your application - I chose 16, which equates to 1.2Mhz. So:

const A4_RANGE = 1.2e6 / 440;  // 1.2Mhz/440Hz
A4Data = A4_RANGE / 2; // 2727.27 / 2 = 1363.636

const F4S_RANGE = 1.2e6 / 370;  // 1.2Mhz/370Hz
F4SData = F4S_RANGE / 2;

const C4S_RANGE = 1.2e6 / 277;  // 1.2Mhz/277Hz
C4SData = C4S_RANGE / 2;


If you wanted to drive a servo at say 50Hz the same range calculation holds true:

PWM Range = PWM frequency / Desired Output Frequency

500us to 2500us

period is 50us.?
http://www.camelsoftware.com/2015/12/25/reading-pwm-signals-from-an-rc-receiver-with-arduino/
Using ISR

A pulse length of 1500 microseconds will drive a standard servo to half way. 1000 microseconds is full travel in one direction and 2000 microseconds is full travel in the other direction. There are 20 milliseconds between each pulse (50Hz).

 A PWM range of 10 with PWM data of 8 is an 80% duty cycle.

So if range is 20,000 us and data is 1,000 us you get 5% duty.

20.000 ms pulses

0.500 ms full?  (0.25% duty)

1.000 ms reverse (5% duty)

1.500 ms idle (7.5% duty)

2.000 ms fw (10% duty)



http://www.codecubix.eu/index.php/2018/05/29/hardware-pwm-with-raspberry-pi-zero/
Several software interfaces are available to manage directly these hardware components as :

– Native C code. (direct register access)
– BCM2835 library
– PIGPIO library

Finally I choose to use the pigio library because of its simplicity and nice documentation.

Configuration C :

GPIO 19 / PWM 1
Duty Cycle : 50%
Frequency : 1 MHz


The PWM pulses are timed using the DMA controller on the Pi, so the resulting signal is much more accurate than software PWM (stuff you get with RPi.GPIO), and more than sufficient for smooth servo control.

http://abyz.co.uk/rpi/pigpio/index.html
https://www.youtube.com/watch?v=aMx6ySzFOzc

# PiGpio

http://abyz.me.uk/rpi/pigpio/cif.html#gpioHardwarePWM

12  PWM channel 0  All models but A and B
13  PWM channel 1  All models but A and B
18  PWM channel 0  All models
19  PWM channel 1  All models but A and B


13 and 18 are used.


## Servos


http://abyz.me.uk/rpi/pigpio/cif.html#gpioServo


### Example

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

### Example

```
gpioSetPWMrange(24, 2000); // Now 2000 is fully on
                           //     1000 is half on
                           //      500 is quarter on, etc.
```




The actual number of steps beween off and fully on is the integral part of 250 million divided by PWMfreq. 

The actual frequency set is 250 million / steps.

There will only be a million steps for a PWMfreq of 250. Lower frequencies will have more steps and higher frequencies will have fewer steps. PWMduty is automatically scaled to take this into account.


So if we want a freq of 100kHz (10us) or 1 MHz (1us)
we get 2500 steps or with 1MHz 250 steps.

Credits
The PWM and servo pulses are timed using the DMA and PWM peripherals. 
This use was inspired by Richard Hirst's servoblaster kernel module. 
See https://github.com/richardghirst/PiBits/tree/master/ServoBlaster

int gpioSetMode(unsigned gpio, unsigned mode)
gpioSetMode(23, PI_ALT0);  // Set GPIO23 (13)
gpioSetMode(1, PI_ALT5); // Set GPIO1 (18)

gpio: 0-53
mode: 0-7

### mode
The operational mode of a GPIO, normally INPUT or OUTPUT. 

PI_INPUT 0
PI_OUTPUT 1
PI_ALT0 4
PI_ALT1 5
PI_ALT2 6
PI_ALT3 7
PI_ALT4 3
PI_ALT5 2


if (gpioGetMode(17) != PI_ALT0) {
   gpioSetMode(17, PI_ALT0);
}

int gpioPWM(unsigned user_gpio, unsigned dutycycle)

Starts PWM on the GPIO, dutycycle between 0 (off) and range (fully on). Range defaults to 255. 
This and the servo functionality use the DMA and PWM or PCM peripherals to control and schedule the pulse lengths and dutycycles. 
user_gpio: 0-31
dutycycle: 0-range


The gpioSetPWMrange function may be used to change the default range of 255. 

Example
gpioPWM(17, 255); // Sets GPIO17 full on.
gpioPWM(18, 128); // Sets GPIO18 half on.
gpioPWM(23, 0);   // Sets GPIO23 full off.

If a hardware clock is active on the GPIO the reported dutycycle will be 500000 (500k) out of 1000000 (1M). 
If hardware PWM is active on the GPIO the reported dutycycle will be out of a 1000000 (1M). 

int gpioSetPWMrange(unsigned user_gpio, unsigned range)
Selects the dutycycle range to be used for the GPIO. Subsequent calls to gpioPWM will use a dutycycle between 0 (off) and range (fully on). 

user_gpio: 0-31
    range: 25-40000


Returns the real range for the given GPIO's frequency if OK, otherwise PI_BAD_USER_GPIO or PI_BAD_DUTYRANGE. 

If PWM is currently active on the GPIO its dutycycle will be scaled to reflect the new range. 

The real range, the number of steps between fully off and fully on for each frequency, is given in the following table. 

                       1MHz  ~~~.
                                V
  25,   50,  100,  125,  200,  250,  400,   500,   625,
 800, 1000, 1250, 2000, 2500, 4000, 5000, 10000, 20000
                         /\
                          \___ 100kHz


The real value set by gpioPWM is `(dutycycle * real range) / range` 

### Example

gpioSetPWMrange(24, 2000); // Now 2000 is fully on
                           //     1000 is half on
                           //      500 is quarter on, etc.



Each GPIO can be independently set to one of 18 different PWM frequencies. 

The selectable frequencies depend upon the sample rate which may be 1, 2, 4, 5, 8, or 10 microseconds (default 5). 

The frequencies for each sample rate are: 

```
                       Hertz

       1: 40000 20000 10000 8000 5000 4000 2500 2000 1600
           1250  1000   800  500  400  250  200  100   50

       2: 20000 10000  5000 4000 2500 2000 1250 1000  800
            625   500   400  250  200  125  100   50   25

       4: 10000  5000  2500 2000 1250 1000  625  500  400
            313   250   200  125  100   63   50   25   13
sample
 rate
 (us)  5:  8000  4000  2000 1600 1000  800  500  400  320   <---- 5us sample rate is 200Hz
            250   200   160  100   80   50   40   20   10

       8:  5000  2500  1250 1000  625  500  313  250  200
            156   125   100   63   50   31   25   13    6

      10:  4000  2000  1000  800  500  400  250  200  160
            125   100    80   50   40   25   20   10    5
```

pigpio has C, Python, and command line methods to choose the settings.
From the command line
`pigs hp GPIO 50 dutycycle`

will start PWM at 50 Hz on GPIO (12, 13, 18, or 19) with dutycycle.
Dutycycle is 0 for 0% (off) or 1,000,000 for 100% (fully on).

0.8 ms is (0.8/20)% or a dutycycle of 40,000. 2.1 ms is (2.1/20)% or a dutycycle of 105,000

So for GPIO 18 with 0,8 ms pulses use
`pigs hp 18 50 40000`

By the way pigpio and servoblaster will let you generate servo PWM on any of the GPIO.
The pigpio method to generate a servo pulse of 0,8 ms on GPIO 23 is

`pigs servo 23 800`
800 denotes the desired pulse width in microseconds

Having range 200 it give you a resolution of 0.1ms. 


```
There is C sample code to read signals from i2c--can monitor BMC stuff perhaps?

There is also a servo pulse demo.

Minimal GPIO Access is another set of C demo code.

You will need to adjust the dutycycle to get a pulsewidth within the servos range once you decide on the frequency.
```

## Servo Models

This is a replacement Traxxas High Torque Waterproof Servo. 2056
80 oz-in torque
Transit time 0.23 sec/60°
This product was added to our catalog on February 29, 2008

The 2075 will give you better performance than the 2056.
Futaba s3305, Savox 0251 or one of the above mentioned Hitecs (the 5625 is my personal favourite for a 2WD basher).

```
Microseconds	Degrees	Pulse Width
500	0.0	2.50%
510	0.9	2.55%
520	1.8	2.60%
530	2.7	2.65%
540	3.6	2.70%
550	4.5	2.75%
560	5.4	2.80%
570	6.3	2.85%
580	7.2	2.90%
590	8.1	2.95%
600	9.0	3.00%
610	9.9	3.05%
620	10.8	3.10%
630	11.7	3.15%
640	12.6	3.20%
650	13.5	3.25%
660	14.4	3.30%
670	15.3	3.35%
680	16.2	3.40%
690	17.1	3.45%
700	18.0	3.50%
710	18.9	3.55%
720	19.8	3.60%
730	20.7	3.65%
740	21.6	3.70%
750	22.5	3.75%
760	23.4	3.80%
770	24.3	3.85%
780	25.2	3.90%
790	26.1	3.95%
800	27.0	4.00%
810	27.9	4.05%
820	28.8	4.10%
830	29.7	4.15%
840	30.6	4.20%
850	31.5	4.25%
860	32.4	4.30%
870	33.3	4.35%
880	34.2	4.40%
890	35.1	4.45%
900	36.0	4.50%
910	36.9	4.55%
920	37.8	4.60%
930	38.7	4.65%
940	39.6	4.70%
950	40.5	4.75%
960	41.4	4.80%
970	42.3	4.85%
980	43.2	4.90%
990	44.1	4.95%
1000	45.0	5.00%
1010	45.9	5.05%
1020	46.8	5.10%
1030	47.7	5.15%
1040	48.6	5.20%
1050	49.5	5.25%
1060	50.4	5.30%
1070	51.3	5.35%
1080	52.2	5.40%
1090	53.1	5.45%
1100	54.0	5.50%
1110	54.9	5.55%
1120	55.8	5.60%
1130	56.7	5.65%
1140	57.6	5.70%
1150	58.5	5.75%
1160	59.4	5.80%
1170	60.3	5.85%
1180	61.2	5.90%
1190	62.1	5.95%
1200	63.0	6.00%
1210	63.9	6.05%
1220	64.8	6.10%
1230	65.7	6.15%
1240	66.6	6.20%
1250	67.5	6.25%
1260	68.4	6.30%
1270	69.3	6.35%
1280	70.2	6.40%
1290	71.1	6.45%
1300	72.0	6.50%
1310	72.9	6.55%
1320	73.8	6.60%
1330	74.7	6.65%
1340	75.6	6.70%
1350	76.5	6.75%
1360	77.4	6.80%
1370	78.3	6.85%
1380	79.2	6.90%
1390	80.1	6.95%
1400	81.0	7.00%
1410	81.9	7.05%
1420	82.8	7.10%
1430	83.7	7.15%
1440	84.6	7.20%
1450	85.5	7.25%
1460	86.4	7.30%
1470	87.3	7.35%
1480	88.2	7.40%
1490	89.1	7.45%
1500	90.0	7.50%
1510	90.9	7.55%
1520	91.8	7.60%
1530	92.7	7.65%
1540	93.6	7.70%
1550	94.5	7.75%
1560	95.4	7.80%
1570	96.3	7.85%
1580	97.2	7.90%
1590	98.1	7.95%
1600	99.0	8.00%
1610	99.9	8.05%
1620	100.8	8.10%
1630	101.7	8.15%
1640	102.6	8.20%
1650	103.5	8.25%
1660	104.4	8.30%
1670	105.3	8.35%
1680	106.2	8.40%
1690	107.1	8.45%
1700	108.0	8.50%
1710	108.9	8.55%
1720	109.8	8.60%
1730	110.7	8.65%
1740	111.6	8.70%
1750	112.5	8.75%
1760	113.4	8.80%
1770	114.3	8.85%
1780	115.2	8.90%
1790	116.1	8.95%
1800	117.0	9.00%
1810	117.9	9.05%
1820	118.8	9.10%
1830	119.7	9.15%
1840	120.6	9.20%
1850	121.5	9.25%
1860	122.4	9.30%
1870	123.3	9.35%
1880	124.2	9.40%
1890	125.1	9.45%
1900	126.0	9.50%
1910	126.9	9.55%
1920	127.8	9.60%
1930	128.7	9.65%
1940	129.6	9.70%
1950	130.5	9.75%
1960	131.4	9.80%
1970	132.3	9.85%
1980	133.2	9.90%
1990	134.1	9.95%
2000	135.0	10.00%
2010	135.9	10.05%
2020	136.8	10.10%
2030	137.7	10.15%
2040	138.6	10.20%
2050	139.5	10.25%
2060	140.4	10.30%
2070	141.3	10.35%
2080	142.2	10.40%
2090	143.1	10.45%
2100	144.0	10.50%
2110	144.9	10.55%
2120	145.8	10.60%
2130	146.7	10.65%
2140	147.6	10.70%
2150	148.5	10.75%
2160	149.4	10.80%
2170	150.3	10.85%
2180	151.2	10.90%
2190	152.1	10.95%
2200	153.0	11.00%
2210	153.9	11.05%
2220	154.8	11.10%
2230	155.7	11.15%
2240	156.6	11.20%
2250	157.5	11.25%
2260	158.4	11.30%
2270	159.3	11.35%
2280	160.2	11.40%
2290	161.1	11.45%
2300	162.0	11.50%
2310	162.9	11.55%
2320	163.8	11.60%
2330	164.7	11.65%
2340	165.6	11.70%
2350	166.5	11.75%
2360	167.4	11.80%
2370	168.3	11.85%
2380	169.2	11.90%
2390	170.1	11.95%
2400	171.0	12.00%
2410	171.9	12.05%
2420	172.8	12.10%
2430	173.7	12.15%
2440	174.6	12.20%
2450	175.5	12.25%
2460	176.4	12.30%
2470	177.3	12.35%
2480	178.2	12.40%
2490	179.1	12.45%
2500	180.0	12.50%
```