#include <Stepper.h>
#include <math.h>

const int stepsPerRevolution = 200;
int posXdata = 500;
int posYdata = 10;
String mode = "NORMAL";

Stepper StepperA = Stepper(stepsPerRevolution, 4,5,6,7);
Stepper StepperB = Stepper(stepsPerRevolution, 8,9,10,11);

float stringLenght = 630;//mm
float maxWidth = 740;//mm
float maxHeight = -480;//mm

int currentPosRaw[2] = {0,0};
//uppper left corner is [0,0]
int currentPos[2] = {500,10};

void move(int posY, int posX){
  int distancePenToStepper[2] = {
    sqrt(pow(posX,2)+pow(posY,2)),
    sqrt(pow(maxWidth - posX,2)+pow(posY,2))
  };

  //calculate how much to move
  int howMuchToMove[2] = {
    currentPosRaw[0] - distancePenToStepper[0],
    currentPosRaw[1] - distancePenToStepper[1]
  };
/*
  //calculate if the position is out of reach
  if (currentPosRaw[0] + howMuchToMove[0] > stringLenght){
    howMuchToMove[0] = 0;
  }
  if (currentPosRaw[1] + howMuchToMove[1] > stringLenght){
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
}

void moveRaw(int posX, int posY){
  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if(abs(currentPosRaw[0] - posX) < abs(currentPosRaw[1] - posY)){
    numOfSteps = abs(currentPosRaw[0] - posX);
  }
  else{
    numOfSteps = abs(currentPosRaw[1] - posY);
  }

  //var progres is the number of steps until completion
  for(int progres=numOfSteps;progres>0;progres--){
    StepperA.step((currentPosRaw[0]-posX)/progres);
    StepperB.step((currentPosRaw[1]-posY)/progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0]-posX)/progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1]-posY)/progres;
  }
}

//run this if the stop button is pressed
void stopA(){
  //currentPosRaw[0] = 0;
}
void stopB(){
  //currentPosRaw[1] = 0;
}

void setup() {
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(100);
  StepperB.setSpeed(100);

  attachInterrupt(digitalPinToInterrupt(2), stopA, RISING);
  attachInterrupt(digitalPinToInterrupt(3), stopB, RISING);
}

void loop() {
  if(Serial.available() > 0) {
    //mode can be RAW or NORMAL
    mode = (Serial.readStringUntil('\n'));
    posXdata = (Serial.readStringUntil('\n')).toInt();
    posYdata = (Serial.readStringUntil('\n')).toInt();
    Serial.print(currentPosRaw[0]);
    Serial.print(currentPosRaw[1]);
  }
  if (mode=="NORMAL"){
    move(posXdata,posYdata);
    //Serial.println("OK");
  }
  else if (mode=="RAW"){
    moveRaw(posXdata,posYdata);
    //Serial.println("OK");
  }
  else{
    //Serial.println("ERROR");
  }
}
