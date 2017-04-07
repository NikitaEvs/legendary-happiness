/*
 * Программа для реализации алгоритма движения МАС
 * Программа для ведомого агента 
 */
 
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <Servo.h>
#include <Ultrasonic.h>
#include <AFMotor.h>

AF_DCMotor motorLeft(3, MOTOR12_64KHZ);
AF_DCMotor motorRight(4,MOTOR12_64KHZ);

Ultrasonic ultrasonic(12,13);
Servo servo;
IRsend irsend;
/*
 * Данные, полученные экспериментальным путём
 */
int distancePrep=20;
int distanceCritical=40;
int distanceWow=5;

int angle=30;

boolean flgPlus=true;
boolean flgWow=false;

void setup() {
  motorRight.setSpeed(200);
  motorLeft.setSpeed(200);
  servo.attach(9);
  Serial.begin(9600);
  servo.write(90);
  delay(1000);
}

/*
 * В коде периодически происходит вывод информации в монитор порта, что позволяет легко заниматься отладкой программы
 */
void loop() {
  motorRight.setSpeed(150);
  motorLeft.setSpeed(150);
  servo.write(angle);
  /*
   * Выполнения движения вперед,если функция checkPrep() имеет значение false 
   * Иначе корректировка движения
   */
  if(!checkPrep()){
    Serial.println("Run");
    motorRight.setSpeed(150);
    motorLeft.setSpeed(150);
    motorRight.run(FORWARD);
    motorLeft.run(FORWARD);
    irsend.sendSony(0xa90, 3);
  }
  else{
    Serial.println("Stop");
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
    irsend.sendSony(0xa90, 3);
    delay(100);
    /*
     * При значении флага true движение назад
     * В противном случае корректировка движения функцией corr
     */
    if(flgWow){
      motorRight.setSpeed(200);
      motorLeft.setSpeed(200);
      motorRight.run(BACKWARD);
      motorLeft.run(BACKWARD);
      irsend.sendSony(0xa90, 3);
      delay(400);
    }
    else{
      if(angle>60){
        corr(true);
      }
      else{
        corr(false);
      }
    }
  }
  /*
   * Корректировка угла сервопривода
   */
  Serial.println("Correct angle");
  
  switch (angle){
    case 180: flgPlus=true;
    case 0: flgPlus=false;
  }
  
  if(flgPlus){
    angle+=30;
  }
  else{
    angle-=30;
  }
  
  Serial.println("Finish correction, run");
  irsend.sendSony(0xa90, 3);
  delay(100);
}
/*
 * Функция, которая определяет, есть ли перед агентом препятствие
 * Если препятствие находится на критическом расстоянии, то присвоение flgWow значение true 
 */
boolean checkPrep(){
  Serial.println("CheckPrep");
  Serial.println(ultrasonic.Ranging(CM));
  if(ultrasonic.Ranging(CM)<distanceWow){
    flgWow=true;
  }
  else{
    flgWow=false;
  }
  if(ultrasonic.Ranging(CM)>distancePrep){
    return false;
  }
  else{
    return true;
  }
}
/*
 *Функция для поддержания определенной дистанции
 *В данной версии кода не используется 
 */
boolean checkDistance(){
  if(ultrasonic.Ranging(CM)>distanceCritical){
    return false;
  }
  else{
    return true;
  }
}
/*
 * Функция, реализующая корректировку агента, в зависимости от положение лидера
 * Параметр: флаг, отвечающий за необходимость поворота вправо, инча влево 
 */
void corr(boolean turnRight){
  Serial.println("Correction!");
  servo.write(90);
  
  if(turnRight){
    Serial.println("Turn right");    
    
    while(ultrasonic.Ranging(CM)<distancePrep){
      motorRight.setSpeed(200);
      motorLeft.setSpeed(200);
      Serial.println("Turn");
      motorLeft.run(FORWARD);
      motorRight.run(BACKWARD);
      irsend.sendSony(0xa90, 3);
      delay(20);
    }
  }
  else{
    Serial.println("Turn left");
    
    while(ultrasonic.Ranging(CM)<distancePrep){
      motorRight.setSpeed(200);
      motorLeft.setSpeed(200);
      Serial.println("Turn");
      motorLeft.run(BACKWARD);
      motorRight.run(FORWARD);
      irsend.sendSony(0xa90, 3);
      delay(20);
    }
  }
}

