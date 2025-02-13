#include "mbed.h"

// MOTOR CLASS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Motor                                     //Begin Motor class definition
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

    void unipolarforwardMode(float speed){
        outputPWM.write(speed);
        operationMode.write(0);
        direction.write(1);
    }

    void unipolarreverseMode(float speed){
        outputPWM.write(speed);
        operationMode.write(0);
        direction.write(0);
    }
};

// POTENTIOMETER CLASSES ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Potentiometer                                 //Begin Potentiometer class definition
{
private:                                            //Private data member declaration
    AnalogIn inputSignal;                           //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; //Float variables to speficy the value of VDD and most recent samples

public:                                             // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                      //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);            //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    
    float amplitudeNorm(void)                       //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                  //Returns the ADC value normalised to range 0.0 - 1.0
    }
    
    void sample(void)                               //Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read();       //Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
        currentSampleVolts = currentSampleNorm * VDD; //Converts the normalised value to the equivalent voltage (0.0 - 3.3 V) and stores this information
    }
    
    float getCurrentSampleVolts(void)               //Public member function to return the most recent sample from the potentiometer (in volts)
    {
        return currentSampleVolts;                  //Return the contents of the data member currentSampleVolts
    }
    
    float getCurrentSampleNorm(void)                //Public member function to return the most recent sample from the potentiometer (normalised)
    {
        return currentSampleNorm;                   //Return the contents of the data member currentSampleNorm  
    }

};

class SamplingPotentiometer : public Potentiometer  //Begin SamplingPotentiometer which inherits from the potentiometer class 
{    
private:                                            //Private data member declaration
    float samplingFrequency, samplingPeriod;        //Float variables
    Ticker sampler;                                 //Declaration of ticker sampler object

public:                                             //Public declarations
    SamplingPotentiometer(PinName p, float v, float fs) : Potentiometer(p,v) {sampler.attach(callback(this,&SamplingPotentiometer::sample),1.0/fs);} //Constructor - accept the pin name, the VDD for the analog input and the sampling frequency as parameters

};

// MAIN FUNCTION //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    DigitalOut enable (PC_3);                           // asigns enable to pin 
    enable.write(1);                                    // writes a 1 to the enable pin in order to turn on the motor board
    Motor leftmotor(PB_7, PA_13, PA_14);                // creates an object for the left motor  
    Motor rightmotor(PA_15, PC_10, PC_12);              // creates an object for the right motor

    SamplingPotentiometer left_pot(A0, 3.3, 2);         // creates an object for left_potentiometer
    SamplingPotentiometer right_pot(A1, 3.3, 2);        // creates an object for right_potentiometer
    //float leftspeed left_pot.getCurrentSampleNorm();    // assigns value of left potentiometer to variable left speeed 
    //float rightspeed right_pot.getCurrentSampleNorm();  // assigns value of right potentiometer to variable right speed

    rightmotor.unipolarforwardMode(0.2f);
    leftmotor.unipolarforwardMode(0.2f);


  while(1) {
  }
}
