#include <Servo.h>

#include <AccelStepper.h>
#include <MultiStepper.h>
//#include <Servo.h>


int RunProgram1 = true;
int RunProgram2 = false;
int RunProgram3 = false;


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
const int RunDrillMotor = 44;
const int ArduinoReady = 45;
const int SpareOP1 = 46;
const int SpareOP2 = 47;


//Bolean
int RobotHomed = false;


// Define some steppers and the pins the will use
AccelStepper Axis5(AccelStepper::DRIVER, stepB, dirB); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

AccelStepper Axis6(AccelStepper::DRIVER, stepC, dirC); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

Servo Gripper;

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
  pinMode (A550Ready, INPUT);
  pinMode (SpareINP, INPUT);

  //OUTPUTS
  pinMode (RunDrillMotor, OUTPUT);
  pinMode (ArduinoReady, OUTPUT);
  pinMode (SpareOP1, OUTPUT);
  pinMode (SpareOP2, OUTPUT);

  //Set Outputs To High
  digitalWrite (RunDrillMotor, LOW);
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

void loop() {
  if(Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    //Outputs
    if (input.equals("k1")) {
      digitalWrite(RunDrillMotor, HIGH);
      Serial.println("Output D44 HIGH");
      delay(2000);
      digitalWrite(RunDrillMotor, LOW);
      Serial.println("Output D44 LOW");
    }
    else if (input.equals("k2")) {
      digitalWrite(ArduinoReady, HIGH);
      Serial.println("Output D45 HIGH");
      delay(200);
      digitalWrite(ArduinoReady, LOW);
      Serial.println("Output D45 LOW");
    }
    else if (input.equals("k3")) {
      digitalWrite(SpareOP1, HIGH);
      Serial.println("Output D46 HIGH");
      delay(2000);
      digitalWrite(SpareOP1, LOW);
      Serial.println("Output D46 LOW");
    }
    else if (input.equals("k4")) {
      digitalWrite(SpareOP2, HIGH);
      Serial.println("Output D47 HIGH");
      delay(2000);
      digitalWrite(SpareOP2, LOW);
      Serial.println("Output D47 LOW");
    }
    else if (input.equals("k1.5")) {
      digitalWrite(RunDrillMotor, HIGH);
      Serial.println("Output D44 HIGH");
      delay(5000);
      digitalWrite(RunDrillMotor, LOW);
      Serial.println("Output D44 LOW");
    }
    else if (input.equals("s1")) {
      Serial.println("Stepper 1 Move");
      StepperOneMove(20,1,20);
      Serial.println("Stepper 1 Move Complete"1);
    }
    else if (input.equals("a5-")) {
      Serial.println("Jog Axis 5= -10000");
      Axis5.move(-500);
      while (Axis5.distanceToGo() != 0) {
        Axis5.run();
      }
      if (Axis5.distanceToGo() == 0) {
        Serial.println("Axis 5 Move Complete");
      }
    }
    else if (input.equals("a5")) {
      Serial.println("Jog Axis 5= 10000");
      Axis5.move(500);
      while (Axis5.distanceToGo() != 0) {
        Axis5.run();
      }
      if (Axis5.distanceToGo() == 0) {
        Serial.println("Axis 5 Move Complete");
      }
    }
    else if (input.equals("a6-")) {
      Serial.println("Jog Axis 6= -10000");
      Axis6.move(-300);
      while (Axis6.distanceToGo() != 0) {
        Axis6.run();
      }
      if (Axis6.distanceToGo() == 0) {
        Serial.println("Axis 6 Move Complete");
      }
    }
    else if (input.equals("a6")) {
      Serial.println("Jog Axis 6= 10000");
      Axis6.move(300);
      while (Axis6.distanceToGo() != 0) {
        Axis6.run();
      }
      if (Axis6.distanceToGo() == 0) {
        Serial.println("Axis 6 Move Complete");
      }
    }
    else if (input.equals("g0")) {
      Serial.println("Gripper= 0deg");
      Gripper.write(0);
      delay(15);
    }
    else if (input.equals("g90")) {
      Serial.println("Gripper= 90deg");
      Gripper.write(90);
      delay(15);
    }
    else if (input.equals("g180")) {
      Serial.println("Gripper= 180deg");
      Gripper.write(180);
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
