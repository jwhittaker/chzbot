#ifndef SERVO_TEST_H
#define SERVO_TEST_H


typedef struct pwm_channel pwm_channel;

int init_pwm(pwm_channel pwm);

int servo(pwm_channel pwm, unsigned int duty);

char* mode_label(int mode);

int run_pwm(pwm_channel pwm, unsigned int duty);

#endif
