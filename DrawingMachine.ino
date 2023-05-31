#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 200;

volatile bool isStopAPressed = false;
volatile bool isStopBPressed = false;

//define pins
const int XjoystickPin = A1;
const int YjoystickPin = A0;
const int joystickButton = 12;
Stepper StepperA = Stepper(stepsPerRevolution, 4,5,6,7);
Stepper StepperB = Stepper(stepsPerRevolution, 11,10,9,8);

// parameters for reading the joystick:
const float deadzone  = 0.1;
float Xjoystick = 0.0;
float Yjoystick = 0.0;

//what are the machine dimensions?
//lenght of the string
const float stringLength = 630;//mm
//how wide the machine is (the distance between 2 stepper motors)
const float maxWidth = 740;//mm
//how low can the rope go
//const float maxHeight = 480;//mm
const float maxHeight = sqrt(pow(stringLength,2) - pow(maxWidth/2,2));

//data for serial
int posXdata = maxWidth/2;//mm
int posYdata = maxHeight;//mm
String mode = "NORMAL";

//200 steps ~ 31mm
//1 step = 0.155mm
//1 mm = 6.45161290323 steps
//converts step number to milimiters
float stepsToMm(int steps){
  return steps * 0.155;
}
int mmToSteps(float mm){
  return mm * 6.45;
}

//if the button is detected as pressed do this
void FuckGoBack(char stepperID){
  if (stepperID == 'A') StepperA.step(100);
  if (stepperID == 'B') StepperB.step(100);
}
//position of the pencil
int currentPosRaw[2] = {mmToSteps(stringLength),mmToSteps(stringLength)};//steps
//uppper left corner is [0,0]
float currentPos[2] = {maxWidth/2,maxHeight};//mm
/*
//move A and B mottors to a pos
void moveRaw(int posA, int posB){
  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if(abs(posA) < abs(posB)){
    numOfSteps = abs(posA);
  }
  else{
    numOfSteps = abs(posB);
  }
  Serial.println(numOfSteps);

  //var progres is the number of steps until completion
  for(int progres=numOfSteps;progres>0;progres--){
    StepperA.step((currentPosRaw[0]-posA)/progres);
    StepperB.step((currentPosRaw[1]-posB)/progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0]-posA)/progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1]-posB)/progres;
  }
}*/

//moves the pen to specified location (in mm)
void move(int X, int Y){
  //check if out of bounds
  if (X < 0) X = 0;
  if (X > maxWidth) X = maxWidth;
  if (Y < 0) Y = 0;
  if (Y > maxHeight) Y = maxHeight;

  //set current coordinates
  currentPos[0] = X;
  currentPos[1] = Y;

  //convert from milimiters to the number of steps
  int posX = mmToSteps(X);
  int posY = mmToSteps(Y);

  int distancePenToStepper[2] = {//in steps
    sqrt(pow(posX,2)+pow(posY,2)),
    sqrt(pow(mmToSteps(maxWidth) - posX,2)+pow(posY,2))
  };

  //calculate how much to move
  int howMuchToMove[2] = {//in steps
    currentPosRaw[0] - distancePenToStepper[0],
    currentPosRaw[1] - distancePenToStepper[1]
  };
  /*
  //calculate if the position is out of reach
  if (currentPosRaw[0] + howMuchToMove[0] > mmToSteps(stringLength)){
    //Serial.println("outOfReachA");
  }
  if (currentPosRaw[1] + howMuchToMove[1] > mmToSteps(stringLength)){
    //Serial.println("outOfReachA");
  }*/
  
  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if(abs(howMuchToMove[0]) < abs(howMuchToMove[1])){
    numOfSteps = abs(howMuchToMove[0]);
  }
  else{
    numOfSteps = abs(howMuchToMove[1]);
  }

  //var progres is the number of steps until completion
  for(int progres=numOfSteps;progres>0;progres--){
    //check if the stop button has been pressed
    if(isStopAPressed == true){
      FuckGoBack('A');
      isStopAPressed = false;
    }
    if(isStopBPressed == true){
      FuckGoBack('B');
      isStopBPressed = false;
    }
    StepperA.step((currentPosRaw[0]-distancePenToStepper[0])/progres);
    StepperB.step((currentPosRaw[1]-distancePenToStepper[1])/progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0]-distancePenToStepper[0])/progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1]-distancePenToStepper[1])/progres;
  }
}
/*
//moves one stepper mottor then the other
void moveOneAtATime(int X, int Y){
  //check if out of bounds
  if (X < 0) X = 0;
  if (X > maxWidth) X = maxWidth;
  if (Y < 0) Y = 0;
  if (Y > maxHeight) Y = maxHeight;

  //set current coordinates
  currentPos[0] = X;
  currentPos[1] = Y;

  //convert from milimiters to the number of steps
  int posX = mmToSteps(X);
  int posY = mmToSteps(Y);

  int distancePenToStepper[2] = {//in steps
    sqrt(pow(posY,2)+pow(posX,2)),
    sqrt(pow(mmToSteps(maxWidth) - posY,2)+pow(posX,2))
  };

  //calculate how much to move
  int howMuchToMove[2] = {//in steps
    currentPosRaw[0] - distancePenToStepper[0],
    currentPosRaw[1] - distancePenToStepper[1]
  };

  StepperA.step(howMuchToMove[0]);
  StepperB.step(howMuchToMove[1]);
  currentPosRaw[0] = currentPosRaw[0] - howMuchToMove[0];
  currentPosRaw[1] = currentPosRaw[1] - howMuchToMove[1];
}*/

