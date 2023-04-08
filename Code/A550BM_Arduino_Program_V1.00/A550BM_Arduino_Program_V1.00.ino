#include <Servo.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
//#include <Servo.h>


int RunProgram1 = true;
int RunProgram2 = false;
int RunProgram3 = false;

int ProgStep = 0;  //20  TEMP TESTING


//Stepper Pins
const int stepB = 25;
const int dirB  = 26;
const int enPinB = 27;

const int stepC = 28;
const int dirC  = 29;
const int enPinC = 30;


//INPUTS
const int Axis5Limit = 2;
const int Axis6Limit = 3;
const int A550Ready = 10;
const int SpareINP = 4;

int Axis5LimitState = 0;
int Axis6LimitState = 0;
int A550ReadyState = 0;
int SpareINPState = 0;


//OUTPUTS
const int EnableServosPin = 44;
const int ArduinoReady = 45;
const int SpareOP1 = 46;
const int SpareOP2 = 47;


//Bolean
int RobotHomed = true;//false TEMP TESTING


// Define some steppers and the pins the will use
AccelStepper Axis5(AccelStepper::DRIVER, stepB, dirB); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

AccelStepper Axis6(AccelStepper::DRIVER, stepC, dirC); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

Servo Gripper;

//Servo EggFlipper; //0 = Upside Down, 180 = Rightside Up

Servo EggCracker;//0 = Fully Open,   180 = Fully Closed

const byte enablePinB = enPinB;  // ***** pin 8 is the enable pin
const byte enablePinC = enPinC;  // ***** pin 8 is the enable pin


void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Steppers
  pinMode(enablePinB, OUTPUT); // **** set the enable pin to output
  pinMode(enablePinC, OUTPUT); // **** set the enable pin to output
  
  digitalWrite(enablePinB, LOW); // *** set the enable pin low
  digitalWrite(enablePinC, LOW); // *** set the enable pin low

  pinMode(stepB,OUTPUT);
  pinMode(dirB,OUTPUT);

  pinMode(stepC,OUTPUT);
  pinMode(dirC,OUTPUT);

  Gripper.attach(9);
  //EggFlipper.attach(8);
  EggCracker.attach(7);

  digitalWrite(dirB,HIGH);

  digitalWrite(dirC,LOW);

  Axis5.setMaxSpeed(1000000000.0);
  Axis5.setAcceleration(20000.0);
    
  Axis6.setMaxSpeed(100000000.0);
  Axis6.setAcceleration(10000.0);

  //Setup IO
  //INPUTS
  pinMode (Axis5Limit, INPUT);
  pinMode (Axis6Limit, INPUT);
  pinMode (A550Ready, INPUT_PULLUP);
  pinMode (SpareINP, INPUT);

  //OUTPUTS
  pinMode (EnableServosPin, OUTPUT);
  pinMode (ArduinoReady, OUTPUT);
  pinMode (SpareOP1, OUTPUT);
  pinMode (SpareOP2, OUTPUT);

  //Set Outputs
  digitalWrite (EnableServosPin, LOW);
  digitalWrite (ArduinoReady, LOW);
  digitalWrite (SpareOP1, LOW);
  digitalWrite (SpareOP2, LOW);
}

void StepperOneMove (int Degrees, int Direction, int SSpeed) {
  //convert steps to degrees 
  Degrees = Degrees * (14000 / 711);
  
  //Sets Direction
  if (Direction == 0) {
    digitalWrite(dirC,LOW);
  }else if (Direction == 1) {
    digitalWrite(dirC,HIGH);
  }

  //Enables the motor to move in a particular direction
  //Makes 200 pulses for making one full cycle rotation
  
  for(int x = 0; x < Degrees; x++) {
    digitalWrite(stepC,HIGH);

    delayMicroseconds(pow(1.06,((SSpeed * -1) + 139.5)));//800 home speed, 10 == 100%, 100 = 50%, 3000 = 1%

    digitalWrite(stepC,LOW);

    delayMicroseconds(pow(1.06,((SSpeed * -1) + 139.5)));//1000
  }
}

void SendReadySig() {
  digitalWrite(ArduinoReady, HIGH);
  //Serial.println("Output D45 HIGH");
  delay(50);
  digitalWrite(ArduinoReady, LOW);
  //Serial.println("Output D45 LOW");
  Serial.println("Ready Signal Sent");
}

