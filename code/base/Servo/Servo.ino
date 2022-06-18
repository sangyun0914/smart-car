#include <Servo.h>
Servo servo;

const int SERVO_PIN = 9;      // 서보모터1 연결핀


void setup() {
    Serial.begin(9600);

    //서보모터 초기화
    servo.attach(SERVO_PIN); 
    servo.write(0);
    delay(500);
    servo.write(180);
    delay(500);
    servo.write(90);       // 오른쪽 이동(정상동작 확인용 코드)
    delay(1000);
    servo.detach();
}

void loop() {

}
