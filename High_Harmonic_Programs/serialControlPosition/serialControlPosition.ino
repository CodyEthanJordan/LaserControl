#include <AccelStepper.h>
#include <EEPROM.h>

AccelStepper gratingStepper;
AccelStepper shutterStepper;

int angle = 0;
long newAngle = 0;
const int MaxChars = 6;
char strValue[MaxChars+1];
int index = 0;

int gratingSpeed = 700;
int shutterSpeed = 500;

bool shutterClosed = true;
int shutterOpenPosition = 15;

bool gratingArrivedAtTarget = true;
bool confirmMovement = false;
bool movementConfirmed = true;

int eeAddress = 0;

void setup()
{  
   Serial.begin(9600);
   gratingStepper = AccelStepper(AccelStepper::DRIVER, 7, 8);
   gratingStepper.setEnablePin(6);
   gratingStepper.setMaxSpeed(gratingSpeed);
   gratingStepper.setSpeed(gratingSpeed);

   shutterStepper = AccelStepper(AccelStepper::DRIVER, 10, 11);
   shutterStepper.setEnablePin(9);
   shutterStepper.setMaxSpeed(shutterSpeed);
   shutterStepper.setSpeed(shutterSpeed);

   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // loads up zero position from memory after power up
  long zeroPosition;
  EEPROM.get(eeAddress, zeroPosition);
  gratingStepper.setCurrentPosition(zeroPosition);
}

void loop()
{  
  gratingStepper.runSpeedToPosition(); 
  shutterStepper.runSpeedToPosition();

  if(!gratingArrivedAtTarget)
  {
    if (gratingStepper.distanceToGo() == 0 && !gratingArrivedAtTarget)
    {
      gratingArrivedAtTarget = true;
      if(confirmMovement && !movementConfirmed)
      {
        Serial.println(gratingStepper.currentPosition());
        movementConfirmed = true;
      }
    }
  }
  //save current position in EEPROM in case device gets turned off
  EEPROM.put(eeAddress, gratingStepper.currentPosition());
}

void serialEvent()
{
   while(Serial.available() > 0) 
   {
      char ch = Serial.read();

      switch(ch)
      {
        case 's': //stop
          gratingStepper.stop();
          break;
        case 'r': //reset
          index = 0;
          confirmMovement = false;
          gratingStepper.stop();
          break;
        case 'z': //set current position as 0
          gratingStepper.setCurrentPosition(0);
          break;
        case 'm': //expect movement command
          index = 0;
          confirmMovement = false;
          break;
        case 'M': //expect movement command with confirmation
          index = 0;
          confirmMovement = true;
          movementConfirmed = false;
          break;
        case 'p': //report current position
          Serial.println(gratingStepper.currentPosition());
          break;
        case '#': //end number and move to value
              strValue[index] = 0; 
              newAngle = atol(strValue); 
              gratingStepper.moveTo(newAngle);
              gratingStepper.setMaxSpeed(gratingSpeed);
              gratingStepper.setSpeed(gratingSpeed); 
              index = 0;
              gratingArrivedAtTarget = false;
              break;
        default: //read in numbers to move motor
          if((index < MaxChars && isDigit(ch)) || (index == 0 && ch == '-')) { 
            strValue[index++] = ch; 
           }
//          else 
//          { 
//              strValue[index] = 0; 
//              newAngle = atol(strValue); 
//              gratingStepper.moveTo(newAngle);
//              gratingArrivedAtTarget = false;
//              gratingStepper.setMaxSpeed(gratingSpeed);
//              gratingStepper.setSpeed(gratingSpeed); 
//              index = 0;
//          }  
      }

//      if (ch == 's') //engage shutter
//      {
//        if(shutterClosed)
//        {
//          shutterStepper.moveTo(shutterOpenPosition);
//          shutterClosed = false;
//        }
//        else
//        {
//          shutterStepper.moveTo(0);
//          shutterClosed = true;
//        }
//
//        shutterStepper.setMaxSpeed(shutterSpeed);
//        shutterStepper.setSpeed(shutterSpeed);
//      }
     
   }
   
}