void ProgRun() {
  //Check if robot was homed
  if (RobotHomed == false) {
    Serial.println("ERROR Robot Not Homed!");
    Serial.println("ProgRun Terminated");
  }
  else {
    Serial.println("Program Running..");

    //reset step count
    //ProgStep = 0;
  
    A550ReadyState = digitalRead(A550Ready);
    while (A550ReadyState == 1) {//wait for A550 Ready Signal
      A550ReadyState = digitalRead(A550Ready);
    }
    SendReadySig();//send ready signal back
    //Ready Signals succes, run program
    //Serial.println("I Shouldn't be running..");
    //ProgStep = 1;//Start Step Counter
    delay(1000);

    //Run through program steps
    while (ProgStep <= 34) {//run program for total steps
      //wait until A550ReadyState is a HIGH
      A550ReadyState = digitalRead(A550Ready);
      while (A550ReadyState == 1) {
        A550ReadyState = digitalRead(A550Ready);
        UserInput();
      }
      if (A550ReadyState == 0) {
        Serial.println("A550 Ready Sig Recieved");
        SendReadySig();
        ProgStep +=1;
        Serial.println("ProgStep +=1 = "); Serial.println(ProgStep);
        delay(15);
      }
  
      //Run Program Steps
      //Pick up sausage
      if (ProgStep == 1) {
        delay(250);
        Serial.println("STEP grip sausage");
        EnableServos();
        //Close Gripper, grab sausage
        Serial.println("Gripper= 15deg");
        Gripper.write(8);
        delay(400);

        //Tilt Manipulator Up
        Serial.println("Jog Axis 5= 500");
        Axis5.setMaxSpeed(100000.0);
        Axis5.setAcceleration(1000.0);
        Axis5.move(500);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        Serial.println("grip sausage complete");
        
        //Send ready signal to keep arm moving
        SendReadySig();

        //Make move while arm is moving
        delay(300);

        //Tilt Down Closer to Sausage roller
        Serial.println("Jog Axis 5= -800");
        Axis5.setMaxSpeed(1000000.0);
        Axis5.setAcceleration(10000.0);
        Axis5.move(-800);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
      }
      
      if (ProgStep == 2) {
        Serial.println("STEP Cook Sausage");

        //Move Gripper to open pos
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(50);
        
        //Move A5 to 0 Pos
        Serial.println("Jog Axis 5= 300");
        Axis5.setMaxSpeed(100000.0);
        Axis5.setAcceleration(1000.0);
        Axis5.move(300);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //Wait for Manipulator to clear cooker
        delay(50);
/*
        //Do a trick and move -100
        //Move A6 Half Turn
        Serial.println("Jog Axis 6= 180");
        Axis6.setMaxSpeed(10000.0);//100000000.0
        Axis6.setAcceleration(8000.0);//25000.0
        Axis6.move(180);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }*/

        //Move A6 Negitive Half Turn
        Serial.println("Jog Axis 6= -100");//-180, -280
        Axis6.setMaxSpeed(10000.0);//100000000.0
        Axis6.setAcceleration(8000.0);//25000.0
        Axis6.move(-100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //wait for manipulator to move over power dial
        delay(1000);

        //Close Gripper, grab Power Dial
        Serial.println("Grab Power Dial");
        Serial.println("Gripper= 20deg");
        Gripper.write(20);
        delay(400);

        //Turn Power Dial On
        Serial.println("Turn Power Dial On");
        Serial.println("Jog Axis 6= -130");
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(100.0);
        Axis6.move(-130);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //Open Gripper, Release Power Dial
        Serial.println("Release Power Dial");
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(400);

        //Move Back to Zero
        Serial.println("Move Back to Zero");
        Serial.println("Jog Axis 6= 128");
        Axis6.setMaxSpeed(10000.0);//100000
        Axis6.setAcceleration(8000.0);//15000
        Axis6.move(128);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        
        Serial.println("Cook Sausage Complete!");
      }

      if (ProgStep == 3) {
        Serial.println("STEP Grab Tool");

        EnableServos();
        //Grab Tool
        Serial.println("Gripper= 50deg");
        Gripper.write(50);
        delay(200);
          
        Serial.println("STEP Grab Tool complete");
      }

      if (ProgStep == 4) {
        Serial.println("STEP Grab Egg");

        //Tilt Manipulator Up
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000.0);
        Axis5.setAcceleration(100.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
          
        Serial.println("STEP Grab Egg complete");
      }

      if (ProgStep == 5) {
        Serial.println("STEP Set Egg Cracker");

        //EnableServos();
        //Set Egg Flipper to Right Side Up
        //Serial.println("EggFlipper= 180deg");
        //EggFlipper.write(180);
        delay(200);

        //Set Egg Cracker to Open
        Serial.println("EggCracker= 0deg");
        EggCracker.write(0);
        delay(200);
          
        Serial.println("STEP Set Egg Cracker Complete");
      }

      if (ProgStep == 6) {
        Serial.println("STEP Drop Egg");

        //Delay for timing the egg drop
        delay(1000);
        Serial.println("delay(2500); Complete");
        
        //Tilt Manipulator Down
        Serial.println("Jog Axis 5= -300");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(100.0);
        Axis5.move(-300);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //delay(800);
        //Serial.println("delay(800); Complete");

        //Tilt Manipulator To 0
        Serial.println("Jog Axis 5= 100");
        Axis5.setMaxSpeed(500000.0);
        Axis5.setAcceleration(1000.0);
        Axis5.move(100);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //delay(800);
        //Serial.println("delay(800); Complete");

        //Crack Egg  
        Serial.println("Crack Egg");

        EnableServos();
        //Crack Egg
        Serial.println("EggCracker= 180deg");
        EggCracker.write(180);
        delay(200);
        
        
        Serial.println("STEP Drop Egg complete");
      }

      if (ProgStep == 7) {
        Serial.println("STEP Release Tool");

        delay(500);
        Serial.println("delay(500); Complete");
        
        EnableServos();
        //Release Tool
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(200);

        //Send Mirobot Ready Signal 2/5/23 ADD
        //Wait For Robot 1 to Clear Workspace
        delay(2000);
        Serial.println("Send Mirobot Ready Signal");
        digitalWrite(SpareOP2, HIGH);
        Serial.println("Output D47 HIGH");
        delay(200);
        digitalWrite(SpareOP2, LOW);
        Serial.println("Output D47 LOW");
        Serial.println("Mirobot Ready Signal Sent");

        //ReCrack Egg (3sec)
        Serial.println("EggCracker= 180deg");
        EggCracker.write(0);
        delay(1000);
        //Crack Egg
        Serial.println("EggCracker= 180deg");
        EggCracker.write(180);
        delay(200);

        delay(10800);//13800
        Serial.println("delay(10800); Complete");

        //Release Crack Egg  
        Serial.println("Release Crack Egg");

        EnableServos();
        //Crack Egg
        Serial.println("EggCracker= 0deg");
        EggCracker.write(0);
        delay(200);
          
        Serial.println("STEP Release Tool complete");
      }

      if (ProgStep == 8) {
        Serial.println("STEP Dump Egg Shels");

        //delay(500);
        //Serial.println("delay(500); Complete");
        
        //EnableServos();
        //Dump Egg Shels
        //Serial.println("EggFlipper= 0deg");
        //EggFlipper.write(0);
        //delay(200);

        //delay(1000);
        //Serial.println("delay(1000); Complete");

        //Egg Flipper 0  
        //Serial.println("Egg Flipper to 0");

        //EnableServos();
        //Egg Flipper 0
        //Serial.println("EggFlipper= 90deg");
        //EggFlipper.write(90);
        delay(200);
          
        Serial.println("STEP Dump Egg Shels Complete");
      }

      if (ProgStep == 9) {
        Serial.println("STEP Cook Egg Relay");

        Serial.println("Turn On EggCooker (k3)");
        digitalWrite(SpareOP1, HIGH);
        Serial.println("Output D46 HIGH");
          
        Serial.println("STEP Cook Egg Relay Complete");
      }

      if (ProgStep == 10) {
        Serial.println("STEP Grab Bread");

        EnableServos();
        //Grab Bread
        Serial.println("Gripper= 0deg");
        Gripper.write(0);
        delay(200);
          
        Serial.println("STEP Grab Bread complete");
      }
      
      if (ProgStep == 11) {
        Serial.println("STEP Release Bread");

        EnableServos();
        //Release Bread
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(1200);
          
        Serial.println("STEP Release Bread complete");
      }

      if (ProgStep == 12) {
        Serial.println("STEP Cook Bread");

        EnableServos();
        //Release Bread
        Serial.println("Gripper= 10deg");
        Gripper.write(10);
        delay(200);

        //Move A6 Half Turn
        Serial.println("Jog Axis 6= 200");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(200);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //Tilt Manipulator Down 90 Deg
        Serial.println("Jog Axis 5= -700");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(-700);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
          
        Serial.println("STEP Cook Bread complete");
      }

      if (ProgStep == 13) {
        Serial.println("STEP Open Coffee Maker");

      ///** TEMP TESTING
        //Wait For Manipulator to Clear Toaster
        delay(150);

        //Tilt Manipulator Up 90 Deg
        Serial.println("Jog Axis 5= 950");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(950);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //Move A6 Neg Half Turn
        Serial.println("Jog Axis 6= -200");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(-200);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        
        EnableServos();
        //Release Bread
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(200);
        //**/

/**
        //Tilt Manipulator Up ~35 Deg
        Serial.println("Jog Axis 5= 950");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(250);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //Move A6 Neg Half Turn
        Serial.println("Jog Axis 6= -100");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(-102);//-100
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
**/

        Serial.println("STEP Open Coffee Maker complete");
      }

      if (ProgStep == 14) {
        Serial.println("STEP Grab KCup");

        //Wait For Manipulator to Clear Coffee Maker
        delay(1000);

        //Tilt Manipulator Down ~35 Deg
        Serial.println("Jog Axis 5= -250");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(-250);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        Serial.println("STEP Grab KCup complete");
      }

      if (ProgStep == 15) {
        Serial.println("STEP Pick KCup");

        //Wait for Manipulator to be on KCup
        delay(200);

        EnableServos();
        //Grip KCup
        Serial.println("Gripper= 60deg");
        Gripper.write(60);
        delay(200);

        //Tilt Manipulator Up ~35 Deg
        Serial.println("Jog Axis 5= 250");
        Axis5.setMaxSpeed(100000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(250);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
          
        Serial.println("STEP Pick KCup complete");
      }

      if (ProgStep == 16) {
        Serial.println("STEP Place KCup");

        //Wait For KCup To Be Over Port
        delay(300);
        
        EnableServos();
        //Grip KCup
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        //delay(200);

        Serial.println("STEP Place KCup complete");
      }

      if (ProgStep == 17) {
        Serial.println("STEP Initial Press");

        //Wait For Manipulator To Clear Coffee Maker
        delay(25);

        EnableServos();
        //Close Gripper
        Serial.println("Gripper= 10deg");
        Gripper.write(10);
        
        //Tilt Manipulator Down ~35 Deg
        Serial.println("Jog Axis 5= -250");
        Axis5.setMaxSpeed(100000.0);
        Axis5.setAcceleration(50000.0);
        Axis5.move(-250);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        
        //delay(200);

        Serial.println("STEP Initial Press complete");
      }

      if (ProgStep == 18) {
        Serial.println("STEP Click Button Press");

        //Move A6 Neg Half Turn
        Serial.println("Jog Axis 6= -100");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(-100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //Prep For Finish Bread
        //Wait For Button to Be Pressed
        delay(5000);
        //Move A6 Pos Half Turn
        Serial.println("Jog Axis 6= 100");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        EnableServos();
        //Open Gripper For Bread
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        //delay(200);
        
        Serial.println("STEP Click Button complete");

        //Decrece Chance for goast signals
        Serial.println("Waiting for Approiximate Toast Completion.. (95sec)");
        //delay(95000);

        //Home
        FindHome();
        //Move A6 back to orig pos
        Serial.println("Jog Axis 6= -102");
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(8000.0);
        Axis6.move(-102);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        EnableServos();
        //Open Gripper For Bread
        Serial.println("Gripper= 70deg");
        Gripper.write(70);

        
      }


      //ADD WHEN INTIGRATION:
      //a6 +100
      //g   90

      //DONE SEE 725
      

      if (ProgStep == 19) {//////////////Finishbread Prog (19-20 )////////////////
        Serial.println("STEP Grab Bread");

        //Wait For Manipulator To Be closer to bread
        delay(25);

        //Tilt Manipulator Down 200 stp
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        EnableServos();
        //Close Gripper
        Serial.println("Gripper= 30deg");
        Gripper.write(30);

        //Wait for gripper to reach position
        delay(500);

        //Tilt Manipulator Down 25 stp
        Serial.println("Jog Axis 5= -25");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(-25);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        EnableServos();
        //Close Gripper
        Serial.println("Gripper= 0deg");
        Gripper.write(0);

        //Wait for gripper to reach position
        delay(100);

        //Tilt Manipulator Up 350 stp to clear toaster
        Serial.println("Jog Axis 5= 350");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(350);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        Serial.println("STEP Grab Bread complete");
      }

      if (ProgStep == 20) {
        Serial.println("STEP Release Bread");

        //Wait For Manipulator To Be closer to Plate
        delay(1000);

        //Tilt Manipulator Down 125 stp to 0
        Serial.println("Jog Axis 5= -155");//-125, -175
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(-155);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        EnableServos();
        //Open Gripper
        Serial.println("Gripper= 90deg");
        Gripper.write(90);

        Serial.println("STEP Release Bread complete");

        //Decrece Chance for goast signals
        Serial.println("Waiting for Approiximate Egg Completion.. (170sec)");
        //delay(170000);

        //Home
        FindHome();
        //Move A6 back to orig pos
        Serial.println("Jog Axis 6= -102");
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(8000.0);
        Axis6.move(-102);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        EnableServos();
      }
      

      if (ProgStep == 21) {///////////////FINISHEGG PROG, (21-26)//////////////////////
        Serial.println("STEP Cook Egg Relay OFF");

        Serial.println("Turn Off EggCooker (k3-)");
        digitalWrite(SpareOP1, LOW);
        Serial.println("Output D46 LOW");
          
        Serial.println("STEP Cook Egg Relay OFF Complete");
      }

      if (ProgStep == 22) {
        Serial.println("STEP Grab Fork");

        EnableServos();
        //Grab Tool
        Serial.println("Gripper= 50deg");
        Gripper.write(50);
        delay(200);
          
        Serial.println("STEP Grab Fork complete");
      }

      if (ProgStep == 23) {
        Serial.println("STEP Stab Egg");

        //Move A6 Half Turn and Tilt Manipulator Down 90 Deg
        Serial.println("Jog Axis 6= 200");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(10000.0);
        Axis6.move(200);
        Serial.println("Jog Axis 5= -700");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(10000.0);
        Axis5.move(-260);
        while (Axis6.distanceToGo() != 0 || Axis5.distanceToGo() != 0) {
          Axis6.run();
          Axis5.run();
        }
        if (Axis6.distanceToGo() == 0 && Axis5.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
          Serial.println("Axis 5 Move Complete");
        }

        Serial.println("STEP Stab Egg complete");
      }

      if (ProgStep == 24) {
        Serial.println("STEP Pick Up Egg");

        //Wait for egg to get stabbed all the way 
        delay(200);

        Serial.println("Jog Axis 5= 150");//150
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(500.0);
        Axis5.move(150);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
          
        Serial.println("STEP Pick Up Egg complete");
      }

      if (ProgStep == 25) {
        Serial.println("STEP Shake Egg");

        //Flip Axis 6
        Serial.println("Jog Axis 6= 200");
        Axis6.setMaxSpeed(10000000.0);
        Axis6.setAcceleration(1000.0);
        Axis6.move(200);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //Tilt Manipulator Down 150 Stps
        Serial.println("Jog Axis 5= -150");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-150);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Down 200 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Down 200 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 25 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }

        //Tilt Manipulator Up to zero
        Serial.println("Jog Axis 5= 250");//200, 260, 210
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(250);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
          
        Serial.println("STEP Shake Egg complete");
      }

      if (ProgStep == 26) {
        Serial.println("STEP Release Fork");

        //Wait For Manipulator to be over platform
        delay(250);

        EnableServos();
        //Open Gripper
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        
        Serial.println("STEP Release Fork complete");
      }

      if (ProgStep == 27) {///////////////FINISHSAUSAGE PROG, (27-33)//////////////////////
        Serial.println("STEP Turn Sausage Roller Off");

        //Move A6 Negitive Quarter Turn
        Serial.println("Jog Axis 6= -100");//-280
        Axis6.setMaxSpeed(100000000.0);
        Axis6.setAcceleration(25000.0);
        Axis6.move(-100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        
        //wait for manipulator to move over power dial
        delay(200);

        EnableServos();
        //Close Gripper, grab Power Dial
        Serial.println("Grab Power Dial");
        Serial.println("Gripper= 20deg");
        Gripper.write(20);
        delay(400);

        //Turn Power Dial On
        Serial.println("Turn Power Dial On");
        Serial.println("Jog Axis 6= 100");//-130
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(100.0);
        Axis6.move(100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }

        //Open Gripper, Release Power Dial
        Serial.println("Release Power Dial");
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        delay(400);

        //Move Back to Zero
        //Serial.println("Move Back to Zero");
        //Serial.println("Jog Axis 6= -30");//128
        //Axis6.setMaxSpeed(100000000.0);
        //Axis6.setAcceleration(25000.0);
        //Axis6.move(-30);
        //while (Axis6.distanceToGo() != 0) {
        //  Axis6.run();
        //}
        //if (Axis6.distanceToGo() == 0) {
        //  Serial.println("Axis 6 Move Complete");
        //}
        
        Serial.println("STEP Turn Sausage Roller Off complete");
      }
      
      if (ProgStep == 28) {
        Serial.println("STEP Grip Fork");

        //Wait For
        delay(0);

        EnableServos();
        //Close Gripper
        Serial.println("Gripper= 50deg");
        Gripper.write(50);

        //wait for arm to be in traverse
        delay(900);

        //Move a6 to 90deg to pierce sausage
        Serial.println("a6 to 90deg to pierce sausage");
        Serial.println("Jog Axis 6= -100");
        Axis6.setMaxSpeed(1000.0);
        Axis6.setAcceleration(15000.0);
        Axis6.move(-100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        
        Serial.println("STEP Grip Fork complete");
      }

      if (ProgStep == 29) {
        Serial.println("STEP Tilt to Stab Sausage");

        //Wait For 
        delay(0);
        
        //Tilt Manipulator Down 225 Stps
        Serial.println("Jog Axis 5= -225");
        Axis5.setMaxSpeed(10000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(-225);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        
        //delay(200);

        Serial.println("STEP Tilt to Stab Sausage Complete");
      }

      if (ProgStep == 30) {
        Serial.println("STEP Tilt Sausage up");

        //Wait For 
        delay(0);
        
        //Tilt Manipulator Up 325 Stps
        Serial.println("Jog Axis 5= 325");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(325);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        //Move A6 Neg Half Turn
        Serial.println("Jog Axis 6= -100");//-200
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(2500.0);
        Axis6.move(-100);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        
        //delay(200);

        Serial.println("STEP Tilt Sausage up Complete");
      }

      if (ProgStep == 31) {
        Serial.println("STEP Wipe Sausage");

        //Wait For 
        delay(0);
        
        //Tilt Manipulator Up 325 Stps
        Serial.println("Jog Axis 5= -300");
        Axis5.setMaxSpeed(100.0);
        Axis5.setAcceleration(5000.0);
        Axis5.move(-300);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        //delay(200);

        Serial.println("STEP Wipe Sausage Complete");
      }

      if (ProgStep == 32) {
        Serial.println("STEP Shake Sausage");

        //Wait For 
        delay(0);
        
        //Tilt Manipulator Up 275 Stps
        Serial.println("Jog Axis 5= 275");
        Axis5.setMaxSpeed(1000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(275);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        //Move A6 Pos Half Turn
        Serial.println("Jog Axis 6= 200");
        Axis6.setMaxSpeed(10000.0);
        Axis6.setAcceleration(2500.0);
        Axis6.move(200);
        while (Axis6.distanceToGo() != 0) {
          Axis6.run();
        }
        if (Axis6.distanceToGo() == 0) {
          Serial.println("Axis 6 Move Complete");
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Down 200 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Down 200 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Down 200 Stps
        Serial.println("Jog Axis 5= -200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 200 Stps
        Serial.println("Jog Axis 5= 200");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(200);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        //Tilt Manipulator Up 25 Stps
        Serial.println("Jog Axis 5= -75");
        Axis5.setMaxSpeed(10000000.0);
        Axis5.setAcceleration(20000.0);
        Axis5.move(-75);
        while (Axis5.distanceToGo() != 0) {
          Axis5.run();
        }
        if (Axis5.distanceToGo() == 0) {
          Serial.println("Axis 5 Move Complete");
        }
        
        //delay(200);

        Serial.println("STEP Shake Sausage Complete");
      }

      if (ProgStep == 33) {
        Serial.println("STEP Release Fork");

        //Wait For Manipulator to be over platform
        delay(250);

        EnableServos();
        //Open Gripper
        Serial.println("Gripper= 90deg");
        Gripper.write(90);
        
        Serial.println("STEP Release Fork complete");
      }

      





      
  
      //Send Ready Signal upon step completion
      SendReadySig();
    }
  }
}

void HomeAxis5() {
  //Home Axis 5
  //If Axis Moves a Certain Distance And Sees No Limit Switch Then Home The Other Direction
  //Start In Negitive Direction
  digitalWrite(dirB,LOW);
  for(int x = 0; x < 400; x++) {
    //Read Limit Switch State
    Axis5LimitState = digitalRead(Axis5Limit);
    if(Axis5LimitState == 1) {
      digitalWrite(stepB,LOW);
  
      delayMicroseconds(8000);//800 home speed, 10 == 100%, 100 = 50%, 3000 = 1%
  
      digitalWrite(stepB,HIGH);
  
      delayMicroseconds(8000);//1000
    }else if(Axis5LimitState == 0) {
      x += 10000000;
      Serial.println("Axis 5 Limit Switch Found..");
    }
  }

  //If Axis Is In Lower End Of Zero Position
  if (Axis5LimitState == 0) {
    Axis5.setMaxSpeed(100000.0);
    Axis5.setAcceleration(1000.0);
    Axis5.move(-320);
    while (Axis5.distanceToGo() != 0) {
      Axis5.run();
    }
    if (Axis5.distanceToGo() == 0) {
      //Serial.println("");
    }
  }

  //If Axis Is In Higher End Of Zero Position
  //Read Limit Switch State
  Axis5LimitState = digitalRead(Axis5Limit);
  if (Axis5LimitState == 1) {
    //Move In In Positive Direction
    digitalWrite(dirB,HIGH);
    
    Axis5LimitState = digitalRead(Axis5Limit);
    while(Axis5LimitState == 1) {
      digitalWrite(stepB,HIGH);
  
      delayMicroseconds(8000);//800 home speed, 10 == 100%, 100 = 50%, 3000 = 1%
  
      digitalWrite(stepB,LOW);
  
      delayMicroseconds(8000);//1000
  
      Axis5LimitState = digitalRead(Axis5Limit);
      if (Axis5LimitState == 0) {
        Serial.println("Axis 5 Homed");
      }
    }
  }
}

void FindHome() {
  //Enable Steppers
  Serial.println("Steppers Enabled");
  digitalWrite(enablePinB, LOW);
  digitalWrite(enablePinC, LOW);
      
  //Home Axis 6
  //Move In In Negitive Direction
  digitalWrite(dirC,LOW);
  Axis6LimitState = digitalRead(Axis6Limit);
  while(Axis6LimitState == 1) {
    digitalWrite(stepC,HIGH);

    delayMicroseconds(8000);//800 home speed, 10 == 100%, 100 = 50%, 3000 = 1%

    digitalWrite(stepC,LOW);

    delayMicroseconds(8000);//1000

    Axis6LimitState = digitalRead(Axis6Limit);
  }

  delay(100);
  
  //Move A6 Neg Half Turn to insure were homing off of the right side of the home indicator
    Serial.println("Jog Axis 6= 100");
    Axis6.setMaxSpeed(100000.0);
    Axis6.setAcceleration(250.0);
    Axis6.move(100);
    while (Axis6.distanceToGo() != 0) {
      Axis6.run();
    }
    if (Axis6.distanceToGo() == 0) {
      Serial.println("Axis 6 Move Complete");
    }

    delay(100);
    
    //Move In In Negitive Direction to insure were homing off of the right side of the home indicator
    digitalWrite(dirC,LOW);
    Axis6LimitState = digitalRead(Axis6Limit);
    while(Axis6LimitState == 1) {
      digitalWrite(stepC,HIGH);
  
      delayMicroseconds(8000);//800 home speed, 10 == 100%, 100 = 50%, 3000 = 1%
  
      digitalWrite(stepC,LOW);
  
      delayMicroseconds(8000);//1000
  
      Axis6LimitState = digitalRead(Axis6Limit);
    }

  Serial.println("Axis 6 Zero Found..");

  Axis6.setMaxSpeed(100000.0);
  Axis6.setAcceleration(1000.0);
  Axis6.move(170);//205
  while (Axis6.distanceToGo() != 0) {
    Axis6.run();
  }
  if (Axis6.distanceToGo() == 0) {
    Serial.println("Axis 6 in Zero Position..");
  }

  //Home Axis 5
  HomeAxis5();
  
  RobotHomed = true;
  EnableServos();
  Serial.println("Gripper= 100deg");
  Gripper.write(100);
  Serial.println("EggCracker= 0deg");
  EggCracker.write(0);
  delay(1500);
  DisableServos();
  Serial.println("Robot Homed");
}

void EnableServos() {
  //Enable Servos
  digitalWrite(EnableServosPin, HIGH);
  Serial.println("Output D44 HIGH");
  Serial.println("Servos Enabled");
}

void DisableServos() {
  //Disable Servos
  digitalWrite(EnableServosPin, LOW);
  Serial.println("Output D44 LOW");
  Serial.println("Servos Disables");
}

void UserInput() {
  if(Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    //Outputs
    if (input.equals("k1") || input.equals("EnSer")) {
      EnableServos();
      delay(15);
    }
    else if (input.equals("k1-") || input.equals("DsSer")) {
      DisableServos();
      delay(15);
    }
    else if (input.equals("k2")) {
      digitalWrite(ArduinoReady, HIGH);
      Serial.println("Output D45 HIGH");
      delay(2000);
      digitalWrite(ArduinoReady, LOW);
      Serial.println("Output D45 LOW");
    }
    else if (input.equals("k3")) {
      digitalWrite(SpareOP1, HIGH);
      Serial.println("Output D46 HIGH");
    }
    else if (input.equals("k3-")) {
      digitalWrite(SpareOP1, LOW);
      Serial.println("Output D46 LOW");
    }
    else if (input.equals("k4")) {
      digitalWrite(SpareOP2, HIGH);
      Serial.println("Output D47 HIGH");
      delay(200);
      digitalWrite(SpareOP2, LOW);
      Serial.println("Output D47 LOW");
    }
    else if (input.equals("s1")) {
      Serial.println("Stepper 1 Move");
      StepperOneMove(20,1,20);
      Serial.println("Stepper 1 Move Complete");
    }
    else if (input.equals("a5-")) {
      Serial.println("Jog Axis 5= -25");
      Axis5.setMaxSpeed(1000.0);//0000
      Axis5.setAcceleration(50000.0);
      Axis5.move(-25);
      while (Axis5.distanceToGo() != 0) {
        Axis5.run();
      }
      if (Axis5.distanceToGo() == 0) {
        Serial.println("Axis 5 Move Complete");
      }
    }
    else if (input.equals("a5")) {
      Serial.println("Jog Axis 5= 25");
      Axis5.setMaxSpeed(1000.0);//00
      Axis5.setAcceleration(50000.0);
      Axis5.move(25);
      while (Axis5.distanceToGo() != 0) {
        Axis5.run();
      }
      if (Axis5.distanceToGo() == 0) {
        Serial.println("Axis 5 Move Complete");
      }
    }
    else if (input.equals("a6-")) {
      Serial.println("Jog Axis 6= -102");
      Axis6.setMaxSpeed(10000.0);
      Axis6.setAcceleration(8000.0);
      
      Axis6.move(-102);
      while (Axis6.distanceToGo() != 0) {
        Axis6.run();
      }
      if (Axis6.distanceToGo() == 0) {
        Serial.println("Axis 6 Move Complete");
      }
    }
    else if (input.equals("a6")) {
      Serial.println("Jog Axis 6= 102");
      Axis6.setMaxSpeed(10000.0);
      Axis6.setAcceleration(100.0);
      Axis6.move(102);
      while (Axis6.distanceToGo() != 0) {
        Axis6.run();
      }
      if (Axis6.distanceToGo() == 0) {
        Serial.println("Axis 6 Move Complete");
      }
    }
    else if (input.equals("g0")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 0deg");
      Gripper.write(0);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g20")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 20deg");
      Gripper.write(20);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g30")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 30deg");
      Gripper.write(30);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g40")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 40deg");
      Gripper.write(40);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g50")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 50deg");
      Gripper.write(50);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g60")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 60deg");
      Gripper.write(60);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g70")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 70deg");
      Gripper.write(70);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g90")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 90deg");
      Gripper.write(90);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("g180")) {
      EnableServos();
      //Move
      Serial.println("Gripper= 180deg");
      Gripper.write(180);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("1e0")) {
      //EnableServos();
      //Move
      //Serial.println("Egg Flipper= 0deg");
      //EggFlipper.write(0);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("1e90")) {
      //EnableServos();
      //Move
      //Serial.println("Egg Flipper= 90deg");
      //EggFlipper.write(90);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("1e180")) {
      //EnableServos();
      //Move
      //Serial.println("Egg Flipper= 180deg");
      //EggFlipper.write(180);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("2e0")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 0deg");
      EggCracker.write(0);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("2e90")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 90deg");
      EggCracker.write(90);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("2e180")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 180deg");
      EggCracker.write(180);
      delay(5000);
      DisableServos();
    }
    else if (input.equals("2e220")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 220deg");
      EggCracker.write(220);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("2e250")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 250deg");
      EggCracker.write(250);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("2e270")) {
      EnableServos();
      //Move
      Serial.println("Egg Cracker= 270deg");
      EggCracker.write(270);
      delay(1500);
      DisableServos();
    }
    else if (input.equals("se-")) {
      Serial.println("Steppers Disabled");
      digitalWrite(enablePinB, HIGH);
      digitalWrite(enablePinC, HIGH);
      delay(15);
    }
    else if (input.equals("se")) {
      Serial.println("Steppers Enabled");
      digitalWrite(enablePinB, LOW);
      digitalWrite(enablePinC, LOW);
      delay(15);
    }
    else if (input.equals("home") || input.equals("h")) {
      Serial.println("Manipulator Homing..");
      FindHome();
      delay(15);
    }
    else if (input.equals("run") || input.equals("r")) {
      Serial.println("Program Running..");
      ProgRun();
      delay(15);
    }
    
    

    
    //Read Inputs
    Axis5LimitState = digitalRead(Axis5Limit);
    Axis6LimitState = digitalRead(Axis6Limit);
    A550ReadyState = digitalRead(A550Ready);
    SpareINPState = digitalRead(SpareINP);
  
    //Print Inputs
    Serial.print("Axis 5 LS: ");
    Serial.println(Axis5LimitState);
    Serial.print("Axis 6 LS: ");
    Serial.println(Axis6LimitState);
    Serial.print("A550 Ready Signal: ");
    Serial.println(A550ReadyState);
    Serial.print("Pin 4: ");
    Serial.println(SpareINPState);

  }
}

void loop() {
  UserInput();
}
