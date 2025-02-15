#include "mbed.h"
#include "QEI.h"
#include "C12832.h"
DigitalOut myled(LED1);
C12832 lcd(D11, D13, D12, D7, D10);
QEI rEncL(PA_0,PB_0,NC,256,QEI::X2_ENCODING); // CHANGE FIRST 2 PINS, DIGITAL INPUT, KEEP REST SAME 
QEI rEncR(PA_0,PB_0,NC,256,QEI::X2_ENCODING); // CHANGE FIRST 2 PINS, DIGITAL INPUT . KEEP REST SAME
double rpmL=0.0;
double rpmR=0.0;

int main() {
rEncL.reset();
rEncR.reset();
rpmL=rEncL.getPulses()*60/256;
rpmR=rEncR.getPulses()*60/256;
    lcd.printf("Left Wheel RPM: %f \n",rpmL);
    lcd.printf("Right Wheel RPM: %f \n",rpmR);
  while(rpmR>0||rpmL>0) {
    myled = 1;
    wait(0.2);
    myled = 0;
    wait(0.2);
  }
}
