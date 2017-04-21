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
double koff=0.26;
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
  
  motorRight.setSpeed(70);
  motorLeft.setSpeed(70);
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
    irsend.sendNEC(0x1, 3);
  }
  else{
    Serial.println("Stop");
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
    irsend.sendNEC(0x1, 3);
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
      irsend.sendNEC(0x1, 3);
      delay(400);
    }
    else{
      if(angle>60){
        corr(true, angle);
      }
      else{
        corr(false, angle);
      }
    }
  }
  /*
   * Корректировка угла сервопривода
   */
  Serial.println("Correct angle");
  
  
  if(angle==30){
    flgPlus=true;
  }
  if(angle==150){
    flgPlus=false;
  }
  
  if(flgPlus){
    angle+=30;
  }
  else{
    angle-=30;
  }
  
  Serial.println("Finish correction, run");
  irsend.sendNEC(0x1, 3);
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
 * Параметр: флаг, отвечающий за необходимость поворота вправо, инча влево; угол поворота сервопривода в данный момент для обеспечений поворота
 */
void corr(boolean turnRight, int angle){
  Serial.println("Correction!");
  servo.write(90);
  
  if(turnRight){
    Serial.println("Turn right");    
    
      motorRight.setSpeed(200);
      motorLeft.setSpeed(200);
      Serial.println("Turn");
      motorLeft.run(FORWARD);
      motorRight.run(BACKWARD);
      irsend.sendNEC(0x1, 3);
      delay(ceil((angle-90)/koff));
      motorLeft.run(RELEASE);
      motorRight.run(RELEASE);
    
  }
  else{
    Serial.println("Turn left");
    
      motorRight.setSpeed(200);
      motorLeft.setSpeed(200);
      Serial.println("Turn");
      motorLeft.run(BACKWARD);
      motorRight.run(FORWARD);
      irsend.sendNEC(0x1, 3);
      delay(ceil((90-angle)/koff));
      motorLeft.run(RELEASE);
      motorRight.run(RELEASE);
    
  }
}

