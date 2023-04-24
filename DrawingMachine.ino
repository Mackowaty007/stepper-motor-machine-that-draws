#include <Stepper.h>
#include <math.h>

// Define number of steps per revolution:
const int stepsPerRevolution = 200;

// Initialize the stepper library
Stepper StepperA = Stepper(stepsPerRevolution, 15,13,12,14);
Stepper StepperB = Stepper(stepsPerRevolution, 2,0,4,5);


int position[4][2] = {
  {400,500},
  {500,500},
  {500,400},
  {400,400}
};

float circle1Radious = 700;//mm
float circle2Radious = 700;//mm

float stringLenght = 630;//mm
//maxWidth is how far away are the mottors from each other
float maxWidth = 740;//mm
float maxHeight = -480;//mm

int currentPosRaw[2] = {0,0};
//uppper left corner is [0,0]
int currentPos[2] = {maxWidth/2,0};

void move(int posX, int posY){
  int distancePenToStepper[2] = {
    sqrt(pow(posX,2)+pow(posY,2)),
    sqrt(pow(maxWidth - posX,2)+pow(posY,2))
  };

  //calculate how much to move
  int howMuchToMove[2] = {
    currentPosRaw[0] - distancePenToStepper[0],
    currentPosRaw[1] - distancePenToStepper[1]
  };
  
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

void setup() {
  //serial setup
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(100);
  StepperB.setSpeed(100);
}

void loop() {
  for(int i=0;i<4;i++){
    move(position[i][0],position[i][1]);
    //Serial.println(position[i][0]);
    delay(2000);
  }
}
