#include "DFRobot_TFmini.h"
/**
  * \brief Common Define
*/
#define largeDirPin  2
#define largePin     3
#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11

/**
  * \brief set up global var
*/
int i=0;
int pulse_per_round= 8000;
double posX=0;
double posY=0;
double posY_inner=0;
int timer_2_counter = 0;
int step_number = 0;
int a=0;
int count_posY=0;
bool measure_flag = false;
int serialRead;
bool stop_flag = false;

SoftwareSerial mySerial(0, 1); // RX, TX

DFRobot_TFmini  TFmini;
uint16_t distance;

void setup()
{
    Serial.begin(115200);
    TFmini.begin(mySerial);
    /* Declare Pins As Output */
    pinMode(largeDirPin, OUTPUT);
    pinMode(largePin, OUTPUT);
    digitalWrite(largeDirPin, HIGH);
    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);

    cli();//stop interrupts
    
    //set timer1 interrupt at 200Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 16000hz increments
    OCR1A = 9999;// = (16*10^6) / (200*8) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12 bits for 256 prescaler
    TCCR1B |= (1 << CS11);
    // enable timer compare interrupt
    TIMSK1 |= (0 << OCIE1A);

    //set timer2 interrupt at 1000
    // TCCR2A = 0;// set entire TCCR2A register to 0
    // TCCR2B = 0;// same for TCCR2B
    // TCNT2  = 0;//initialize counter value to 0
    // // set compare match register for 1000 increments
    // OCR2A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
    // // turn on CTC mode
    // TCCR2A |= (1 << WGM21);
    // // Set CS22 for 64 
    // TCCR2B |= (1 << CS22);   
    // // enable timer compare interrupt
    // TIMSK2 |= (1 << OCIE2A);

    sei();//allow interrupts   
}

ISR(TIMER1_COMPA_vect)
{
  if(!stop_flag)
  {
    if(i < (pulse_per_round))
    {
      /* If i%2 ==0 -> Set High */
      if(0 == i%2)
      {
        digitalWrite(largePin, HIGH);
      }
      /* If i%2 ==0 -> Set Low */
      else
      {
        digitalWrite(largePin, LOW);
        posX += double(360)/(pulse_per_round/2);
        if(360 <= posX)
        {
          posX = 0;
        }
      }
      if((i >= (pulse_per_round - 8)) && (i <= (pulse_per_round - 1))){
          OneStep();
          if (i == (pulse_per_round - 1)) 
            posY = posY_inner;
      }
    }
    else
    {
      i=-1; // Set i=-1 then ++ i =0
    } 

    i++;

    if(distance <=360)
    {  
      if(posY<10)
        Serial.println(String(posX,2)+ ","+ String(posY,2) +"," + String(distance));
      else{
        Serial.println("End Data");
        stop_flag = true;
      }
    }
  }
  else{
    /* Do nothing */
  }
}  

void loop()
{
    if(false==measure_flag)
    {
      if(Serial.available() > 0)
      {
          serialRead= Serial.readString().toInt();
          if(serialRead==5)
          {
            TIMSK1 |= (1 << OCIE1A);
            measure_flag = true;
          }
      }
    }

    else if(true==measure_flag)
    {
      if(TFmini.measure())
      {                      //Measure Distance and get signal strength
          distance = TFmini.getDistance();       //Get distance data
      }
    }
}

void OneStep()
{
 switch(step_number)
    {
    case 0:
    digitalWrite(STEPPER_PIN_1, HIGH);
    digitalWrite(STEPPER_PIN_2, LOW);
    digitalWrite(STEPPER_PIN_3, LOW);
    digitalWrite(STEPPER_PIN_4, LOW);
    break;
    case 1:
    digitalWrite(STEPPER_PIN_1, LOW);
    digitalWrite(STEPPER_PIN_2, HIGH);
    digitalWrite(STEPPER_PIN_3, LOW);
    digitalWrite(STEPPER_PIN_4, LOW);
    break;
    case 2:
    digitalWrite(STEPPER_PIN_1, LOW);
    digitalWrite(STEPPER_PIN_2, LOW);
    digitalWrite(STEPPER_PIN_3, HIGH);
    digitalWrite(STEPPER_PIN_4, LOW);
    break;
    case 3:
    digitalWrite(STEPPER_PIN_1, LOW);
    digitalWrite(STEPPER_PIN_2, LOW);
    digitalWrite(STEPPER_PIN_3, LOW);
    digitalWrite(STEPPER_PIN_4, HIGH);
    break;
    }
    step_number++;
    posY_inner+=((float)360/2048); 
    if(step_number > 3)
    {
        step_number = 0;
        
    }
}
