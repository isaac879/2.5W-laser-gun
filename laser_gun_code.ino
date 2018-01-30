/*------------------------------------------------------------------------------------------------------------------------------------------------------*
 * This code is written for my 2.5W laser gun project. The laser gun in controlled by an ATtiny85 microcontroller running at 16MHz clock speed.
 * 
 * CHECK ALL THE TODOs BEFORE RUNNING THE CODE!
 * 
 * This project uses a 11.1V 2200mAh Li-Po battery, 2.5W laser module and some custom circuitry.
 * 
 * The laser module is powered directly from the battery and controlled by the ATtiny85 by a PWM signal. A 5V voltage regulator is used for ATtiny85. The
 * battery level is measured by an analog input of the ATtiny85 through a potential divider so the maximum voltage it will be exposed to is 5V (The minimum 
 * value will need to be calibrated for your design by changing the value of the MINIMUM_BATTERY_LEVEL define). The trigger is connected to a potentiometer
 * and will need to be calibrated to control how the laser fires. This version of code was created assuming that as the trigger is pressed the potentiometer 
 * reading will get lower. If you have wired the potentiometer power and ground the opposite way, the value will increase instead and you will need to change 
 * some of the logic in the code or re-wire the potentiometer. (Edit the TRIGGER_****** Defines for you calibrated values.)
 * 
 * Last updated 30/01/2018
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE
 * 
 * 
 *Code written by isaac879
 *------------------------------------------------------------------------------------------------------------------------------------------------------*/

//Pin declarations
#define BATERY_LEVEL_PIN A1
#define TRIGGER_PIN A2
#define MODE_PIN A3
#define GREEN_LED_PIN PB0
#define LASER_PIN PB1

//Low battery voltage warning threshold. 
//TODO: This will need to be calibrated for your battery and potential divider values.
#define MINIMUM_BATTERY_LEVEL 1023 * 3.8 / 5 

//Fireing mode definitions
#define MODE_SAFETY 0
#define MODE_SEMI_AUTO 1
#define MODE_BURST 2
#define MODE_FULL_AUTO 3
#define MODE_BEAM 4

//Mode potentiometer values
#define MODE_SAFETY_MIN 5
#define MODE_SAFETY_MAX 4 * 1023/5

#define MODE_SEMI_AUTO_MIN 4 * 1023/5
#define MODE_SEMI_AUTO_MAX 3 * 1023/5

#define MODE_BURST_MIN 3 * 1023/5
#define MODE_BURST_MAX 2 * 1023/5

#define MODE_FULL_AUTO_MIN 2 * 1023/5
#define MODE_FULL_AUTO_MAX 1 * 1023/5

#define MODE_BEAM_MIN 1 * 1023/5
#define MODE_BEAM_MAX 0 * 1023/5

//TODO: Edit these TRIGGER_ defines to match the calibration values for your potentiometer
#define TRIGGER_NOT_PRESSED_THRESHOLD 10
#define TRIGGER_SIGHT_THRESHOLD 485
#define TRIGGER_PRESSED 350
#define TRIGGER_HALF_PRESSED 415
#define TRIGGER_POSITION_TOLERANCE 10
#define TRIGGER_RELEASE_THRESHOLD 5

#define LASER_SIGHT_POWER 1

#define MAX_LASER_POWER 255
#define LASER_BULLET_ON_TIME 50
#define LASER_BULLET_DELAY 150

#define LED_TIMER_MAX 2500

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

