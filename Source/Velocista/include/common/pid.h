#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>
#include <stdio.h>

class PID{
public:
    PID(float pKp, float pKi, float pKd, float pInt_saturation = 2000);

    float update(float error, float dt);

    void reset();

    void setKp(float pKp){  kp = pKp; printf("Kp set to: %.2f\n", kp); }
    void setKi(float pKi){  ki = pKi; printf("Ki set to: %.2f\n", ki); }
    void setKd(float pKd){  kd = pKd; printf("Kd set to: %.2f\n", kd); }

    float getKp(){ return kp;}
    float getKi(){ return ki;}
    float getKd(){ return kd;}

private:
    float kp, ki, kd;

    float prev_error;
    float acum_integral;

    float int_saturation;

    uint8_t first_run;

};


#endif //__PID_H__