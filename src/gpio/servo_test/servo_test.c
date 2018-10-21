#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <pigpio.h>
#include "./servo_test.h"

/*
 * chzbot
 * 0.0.1
 * Joseph Whittaker
 * 10/10/2018
 * Cheese is for mice and two 50 Hz pwm signals are used (C for 100) Hz.
 */

#define MIN_WIDTH 1000
#define MAX_WIDTH 2000
#define FREQUENCY 50


typedef struct pwm_channel {
  unsigned int gpio;
  unsigned int mode;
  char* name;
} pwm_channel;


int
main(void) {
  pwm_channel pwm_0 = {18, PI_ALT5, "bcm18"};
  pwm_channel pwm_1 = {13, PI_ALT0, "bcm13"};
  int count;
  int pin;

  // lib init requirement
  if (gpioInitialise() < 0) {
    fprintf(stderr, "%d\n", PI_INIT_FAILED);
    return -1;
  }
  
  // cb. safely close on ctrl+c or kill -9; calls gpioTerminate and exits.
  //gpioSetSignalFunc(SIGINT, gpioTerminate);
  
  // setup hardware gpio
  if (init_pwm(pwm_0) != 0) {
    return -1;
  }
  if (init_pwm(pwm_1) != 0) {
    return -1;
  }

  //servo(pwm_0, 2111);
  //servo(pwm_1, 1899);
  
  run_pwm(pwm_0, 1024);
  run_pwm(pwm_1, 200);
  for (count = 0; count < 3; count++) {
   //fprintf(stderr, "Servo 0 pwm: %d\n", gpioGetServoPulsewidth(pwm_0.gpio));
   //fprintf(stderr, "Servo 1 pwm: %d\n", gpioGetServoPulsewidth(pwm_1.gpio));
   fprintf(stderr, "for loop %d\n", count);
  }
  
  
  for (pin = 0; pin < 54; pin++) {
    fprintf(stderr, "GPIO %d mode: %s   %d Hz\n", pin, mode_label(gpioGetMode(pin)), gpioGetPWMfrequency(pin)); 
  }

  //servo(pwm_0, 606);
  run_pwm(pwm_0, 8888);
  run_pwm(pwm_1, 444);
  for (count = 0; count < 3; count++) {
   //fprintf(stderr, "Servo 0 pwm: %d\n", gpioGetServoPulsewidth(pwm_0.gpio));
   //fprintf(stderr, "Servo 1 pwm: %d\n", gpioGetServoPulsewidth(pwm_1.gpio));
    fprintf(stderr, "GPIO %d PWM frequency is at %d Hz\n", pwm_0.gpio, gpioGetPWMfrequency(pwm_0.gpio));  
    fprintf(stderr, "GPIO %d PWM frequency is at %d Hz\n", pwm_1.gpio, gpioGetPWMfrequency(pwm_1.gpio));  
    fprintf(stderr, "GPIO %d PWM duty cycle is at %d\n", pwm_0.gpio, gpioGetPWMdutycycle(pwm_0.gpio));  
    fprintf(stderr, "GPIO %d PWM duty cycle is at %d\n", pwm_1.gpio, gpioGetPWMdutycycle(pwm_1.gpio));
    fprintf(stderr, "GPIO %d PWM range is at %d\n", pwm_0.gpio, gpioGetPWMrange(pwm_0.gpio));
    fprintf(stderr, "GPIO %d PWM range is at %d\n", pwm_1.gpio, gpioGetPWMrange(pwm_1.gpio));
    fprintf(stderr, "for loop %d\n", count);
  }
  
  //servo(pwm_1, 1777);
  run_pwm(pwm_0, 5000);
  run_pwm(pwm_1, 777);
  for (count = 0; count < 3; count++) {
    fprintf(stderr, "GPIO %d PWM frequency is at %d Hz\n", pwm_0.gpio, gpioGetPWMfrequency(pwm_0.gpio));  
    fprintf(stderr, "GPIO %d PWM frequency is at %d Hz\n", pwm_1.gpio, gpioGetPWMfrequency(pwm_1.gpio));  
    fprintf(stderr, "GPIO %d PWM duty cycle is at %d\n", pwm_0.gpio, gpioGetPWMdutycycle(pwm_0.gpio));  
    fprintf(stderr, "GPIO %d PWM duty cycle is at %d\n", pwm_1.gpio, gpioGetPWMdutycycle(pwm_1.gpio));  
    fprintf(stderr, "GPIO %d PWM range is at %d\n", pwm_0.gpio, gpioGetPWMrange(pwm_0.gpio));
    fprintf(stderr, "GPIO %d PWM range is at %d\n", pwm_1.gpio, gpioGetPWMrange(pwm_1.gpio));
   //fprintf(stderr, "Servo 0 pwm: %d\n", gpioGetServoPulsewidth(pwm_0.gpio));
   //fprintf(stderr, "Servo 1 pwm: %d\n", gpioGetServoPulsewidth(pwm_1.gpio));
   fprintf(stderr, "for loop %d\n", count);
  }
  // Release DMA channels, memory, threads
  gpioTerminate();
  
  return 0;
}

