const int IR_R = A3;  //  적외선센서 우측 핀
const int IR_L = A4;  // 적외선센서 좌측 핀

void setup() {
    Serial.begin(115200);
    pinMode(IR_R, INPUT);
    pinMode(IR_L, INPUT);
}

void loop() {

    Serial.print("Right : "); Serial.print(analogRead(IR_R));
     Serial.print("    Left : "); Serial.println(analogRead(IR_L));
    delay(500);
    
}
