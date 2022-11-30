
#include <Servo.h>  //servo library
Servo myservo;    // create servo object to control servo
 
int Echo = A4;    // attach Arduino A4 pin to Echo of HC-SR04
int Trig = A5;    // attach Arduino A4 pin to Echo of HC-SR04

// Line Tracking IO define
#define LT_R !digitalRead(10) // read & negate right light sensor
#define LT_M !digitalRead(4)  // read & negate middle light sensor
#define LT_L !digitalRead(2)  // read & negate left light sensor

int rightDistance = 0, leftDistance = 0, middleDistance = 0;
 
// Arduino control pins for servomotors
#define ENA 5 // analog pins for controlling the robot speed
#define ENB 6 // on turns left and right

// attach Arduino pins to L298 motor control 4 servo motors
#define IN1 7   // Attach Arduino pin 7 to motor control pin IN1  
#define IN2 8   // Attach Arduino pin 8 to motor control pin IN2
#define IN3 9   // Attach Arduino pin 9 to motor control pin IN3
#define IN4 11  // Attach Arduino pin 11 to motor control pin IN4

int LED = 13;     //specify the LED pin to pin 13
 
unsigned char carSpeed = 150; // car speed on turn left and right

bool state = LOW;

// var to store the message from the phone app, through Bluetooth
char getstr;  

// function to move the robot forward 
void forward(){ 
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  
  digitalWrite(IN1,HIGH); 
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  Serial.println("Forward");  // write on the IDE serial monitor
}

// function to move the robot backwards  
void back(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  Serial.println("Back");
}

// function to turn the robot to the left  
void left(){
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH); 
  Serial.println("Left");
}
 
// function to turn the robot to the right  
void right(){  
  analogWrite(ENA,carSpeed);
  analogWrite(ENB,carSpeed);
  
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  
  digitalWrite(LED, HIGH);  

  Serial.println("Right");
}

// function to stop the robot     
void stop(){
  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
  
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  
  digitalWrite(LED, LOW); 
  
  Serial.println("Stop!");
}

// function to change the robot state var. to flash the LED   
void stateChange(){
  state = !state;
  digitalWrite(LED, state);
  Serial.println("Light");  
  myservo.detach();
}
 
// function for Ultrasonic distance measurement 
int Distance_test() {  
  digitalWrite(Trig, LOW);     //starts trig pin low 
  delayMicroseconds(2);   //waits for hardware to respond
  digitalWrite(Trig, HIGH);   // trig pin sends sound wave
  delayMicroseconds(20); 
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);// receives the sound wave
  Fdistance = Fdistance / 58;     // calc. dist. from detected object 
  return (int)Fdistance;
}  

// function to decide if follow an object forward, stop or move back  
void followObject(){

  middleDistance = Distance_test();

  // Displays the distance on the serial Monitor
  Serial.print ("Distance: ");
  Serial.print (middleDistance);
  Serial.println(" cm");
  
  if (middleDistance <= 45) {
// robot moves BACKWARD when the distance to an object is too close
    back();
    
  } else if (middleDistance <= 80) {
// robot will STOP when the distance to an object is relatively close
    stop();

  } else if (middleDistance <= 1000) {
// robot moves FORWARD when the dist. to an obj. is not very far,
    forward();
    
  } else {
// If robot finds another distance which is not in previuos cases it // stops and informs: OUT OF RANGE
    stop();    
  }
// Pauses the program for the amount of time (in milliseconds)  
  delay(10);
  
  Serial.println("Robotic Behavior!");

}  

// *** used this Penguin robot option without the foam chassis
// function for obstacle avoidance. 
// Steps: 1-Stop the car.
// 2- Measure the dist. to the nearest obj in multiple directions.
// 3- Turn the car in the direction it finds the longest distance.
// 4- Start the car, then repeat when you encounter another object.
// myservo.write()- set the angle of the shaft; with 0° being full-
// speed in one direction, 180° being full speed in the other, and a   // value near 90° being no movement
void avoidObstacle(){
    myservo.write(90);  //set servo position according to scaled value
    delay(500); 
    middleDistance = Distance_test(); // find distance
 
    if(middleDistance <= 20) {  // very close distance   
      stop();       // stop robot
      delay(500);           // delay 500 milisec.               
      myservo.write(10);  // set the angle of the shaft 10°
      delay(1000);      
      rightDistance = Distance_test();  // calc right distance
      
      delay(500);
      myservo.write(90);     // set the angle of the shaft 90°         
      delay(1000);                                                  
      myservo.write(180);     // set the angle of the shaft 180°         
      delay(1000); 
      leftDistance = Distance_test(); // calc left dist
      
      delay(500);
      myservo.write(90);              
      delay(1000);
      if(rightDistance > leftDistance) { // more space on the right
        right();  // move right
        delay(360);
      }
// more space on the left
      else if(rightDistance < leftDistance) { 
        left(); // move leftDistance
        delay(360);
      }
      else if((rightDistance <= 20) || (leftDistance <= 20)) {
        back(); // short distances, then move back
        delay(180);
      }
      else {
        forward();
      }
    }  
    else {
        forward();
    }                       
    Serial.println("Avoid Obstacles");
}

// function to follow a dark line on a light background and use 
// optical sensors to determine its position.
void followLine(){

  if(LT_M){  // if middle sensor sense dark move forward
    forward();
  }
  else if(LT_R) {   // if dark on the right move right
    right();
    while(LT_R);    // while dark on the right move right                         
  }   
  else if(LT_L) {  // if dark on the left move left
    left();
    while(LT_L);  
  }
   
  Serial.println("Follow a line");
}

// initial input or output pin setup
void setup() { 
  Serial.begin(9600);

  pinMode (Trig, OUTPUT) ; // sonar digital pin mode for trig
  pinMode (Echo, INPUT);   // sonar digital pin mode for echo

  pinMode(LED, OUTPUT);  // Arduino LED pin
  pinMode(IN1, OUTPUT);  // control the motor controller for servo motor 1
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);  // control the motor controller for servo motor 4
  
  pinMode(LT_R,INPUT);  // sensing light from the right sensor
  pinMode(LT_M,INPUT);  // sensing light from the middle sensor
  pinMode(LT_L,INPUT);  // sensing light from the left sensor
  
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stop();  // initially keep the robot stopped
}

// looping Arduino function
void loop() { 
  // check the message from the phone remote through robot Bluetooth
  if (Serial.available() > 0 )
  {
     getstr = Serial.read();  
  }
  
  switch(getstr){
    
    case 'a': followLine(); // if msg ‘a’ follow the dark line         
      break;
      
    case 'c': followObject();  // if msg ‘c’ follow an object or move back - Robotic behavior 
      break;
      
    case 'd': stateChange();
      break;
      
    case 'o': avoidObstacle();  // *** used this Penguin robot option without the foam chassis
      break;

    case 'f': forward();  // if msg ‘f’ move forward         
      break;
      
    case 'b': back();   
      break;
      
    case 'l': left();   
      break;
      
    case 'r': right(); 
      break;
      
    case 's': stop();   
      break;
          
     default:  break;
  }
}
