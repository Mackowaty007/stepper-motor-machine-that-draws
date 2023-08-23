#include <Stepper.h>
#include <math.h>
#include <Servo.h>

//#define ENABLE_JOYSTICK     //enables the use of a joystick
#define ENABLE_SERIAL       //enables serial communication with a PC
//#define ENABLE_DEBUG        //prints out position over serial

#define STEPPER_SPEED_NORMAL 40
#define STEPPER_SPEED_RAPID 100

Servo myservo;
const int servoMinAngle = 40;
const int servoMaxAngle = 90;
bool lastServoAngleMax = true;//this variable is used to make the servo move gradualy (when it changes you know to change from min to max or vice versa)

const int stepsPerRevolution = 200;

//what are the machine dimensions?
//lenght of the string
const float stringLength = 600;  //mm
//how wide the machine is (the distance between 2 stepper motors)
const float maxWidth = 736;  //mm
//how low can the rope go
const float maxHeight = sqrt(pow(stringLength, 2) - pow(maxWidth / 2, 2));

volatile bool isStopAPressed = false;
volatile bool isStopBPressed = false;

//define pins
Stepper StepperA = Stepper(stepsPerRevolution, 4, 5, 6, 7);
Stepper StepperB = Stepper(stepsPerRevolution, 13, 10, 9, 8);  //had to swap 11 and 13 pin because i needed that pwm pin for other shit

int servo_pin = 11;

#ifdef ENABLE_JOYSTICK
const int XjoystickPin = A1; 
const int YjoystickPin = A0;
const int joystickButton = 12;
// parameters for reading the joystick:
const float deadzone = 0.04;
float Xjoystick = 0.0;
float Yjoystick = 0.0;
#endif

#ifdef ENABLE_SERIAL
String string_line;
String string_parameter;
//data for serial
float posXdata = maxWidth / 2;  //mm
float posYdata = maxHeight;     //mm
int posZdata = 0;
String mode = "NORMAL";
#endif

//TODO: implement this into move()
float lerp(float a,float b,float t){
    return a + t * (b - a);
}

//200 steps ~ 31mm
//1 step = 0.155mm
//1 mm = 6.45161290323 steps
//converts step number to milimiters
float stepsToMm(int steps) {
  return steps * 0.155;
}
int mmToSteps(float mm) {
  return mm * 6.45;
}

//if the button is detected as pressed do this
void FuckGoBack(char stepperID) {
  if (stepperID == 'A') StepperA.step(30+264);//the 264 is for callibration reasons (so that the strings are both equal)
  if (stepperID == 'B') StepperB.step(30);
}
//how much the strings are unwind
int currentPosRaw[2] = { mmToSteps(stringLength), mmToSteps(stringLength) };  //steps
//uppper left corner is [0,0]
float currentPos[2] = { maxWidth / 2, maxHeight };  //mm

//moves the pen to specified location (in mm)
void move(float X, float Y) {
  //check if out of bounds
  if (X < 0) X = 0;
  if (X > maxWidth) X = maxWidth;
  if (Y < 0) Y = 0;
  if (Y > maxHeight) Y = maxHeight;

  //check if one of the lines is too short to reach the point
  if (sqrt(pow(X, 2) + pow(Y, 2)) > stringLength){
    return;
  }
  if (sqrt(pow(maxWidth - X, 2) + pow(Y, 2)) > stringLength){
    return;
  }

  //set current coordinates
  currentPos[0] = X;
  currentPos[1] = Y;

  //convert from milimiters to the number of steps
  int posX = mmToSteps(X);
  int posY = mmToSteps(Y);

  int distancePenToStepper[2] = { //in steps
    sqrt(pow(posX, 2) + pow(posY, 2)),
    sqrt(pow(mmToSteps(maxWidth) - posX, 2) + pow(posY, 2))
  };

  //calculate how much to move
  int howMuchToMove[2] = { //in steps
    currentPosRaw[0] - distancePenToStepper[0],
    currentPosRaw[1] - distancePenToStepper[1]
  };
  /*
  //calculate if the position is out of reach
  if (currentPosRaw[0] + howMuchToMove[0] > mmToSteps(stringLength)){
  }
  if (currentPosRaw[1] + howMuchToMove[1] > mmToSteps(stringLength)){
  }*/

  int numOfSteps = 100;

  //set the number of steps based on which distance is smaller
  if (abs(howMuchToMove[0]) < abs(howMuchToMove[1])) {
    numOfSteps = abs(howMuchToMove[0]);
  } else {
    numOfSteps = abs(howMuchToMove[1]);
  }

  //var progres is the number of steps until completion
  //*
  for (int progres = numOfSteps; progres > 0; progres--) {
    //check if the stop button has been pressed
    if (isStopAPressed == true) {
      FuckGoBack('A');
      isStopAPressed = false;
    }
    if (isStopBPressed == true) {
      FuckGoBack('B');
      isStopBPressed = false;
    }
    StepperA.step((currentPosRaw[0] - distancePenToStepper[0]) / progres);
    StepperB.step((currentPosRaw[1] - distancePenToStepper[1]) / progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0] - distancePenToStepper[0]) / progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1] - distancePenToStepper[1]) / progres;
  }//*/
  /*
  for (int progres = numOfSteps; progres > 0; progres--) {
    //check if the stop button has been pressed
    if (isStopAPressed == true) {
      FuckGoBack('A');
      isStopAPressed = false;
    }
    if (isStopBPressed == true) {
      FuckGoBack('B');
      isStopBPressed = false;
    }
    StepperA.step((currentPosRaw[0] - distancePenToStepper[0]) / progres);
    StepperB.step((currentPosRaw[1] - distancePenToStepper[1]) / progres);
    currentPosRaw[0] = currentPosRaw[0] - (currentPosRaw[0] - distancePenToStepper[0]) / progres;
    currentPosRaw[1] = currentPosRaw[1] - (currentPosRaw[1] - distancePenToStepper[1]) / progres;
  }*/
}

