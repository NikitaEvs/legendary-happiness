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

AF_DCMotor motorLeft(4, MOTOR12_64KHZ);
AF_DCMotor motorRight(3,MOTOR12_64KHZ);

Ultrasonic ultrasonic(12,13);

Servo servo;
IRrecv irrecv(11);

decode_results results;
int rByte = 0;
char command = '0';
/*
 * Данные, полученные экспериментальным путём
 */
double koff=0.26;
int distancePrep=20;
int distanceCritical=40;
int angle=30;

boolean flgPlus=true;
boolean flgIR=false;

void setup() {
  servo.attach(9);
  Serial.begin(9600);
  servo.write(90);
  irrecv.enableIRIn();
  delay(1000);
  while(!flgIR){
    motorRight.setSpeed(100);
    motorLeft.setSpeed(100);
    motorRight.run(FORWARD);
    motorLeft.run(FORWARD);
    input();
    if(results.value==0x1){
      flgIR=true;
    }
    delay(20);
  }
}

void loop() {
  
  servo.write(angle);
  /*
   * Проверка на наличие ИК сигнала перед агентом
   * Вызов функции коррекции при обнаружение ИК сигнала 
   */
  input();
  if(results.value==0x1){
    clean();
    if(angle!=90){
      if(angle>60){
          corr(true);
        }
        else{
          corr(false);
        }
    }
  }
  /*
   * Проверка дистанции перед агентом
   */
  motorRight.setSpeed(150);
  motorLeft.setSpeed(150);
  if(ultrasonic.Ranging(CM)< distancePrep){
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
  }
  else{
    motorRight.run(FORWARD);
    motorLeft.run(FORWARD);
  }
  /*
   * Корректировка угла сервопривода для достижения обзора в 180 градусов
   */
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
  
  delay(20);
} 
/*
 * Функция, возвращающая среднее значение показаний ультразвукового датчика
 * Это позволяет избавится от помех при работе датчика
 */
int distance(){
  int distances=ultrasonic.Ranging(CM);
  
  for(int i=0; i<6; i++){
    distances+=ultrasonic.Ranging(CM);
    distances/=2;
    delay(10);
  }
  Serial.println(distances); 
  return distances;
}
/*
 * Функция, реализующая корректировку агента, в зависимости от положение лидера
 * Параметр: флаг, отвечающий за необходимость поворота вправо, инча влево 
 */
void corr(boolean flgRight){
  servo.write(90);
  
  if(flgRight){
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
    clean();
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
    clean();
  }
}
/*
 * Функция, которая определяет ИК сигнал, если он существует
 */
void input(){
  
  if(rByte = Serial.available()!= 0 || irrecv.decode(&results)){
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
  
}
/*
 * Функция освобождает переменную, в которой хранятся результаты считывания ИК сигнала
 */
void clean(){
  results.value=0;
}