char*
mode_label(int mode) {
  switch (mode) {
    case 0: return "0 PI_INPUT";
    case 1: return "1 PI_OUTPUT";
    case 2: return "2 PI_ALT 5";
    case 3: return "3 PI_ALT 4";
    case 4: return "4 PI_ALT 0";
    case 5: return "5 PI_ALT 1";
    case 6: return "6 PI_ALT 2";
    case 7: return "7 PI_ALT 3";
    default: return "-1 error";
  }
}

int
init_pwm(pwm_channel pwm) {
  int rv;
    
  if (gpioGetMode(pwm.gpio) != pwm.mode) {
    rv = gpioSetMode(pwm.gpio, pwm.mode);
    if (rv != 0) {
      if (rv == -95) {
        fprintf(stderr, "%d: GPIO has no hardware PWM! ", rv); 
      }
      fprintf(stderr, "Failed to init %s: %d\n", pwm.name, PI_BAD_GPIO);
      return -1;
    }
  }
  // clear pull up/down resistors
  gpioSetPullUpDown(pwm.gpio, PI_PUD_OFF);
  gpioSetPWMrange(pwm.gpio, 10000);
  fprintf(stderr, "gpio %d mode %d mode for %s set\n", pwm.gpio, pwm.mode, pwm.name);

  return 0;
}

int
servo(pwm_channel pwm, unsigned int pulsewidth) {
  int rv;
  
  rv = gpioServo(pwm.gpio, pulsewidth);  
  if (rv != 0) {
    fprintf(stderr, "%d\n", rv);
    return -1;
  }
  fprintf(stderr, "servo on %s assigned %d μs pulsewidth\n", pwm.name, pulsewidth);
  
  return 0;
}


int
run_pwm(pwm_channel pwm, unsigned int duty) {
  int rv;
  
  rv = gpioHardwarePWM(pwm.gpio, FREQUENCY, duty);
  if (rv != 0) {
    fprintf(stderr, "%d\n", rv);
    return -1;
  }
  fprintf(stderr, "servo on %s assigned %d duty\n", pwm.name, duty);
  
  return 0;
}




/*
int
test_stuff(unsigned int gpio, unsigned int duty) {

  // Starts PWM on the GPIO, dutycycle between 0 (off) and range (fully on). Range defaults to 255. 
  // The gpioSetPWMrange function may be used to change the default range of 255. 
  // if servo is 1000-2000, then range 1000 is appropriate. if it is 500-2500 then 2000.

  unsigned int range = 1000;
  
  gpioSetPWMrange(1, range);
  gpioSetPWMrange(23, range);
  
  gpioSetPWMfrequency(1, 50);
  
  gpioSetPWMfrequency(23, 50);
  
  //int gpioPWM(gpio, unsigned dutycycle)
}  
*/


  /*
    PWMfreq: 0 (off) or 1-125000000 (125M)
    PWMduty: 0 (off) to 1000000 (1M)(fully on)
    Steps between 0-100% duty = (250M/PWMfreq)
    PWMduty is automatically scaled
    The default setting is 5 microseconds using the PCM peripheral.
     
    gpioServo(17, 1000); // Move servo to safe position anti-clockwise.
    gpioServo(23, 1500); // Move servo to centre position.
    gpioServo(25, 2000); // Move servo to safe position clockwise.
    
    gpioSetPWMfrequency(24, 10000);
    gpioSetPWMrange(24, 100);
    gpioPWM(24, 50);

   */
