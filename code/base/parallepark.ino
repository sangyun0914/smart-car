#include <Servo.h>
Servo servo;
const int M1_PWM = 5;   // DC모터1 PWM 핀 
const int M1_DIR1 = 7;   // DC모터1 DIR1 핀
const int M1_DIR2 = 8;   // DC모터 1 DIR2 핀

const int M2_PWM = 6;   // DC모터2 PWM 핀 
const int M2_DIR1 = 11;   // DC모터2 DIR1 핀 
const int M2_DIR2 = 12;   // DC모터2 DIR2 핀

const int SERVO_PIN = 9;

#define FC_TRIG 13   // 전방 초음파 센서 TRIG 핀
#define FC_ECHO 10  // 전방 초음파 센서 ECHO 핀
#define L_TRIG  A2  // 좌측 초음파 센서 TRIG 핀
#define L_ECHO  A1  // 좌측 초음파 센서 ECHO 핀
#define R_TRIG  2   // 우측 초음파 센서 TRIG 핀
#define R_ECHO  A5  // 우측 초음파 센서 ECHO 핀

float f_center;
float left;
float right;

void setup() {
  // put your setup code here, to run once:
    pinMode(M1_PWM, OUTPUT);
    pinMode(M1_DIR1, OUTPUT);
    pinMode(M1_DIR2, OUTPUT);

    pinMode(M2_PWM, OUTPUT);
    pinMode(M2_DIR1, OUTPUT);
    pinMode(M2_DIR2, OUTPUT);

    servo.attach(SERVO_PIN);

    pinMode(FC_TRIG, OUTPUT);
    pinMode(FC_ECHO, INPUT);
   
    pinMode(L_TRIG, OUTPUT);
    pinMode(L_ECHO, INPUT);
  
    pinMode(R_TRIG, OUTPUT);
    pinMode(R_ECHO, INPUT);
  
    Serial.begin(9600);
}

float GetDistance(int trig, int echo)
{ 
  // Range: 3cm ~ 75cm
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(trig, LOW);
  
  unsigned long duration = pulseIn(echo, HIGH, 5000);
  if(duration == 0)
      return 800;
  else
      return duration * 0.17;
}


void PararlleParking() {
  // put your main code here, to run repeatedly:
    //전진 till 양면에 벽나올때까지
    for (i) {
      digitalWrite(M1_DIR1, 1);
      digitalWrite(M1_DIR2, 0);
      analogWrite(M1_PWM, 100);
      digitalWrite(M2_DIR1, 1);
      digitalWrite(M2_DIR2, 0);
      analogWrite(M2_PWM, 100);
      delay(25);
      left = GetDistance(L_TRIG, L_ECHO);
      right = GetDistance(R_TRIG, R_ECHO);
      if (left < 10 && right < 10) {
        break;
      }
    }

    //후진 till 우측 벽 없어질때까지 (주차공간 접근)
    for (j) {
      digitalWrite(M1_DIR1, 0);
      digitalWrite(M1_DIR2, 1);
      analogWrite(M1_PWM, 100);
      digitalWrite(M2_DIR1, 0);
      digitalWrite(M2_DIR2, 1);
      analogWrite(M2_PWM, 100);
      delay(25);
      left = GetDistance(L_TRIG, L_ECHO);
      right = GetDistance(R_TRIG, R_ECHO);
      if (left < 10 && right > 10) {
        break;
    }


    //주차 - 후진 우회전 45도
    servo.write(145); // 90 + 55

    digitalWrite(M1_DIR2, LOW);
    digitalWrite(M1_DIR1, HIGH);
    analogWrite(M1_PWM, 255);   
    digitalWrite(M2_DIR1, 0);
    digitalWrite(M2_DIR2, 1);
    analogWrite(M2_PWM, 255); 
    delay(600);

    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);
    delay(300);

    //주차 -후진
    servo.write(90);
    delay(300);
    
    digitalWrite(M1_DIR1, 1);
    digitalWrite(M1_DIR2, 0);
    analogWrite(M1_PWM, 255);
    digitalWrite(M2_DIR1, 1);
    digitalWrite(M2_DIR2, 0);
    analogWrite(M2_PWM, 255);
    delay(150);

    //주차 - 후진 좌회전 45도 가량
    servo.write(35); // 90 - 55
    delay(300);
    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);
    delay(300);

    servo.write(35); // 90 - 55

    digitalWrite(M1_DIR2, 1);
    digitalWrite(M1_DIR1, 0);
    analogWrite(M1_PWM, 255);   
    digitalWrite(M2_DIR1, 1);
    digitalWrite(M2_DIR2, 0);
    analogWrite(M2_PWM, 255);   
    delay(100);

 
    
    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);

    delay(300);

    //주차 - 후진 파킹 완료
    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);

    servo.write(90);
    delay(300);
    
    digitalWrite(M1_DIR1, 1);
    digitalWrite(M1_DIR2, 0);
    analogWrite(M1_PWM, 255);
    digitalWrite(M2_DIR1, 1);
    digitalWrite(M2_DIR2, 0);
    analogWrite(M2_PWM, 255);
    delay(150);

    //주차 완료 정차
    delay(3000);

    //이탈 - 전진 좌회전
    servo.write(35); // 90 - 55

    digitalWrite(M1_DIR2, 0);
    digitalWrite(M1_DIR1, 1);
    analogWrite(M1_PWM, 255);   
    digitalWrite(M2_DIR1, 0);
    digitalWrite(M2_DIR2, 1);
    analogWrite(M2_PWM, 255);   
    delay(100);

    // 이탈 - 직진
    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);

    servo.write(90);
    delay(300);

    digitalWrite(M1_DIR1, 0);
    digitalWrite(M1_DIR2, 1);
    analogWrite(M1_PWM, 255);
    digitalWrite(M2_DIR1, 0);
    digitalWrite(M2_DIR2, 1);
    analogWrite(M2_PWM, 255);
    delay(150);

    // 이탈 - 우회전 
    servo.write(145); // 90 + 55
    digitalWrite(M1_DIR1, LOW);
    digitalWrite(M1_DIR2, LOW);
    analogWrite(M1_PWM, 0);
    digitalWrite(M2_DIR1, LOW);
    digitalWrite(M2_DIR2, LOW);
    analogWrite(M2_PWM, 0);
    delay(300);

    digitalWrite(M1_DIR2, LOW);
    digitalWrite(M1_DIR1, HIGH);
    analogWrite(M1_PWM, 255);   
    digitalWrite(M2_DIR1, 0);
    digitalWrite(M2_DIR2, 1);
    analogWrite(M2_PWM, 255); 
    delay(600);
}