void homeAll() {
  //click the A button
  while (isStopAPressed == false) {
    StepperA.step(-2);
  }
  FuckGoBack('A');
  isStopAPressed = false;

  //click the B button
  while (isStopBPressed == false) {
    StepperB.step(-2);
  }
  FuckGoBack('B');
  isStopBPressed = false;

  //set coordinates
  currentPos[0] = maxWidth / 2;
  currentPos[1] = maxHeight;
}

//run this if the stop button is pressed
void stopA() {
  isStopAPressed = true;
  currentPosRaw[0] = mmToSteps(stringLength);
}
void stopB() {
  isStopBPressed = true;
  currentPosRaw[1] = mmToSteps(stringLength);
}

void setup() {
  Serial.begin(9600);

  // Set the motor speed (RPMs):
  StepperA.setSpeed(STEPPER_SPEED_RAPID);
  StepperB.setSpeed(STEPPER_SPEED_RAPID);

  attachInterrupt(digitalPinToInterrupt(3), stopA, RISING);
  attachInterrupt(digitalPinToInterrupt(2), stopB, RISING);
  #ifdef ENABLE_JOYSTICK
  pinMode(joystickButton, INPUT);
  #endif

  myservo.attach(servo_pin);
}

void loop() {
  #ifdef ENABLE_JOYSTICK
  //joystick handling
  float Xjoystick = -(analogRead(XjoystickPin) * (2.0 / 1023.0) - 1.0);
  float Yjoystick = (analogRead(YjoystickPin) * (2.0 / 1023.0) - 1.0);

  //apply Deadzone
  if (Xjoystick < deadzone && Xjoystick > -deadzone) Xjoystick = 0;
  if (Yjoystick < deadzone && Yjoystick > -deadzone) Yjoystick = 0;

  //check joystick button
  //if (digitalRead(joystickButton) == HIGH) homeAll();
  if (digitalRead(joystickButton) == LOW) myservo.write(servoMaxAngle);
  else { myservo.write(servoMinAngle); }

  //move the pen using the joystick
  move(currentPos[0] + Xjoystick * 2.0, currentPos[1] + Yjoystick * 2.0);
  #endif

  #ifdef ENABLE_DEBUG
  //debug info
  /*
  Serial.print("X: ");
  Serial.print(currentPosRaw[0]);
  Serial.print(" steps Y: ");
  Serial.print(currentPosRaw[1]);
  Serial.print(" steps   ");
*/
  Serial.print("X: ");
  Serial.print(currentPos[0]);
  Serial.print("mm Y: ");
  Serial.print(currentPos[1]);
  Serial.println("mm");
  #endif

  #ifdef ENABLE_SERIAL
  bool badSerialInput = false;

  //serial communication with a PC
  if(Serial.available() > 0) {
    //mode can be NORMAL, RAPID or HOME
    mode = (Serial.readStringUntil('\n'));

    if (mode == "NORMAL"){
      StepperA.setSpeed(STEPPER_SPEED_NORMAL);
      StepperB.setSpeed(STEPPER_SPEED_NORMAL);
    }
    else if (mode == "RAPID"){
      StepperA.setSpeed(STEPPER_SPEED_RAPID);
      StepperB.setSpeed(STEPPER_SPEED_RAPID);
    }
    else if (mode == "HOME") {
      StepperA.setSpeed(STEPPER_SPEED_RAPID);
      StepperB.setSpeed(STEPPER_SPEED_RAPID);
      homeAll();
      mode = "NORMAL";
    }
    else{
      //integrity check
      badSerialInput = true;
    }
    
    posXdata = (Serial.readStringUntil('\n')).toFloat();
    posYdata = (Serial.readStringUntil('\n')).toFloat();
    posZdata = (Serial.readStringUntil('\n')).toInt();
    //integrity check
    if (posZdata != 1 && posZdata != 0){
      badSerialInput = true;
    }

    if(badSerialInput == true){
      Serial.print("ERROR");
      badSerialInput = false;
    }
    else Serial.println("OK");
  }
  //move the servo
  if (posZdata == 1 && lastServoAngleMax == false){
    //this loop makes the servo move gradualy
    //myservo.write(servoMaxAngle);
    
    for (int servoPos = servoMinAngle; servoPos <= servoMaxAngle; servoPos += 1) {
      // in steps of 1 degree
      myservo.write(servoPos);
      delay(10);
    }
    lastServoAngleMax = true;
  }
  else if (posZdata == 0 && lastServoAngleMax == true){ 
    //this loop makes the servo move gradualy
    myservo.write(servoMinAngle);
    //*
    for (int servoPos = servoMaxAngle; servoPos >= servoMinAngle; servoPos -= 1) {
      // in steps of 1 degree
      myservo.write(servoPos);
      delay(10);
    }//*/
    lastServoAngleMax = false;
  }
  //move the head
  move(posXdata,posYdata);
  #endif
}
