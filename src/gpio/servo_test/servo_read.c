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
  
  // lib init requirement
  if (gpioInitialise() < 0) {
    fprintf(stderr, "%d\n", PI_INIT_FAILED);
    return -1;
  }
  
  while (1) {
   fprintf(stderr, "Servo 0 pwm: %d\n", gpioGetServoPulsewidth(pwm_0.gpio));
   fprintf(stderr, "Servo 1 pwm: %d\n", gpioGetServoPulsewidth(pwm_1.gpio));
   fprintf(stderr, "while loop complete\n");
  }
  
  // Release DMA channels, memory, threads
  gpioTerminate();
  
  return 0;
}