//Global variables
bool low_battery = false;
int mode = MODE_SAFETY;
unsigned long led_timing_counter = 0;

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void checkBatteryLevel(){
    if(analogRead(BATERY_LEVEL_PIN) < MINIMUM_BATTERY_LEVEL){
        low_battery = true;
    }
    else{
        low_battery = false;
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setMode(){
    int modePot = analogRead(MODE_PIN);
    //printi("mode pot: ", modePot,"\n\n");
    if(modePot <= MODE_SAFETY_MIN && modePot > MODE_SAFETY_MAX ){
        mode = MODE_SAFETY;
    }
    else if(modePot <= MODE_SEMI_AUTO_MIN && modePot > MODE_SEMI_AUTO_MAX){
        mode = MODE_SEMI_AUTO;
    }
    else if(modePot <= MODE_BURST_MIN && modePot > MODE_BURST_MAX){
        mode = MODE_BURST;
    }
    else if(modePot <= MODE_FULL_AUTO_MIN && modePot > MODE_FULL_AUTO_MAX){
        mode = MODE_FULL_AUTO;
    }
    else if(modePot <= MODE_BEAM_MIN && modePot >= MODE_BEAM_MAX){
        mode = MODE_BEAM;
    }
    else{
        mode = MODE_SAFETY;
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setLED(){
    //Safety - LED off
    //battery low - Dimly lit   
    // semi auto - short pulse
    //burst - 3 short puleses 
    //ful auto - rapid pulses
    //beam - bright solid
    
    led_timing_counter++;
    if(led_timing_counter > LED_TIMER_MAX){
        led_timing_counter = 0;
    }
    
    if(low_battery){
        if(led_timing_counter % 30 == 0){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else{
             digitalWrite(GREEN_LED_PIN, LOW);
        }
    }
    else if(mode == MODE_SAFETY){
        digitalWrite(GREEN_LED_PIN, LOW);
    }
    else if(mode == MODE_SEMI_AUTO){
        if(led_timing_counter < LED_TIMER_MAX/16){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 8 * LED_TIMER_MAX/16){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 9 * LED_TIMER_MAX/16){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else{
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        
    }
    else if(mode == MODE_BURST){
        if(led_timing_counter < LED_TIMER_MAX / 20){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 3 * LED_TIMER_MAX / 15){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 4 * LED_TIMER_MAX / 15){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 6 *  LED_TIMER_MAX / 15){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 7 * LED_TIMER_MAX / 15){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 9 *  LED_TIMER_MAX / 15){
            digitalWrite(GREEN_LED_PIN, LOW);
        } 
    } 
    else if(mode == MODE_FULL_AUTO){
        if(led_timing_counter < 1 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 2 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 3 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 4 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 5 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 6 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }
        else if(led_timing_counter < 7 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, LOW);
        }
        else if(led_timing_counter < 8 * LED_TIMER_MAX / 8){
            digitalWrite(GREEN_LED_PIN, HIGH);
        }    
    }
    else if(mode == MODE_BEAM){
        digitalWrite(GREEN_LED_PIN, HIGH);
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setLaserPower(byte pwm){//PWM 0 - 255
    analogWrite(LASER_PIN, pwm);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void fire(){
    digitalWrite(GREEN_LED_PIN, LOW);
    if(mode == MODE_SEMI_AUTO){
        setLaserPower(MAX_LASER_POWER);//Laser on
        delay(LASER_BULLET_ON_TIME);//delay
        setLaserPower(0);//laser off
        delay(LASER_BULLET_DELAY);//delay
        while(analogRead(TRIGGER_PIN) < TRIGGER_PRESSED + TRIGGER_RELEASE_THRESHOLD){};//wait for trigger to be released
    }
    else if(mode == MODE_BURST){
        for(int i = 0; i < 3; i++){//Repeats 3 time for burst mode
            setLaserPower(MAX_LASER_POWER);//Laser on
            delay(LASER_BULLET_ON_TIME);//delay
            setLaserPower(0);//laser off
            delay(LASER_BULLET_DELAY);//delay
        } 
        while(analogRead(TRIGGER_PIN) < TRIGGER_PRESSED + TRIGGER_RELEASE_THRESHOLD){};//wait for trigger to be released
    }
    else if(mode == MODE_FULL_AUTO){
        while(analogRead(TRIGGER_PIN) < TRIGGER_PRESSED + TRIGGER_RELEASE_THRESHOLD){//Loop until the trigger to be released
            setLaserPower(MAX_LASER_POWER);//Laser on
            delay(LASER_BULLET_ON_TIME);//delay
            setLaserPower(0);//laser off
            delay(LASER_BULLET_DELAY);//delay
        }
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void fireBeam(){
    int triggerPosition = analogRead(TRIGGER_PIN);
    int beamPower = 0;
    while(triggerPosition < TRIGGER_SIGHT_THRESHOLD){
        triggerPosition = analogRead(TRIGGER_PIN);
        beamPower = map(triggerPosition, TRIGGER_SIGHT_THRESHOLD, TRIGGER_PRESSED, LASER_SIGHT_POWER, MAX_LASER_POWER);//sets beam power proportional to the trigger press
        if(beamPower < 0){//Confines beamPower to its valid range
            beamPower = 0;
        }
        else if(beamPower > 255){
            beamPower = 255;
        }
        beamPower = (beamPower * beamPower) / 255; //Exponential curve for the laser beam power
        setLaserPower(beamPower);
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setup(){   
    pinMode(BATERY_LEVEL_PIN, INPUT);
    pinMode(TRIGGER_PIN, INPUT);
    pinMode(MODE_PIN, INPUT);
    pinMode(GREEN_LED_PIN, OUTPUT); 
    setLaserPower(0);//Sets the Laser off

    for(int i = 0; i < 10; i++){ //Rapid LED pulses to show it has been switched on
        digitalWrite(GREEN_LED_PIN, HIGH);
        delay(80);
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(80);
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

void loop(){    
    setMode();//checks the fireing mode and sets it
    checkBatteryLevel();//Checks the battery level
    setLED();//Displays the mode on the LED

    int triggerPosition = analogRead(TRIGGER_PIN);//read the trigger position
    if(triggerPosition < TRIGGER_SIGHT_THRESHOLD && triggerPosition > TRIGGER_PRESSED &&  mode != MODE_SAFETY && mode != MODE_BEAM){//If the trigger is slightly pressed the aiming beam will come on.
        setLaserPower(LASER_SIGHT_POWER);
    }
    else if(triggerPosition <= TRIGGER_PRESSED && mode != MODE_BEAM && mode != MODE_SAFETY){//If the trigger is fully pressed it will fire
       fire();//TODO: if it fires don't exit until the trigger is released
    }
    else if(triggerPosition < TRIGGER_SIGHT_THRESHOLD && mode == MODE_BEAM){//If in beam mode it will fire a beam
       fireBeam();
    }
    else{
        setLaserPower(0);
    }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/
