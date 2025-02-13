#include "mbed.h"

class Motor
{
    protected:
    PwmOut outputPWM;
    DigitalOut operationMode;
    DigitalOut direction;
   

    public:
    Motor(PinName pwm, PinName mode, PinName dir): outputPWM(pwm), operationMode(mode), direction(dir){
        outputPWM.period(0.4f);
    }

    void bipolarMode(float speed){

        outputPWM.write(speed);
        operationMode.write(1);
        direction.write(0);
    }

    void unipolar1Mode(float speed){
        outputPWM.write(speed);
        operationMode.write(0);
        direction.write(1);
    }

    void unipolar2Mode(float speed){
        outputPWM.write(speed);
        operationMode.write(0);
        direction.write(0);
    }
};

int main() {
    DigitalOut enable (PC_3);
    enable.write(1);
    Motor leftmotor(PB_7, PA_13, PA_14); //change PH0 and PH1
    Motor rightmotor(PA_15, PC_10, PC_12); ///change PC_14 and PC_15
    //leftmotor.bipolarMode();
    //rightmotor.bipolarMode();

  while(1) {
  }
}
