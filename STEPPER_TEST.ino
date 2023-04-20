#include <Stepper.h>
#include <math.h>

// Define number of steps per revolution:
const int stepsPerRevolution = 200;

// Initialize the stepper library
Stepper StepperA = Stepper(stepsPerRevolution, 8, 9, 10, 11);
Stepper StepperB = Stepper(stepsPerRevolution, 4, 5,  6,  7);

int currentPos[2] = {0,0};
int position[2] = {200,150};
int position2[2] = {0,0};


void move(int pos[2]){
  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if(abs(currentPos[0] - pos[0]) < abs(currentPos[1] - pos[1])){
    numOfSteps = abs(currentPos[0] - pos[0]);
  }
  else{
    numOfSteps = abs(currentPos[1] - pos[1]);
  }

  //Serial.println(numOfSteps);

  //var progres is the number of steps until completion
  for(int progres=numOfSteps;progres>0;progres--){
    StepperA.step((currentPos[0]-pos[0])/progres);
    StepperB.step((currentPos[1]-pos[1])/progres);
    currentPos[0] = currentPos[0] - (currentPos[0]-pos[0])/progres;
    currentPos[1] = currentPos[1] - (currentPos[1]-pos[1])/progres;

  }

}

void setup() {
  //serial setup
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(100);
  StepperB.setSpeed(100);
}

void loop() {
  move(position);
  delay(2000);
  move(position2);
  delay(2000);
}