void homeAll(){
  //click the A button
  while(isStopAPressed==false){
    StepperA.step(-2);
  }
  FuckGoBack('A');
  isStopAPressed = false;

  //click the B button
  while(isStopBPressed==false){
    StepperB.step(-2);
  }
  FuckGoBack('B');
  isStopBPressed = false;

  //set coordinates
  currentPos[0] = maxWidth/2;
  currentPos[1] = maxHeight;
}

//run this if the stop button is pressed
void stopA(){
  isStopAPressed = true;
  currentPosRaw[0] = mmToSteps(stringLength);
}
void stopB(){
  isStopBPressed = true;
  currentPosRaw[1] = mmToSteps(stringLength);
}

void setup() {
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(100);
  StepperB.setSpeed(100);

  attachInterrupt(digitalPinToInterrupt(3), stopA, RISING);
  attachInterrupt(digitalPinToInterrupt(2), stopB, RISING);

  pinMode(joystickButton, INPUT);
}

void loop() {
//joystick handling
  float Xjoystick =-(analogRead(XjoystickPin) * (2.0 / 1023.0) - 1.0);
  float Yjoystick = (analogRead(YjoystickPin) * (2.0 / 1023.0) - 1.0);

  //apply Deadzone
  if (Xjoystick < deadzone && Xjoystick > -deadzone) Xjoystick = 0;
  if (Yjoystick < deadzone && Yjoystick > -deadzone) Yjoystick = 0;

  //check joystick button
  if (digitalRead(joystickButton) == HIGH) homeAll();

  // print out the value you read:
  /*Serial.print  ("X: ");
  Serial.print  (Xjoystick);
  Serial.print  (" Y: ");
  Serial.println(Yjoystick);*/

  //move the pen using the joystick
  move(currentPos[0]+Xjoystick*5,currentPos[1]+Yjoystick*5);
  //moveOneAtATime(0,0);
  //moveRaw(0,0);

  Serial.print("X: ");
  Serial.print(currentPosRaw[0]);
  Serial.print(" steps Y: ");
  Serial.print(currentPosRaw[1]);
  Serial.print(" steps   ");

  Serial.print("X: ");
  Serial.print(currentPos[0]);
  Serial.print("mm Y: ");
  Serial.print(currentPos[1]);
  Serial.println("mm");

//serial communication with a PC
/*
  if(Serial.available() > 0) {
    //mode can be RAW, NORMAL or HOME
    mode = (Serial.readStringUntil('\n'));
    posXdata = (Serial.readStringUntil('\n')).toInt();
    posYdata = (Serial.readStringUntil('\n')).toInt();
    Serial.print("X: ");
    Serial.print(currentPos[0]);
    Serial.print("mm Y: ");
    Serial.print(currentPos[1]);
    Serial.print("mm");
  }
  if (mode=="NORMAL"){
    move(posXdata,posYdata);
    //Serial.println("OK");
  }
  else if (mode=="RAW"){
    moveRaw(posXdata,posYdata);
    //Serial.println("OK");
  }
  else if (mode=="HOME"){
    homeAll();
  }
  else{
    //Serial.println("ERROR");
  }*/
}
