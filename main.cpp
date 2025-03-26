#include "mbed.h"

//GLOBAL Variables
Ticker PID_ticker;
Timer PID_timer;
float correction_value; //Stores the sum of P+I+D each time time the PID_function run
float previous_error = 0; // Accumulates previous errors 
float integral = 0; // Used in the I part of the controller to account for accumulatted error and correct the offset
float previous_time = 0; // Creates variable used in PID_calulation function which stores the previous time

// MOTOR CLASS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Motor                                     // Begin Motor class definition
{
    protected:
    PwmOut outputPWM;
    DigitalOut operationMode;
    DigitalOut direction;
   
    public:
    Motor(PinName pwm, PinName mode, PinName dir): outputPWM(pwm), operationMode(mode), direction(dir){
        outputPWM.period(0.01f);
    }

    //void bipolarMode(float speed){
    //    outputPWM.write(speed);
    //    operationMode.write(1);
    //    direction.write(0);
    // }

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

    void stop(){
        outputPWM.write(1);
        operationMode.write(0);
        direction.write(1);
    }
};

// TCRT CLASS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TCRT                                      // Begin TCRT class definition 
{
    protected:
    DigitalOut LED;
    AnalogIn IR_DET;
    bool ENABLE;
    float VDD, currentSampleNorm, currentSampleVolts;

    public:
    TCRT(PinName IR_LED, PinName IR_DETECTOR): LED(IR_LED), IR_DET(IR_DETECTOR) {
        ENABLE = false;
        VDD=5.0;
    }

    void toggle(void)
    {
        ENABLE=!ENABLE;
        LED.write(ENABLE);
    }

    bool enable(void){return ENABLE;}

    float amplitudeVolts(void) { return (IR_DET.read() * VDD); }
    float amplitudeNorm(void) { return IR_DET.read(); }
    void sample(void) {
        currentSampleNorm = IR_DET.read();
        currentSampleVolts = currentSampleNorm * VDD;
    }
    float getCurrentSampleVolts(void) { return currentSampleVolts; }
    float getCurrentSampleNorm(void) { return IR_DET.read(); }
};

//Define and initialize TCRT objects for PID error calculation
TCRT TCRTL2 (PB_2, PC_4);   // create the -2 (left most) TCRT object
TCRT TCRTL1 (PB_3, PB_1);   // create the -1 (second) TCRT object
TCRT TCRT0 (PB_15, PC_2);   // create the 0 (middle/reference) TCRT object
TCRT TCRTR1 (PB_14, PC_3);  // create the 1 (fourth) TCRT object
TCRT TCRTR2 (PC_8, PC_5);   // create the 2 (right most) TCRT object

// PID calculation function
void PID_calculation(){
    // PID controller variable definition
    float Kp = 0.8; // Proportional gain constant definition (TUNE FIRST --> increase until buggy follows the line)
    float Ki = 0; // Integral gain constant definition (ADD LAST --> improve ossilations)
    float Kd = 0.0; // Derivative gain constant definition (ADD SECOND --> increase for faster corrections (slow correction??))
    float current_time = PID_timer.read();
    // Calculate dt (time elapsed between sampling (period))
    float dt = current_time - previous_time;
    previous_time = current_time; //Updates for the next loop iteration
    // Position calculation -- a negative value represents the line being to the right while a positive value represents the line being to the left
    float error = (-2 * TCRTL2.getCurrentSampleNorm()) + (-1 * TCRTL1.getCurrentSampleNorm()) + (1 * TCRTR1.getCurrentSampleNorm()) + (2 * TCRTR2.getCurrentSampleNorm());   //TCRT0 is not included as this is the reference LED (origin) and has no weight in the error calculation
    // PID values calculation
    float P = Kp * error;   // Calculates the P term -- accounts and corrects for current error
    integral += error * dt; //Adds the current error, scaled by elapsed time (period), to the integral sum -- acounts for past error
    float I = Ki * integral; // Calculates the I term -- accounts for past errors and corrects drifting 
    float D = Kd * (error - previous_error) / dt ;  //Calculates the D term -- predicts fture error in order to correct ossilations
    // Store error for next iteration
    previous_error = error;
    // Return calculated value
    correction_value = P + I + D;
}
   
int main(void){

    // definition, creation and initialization of objects
    DigitalOut enable (PC_6);               // asigns enable to pin 
    enable.write(1);                        // writes a 1 to the enable pin in order to turn on the motor board
    Motor rightmotor(PB_7, PA_13, PA_14);   // creates an object for the left motor  
    Motor leftmotor(PA_15, PC_10, PC_12);   // creates an object for the right motor
    float left_speed = 0.4, right_speed = 0.4, base_speed = 0.4; // creates a variable for the base (normalized) speed of the motors  -- initialized as one since in our motors 1 is 0 speed
    leftmotor.unipolarforwardMode(left_speed); //initializes leftmotor object in unipolar forward mode with no speed       
    rightmotor.unipolarforwardMode(right_speed); // initializes rightmotor object in unipolar forward mode with no speed
    
    //Initilizes timers and tickers
    PID_timer.start(); //Starts the the timer to measure dt
    PID_ticker.attach(&PID_calculation, 0.01);

    while(1){

    // Toggle the LEDs on and off
    TCRTL2.toggle();
    TCRTL1.toggle();
    TCRT0.toggle();
    TCRTR1.toggle();
    TCRTR2.toggle();   

    // If statements to correct the speed of the wheels
    if (correction_value > 0 ){ // If the correction_value returned by the PID function is positive this means the line is to the right of the buggy therefore the right motor's speed must be decreased
    right_speed = base_speed + correction_value; // Subtracts the correction_value from the base speed of the left motor
    }
    else if (correction_value < 0){ // If the correction_value returned by the PID function is negative this means the line is to the left of the buggy therefore the left motor's speed must be decreased
    left_speed = base_speed - correction_value; // Adds the correction_value to the base speed of the right motor (acts as a subtraction since correction value is negative)
    }

    leftmotor.unipolarforwardMode(left_speed); // updates leftmotor object speed
    rightmotor.unipolarforwardMode(right_speed); // updates rightmotor object speed
    
    //if (integral > 5){
    //    integral = 0;
    //}

    }
}
