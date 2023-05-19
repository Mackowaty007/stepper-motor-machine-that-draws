#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 200;

volatile bool isStopAPressed = false;
volatile bool isStopBPressed = false;

Stepper StepperA = Stepper(stepsPerRevolution, 4,5,6,7);
Stepper StepperB = Stepper(stepsPerRevolution, 8,9,10,11);
//what are the machine dimensions?
float stringLength = 630;//mm
float maxWidth = 740;//mm
float maxHeight = 480;//mm
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

//position of the pencil
int currentPosRaw[2] = {stringLength,stringLength};//steps
//uppper left corner is [0,0]
float currentPos[2] = {maxWidth/2,maxHeight};//mm

//moves the pen to specified location (in mm)
void move(int X, int Y){
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
  /*
  //calculate if the position is out of reach
  if (currentPosRaw[0] + howMuchToMove[0] > stringLength){
    howMuchToMove[0] = 0;
  }
  if (currentPosRaw[1] + howMuchToMove[1] > stringLength){
    howMuchToMove[1] = 0;
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
    StepperA.step((currentPosRaw[0]-distancePenToStepper[0])/progres);
    StepperB.step((currentPosRaw[1]-distancePenToStepper[1])/progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0]-distancePenToStepper[0])/progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1]-distancePenToStepper[1])/progres;
  }
  currentPos[0] = X;
  currentPos[1] = Y;
}

void moveRaw(int posX, int posY){
  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if(abs(currentPosRaw[0] - posY) < abs(currentPosRaw[1] - posX)){
    numOfSteps = abs(currentPosRaw[0] - posY);
  }
  else{
    numOfSteps = abs(currentPosRaw[1] - posX);
  }

  //var progres is the number of steps until completion
  for(int progres=numOfSteps;progres>0;progres--){
    StepperA.step((currentPosRaw[0]-posY)/progres);
    StepperB.step((currentPosRaw[1]-posX)/progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0]-posY)/progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1]-posX)/progres;
  }
}

void homeAll(){
  //click the A button
  while(isStopAPressed==false){
    StepperA.step(-2);
  }
  isStopAPressed = false;
  

  //click the B button
  while(isStopBPressed==false){
    StepperB.step(2);
  }
  isStopBPressed = false;

  //set coordinates
  currentPos[0] = maxWidth/2;
  currentPos[1] = maxHeight;
}

void setup() {
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(100);
  StepperB.setSpeed(100);

  attachInterrupt(digitalPinToInterrupt(3), stopA, RISING);
  attachInterrupt(digitalPinToInterrupt(2), stopB, RISING);
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

void loop() {
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
  }
}
