#include <Servo.h>

#define NOTE_E6 1319
#define NOTE_C6 1047
#define NOTE_C5 523
#define NOTE_G4 392
#define NOTE_G5 784
#define NOTE_E5 659
#define REST 0

Servo servo;

/**
 * Black IR detection: 33
 * White IR detection: 21
 */

const int SERVO1_PIN = 9; // 서보모터1 연결핀
const int IR_R = A3;      //  적외선센서 우측 핀
const int IR_L = A4;      // 적외선센서 좌측 핀

const int M1_PWM = 5;  // DC모터1 PWM 핀 왼
const int M1_DIR1 = 7; // DC모터1 DIR1 핀
const int M1_DIR2 = 8; // DC모터 1 DIR2 핀

const int M2_PWM = 6;   // DC모터2 PWM 핀
const int M2_DIR1 = 11; // DC모터2 DIR1 핀
const int M2_DIR2 = 12; // DC모터2 DIR2 핀

const int FC_TRIG = 13; // 전방 초음파 센서 TRIG 핀
const int FC_ECHO = 10; // 전방 초음파 센서 ECHO 핀
const int L_TRIG = A2;  // 좌측 초음파 센서 TRIG 핀
const int L_ECHO = A1;  // 좌측 초음파 센서 ECHO 핀
const int R_TRIG = 2;   // 우측 초음파 센서 TRIG 핀
const int R_ECHO = A5;  // 우측 초음파 센서 ECHO 핀

const int MAX_DISTANCE = 2000; // 초음파 센서의 최대 감지거리 실제로는 300언저리...
const int PAUSE_TIME = 2000;
const int buzzer = 3;

float center;
float left;
float right;

int state = 0;
int count_lines = 0;
int turn_time = 0;
int tempo = 200;

int melody[] = {
    NOTE_E6, 4, NOTE_C6, 2,
    NOTE_C5, 8, NOTE_C5, 8, NOTE_C5, 8,
    NOTE_G4, 8, NOTE_G5, 4, NOTE_G5, 8, NOTE_E5, 4,
    REST, 4};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

// 자동차 튜닝 파라미터 =====================================================================
int detect_ir = 27; // 검출선이 흰색과 검정색 비교

int punch_pwm = 200; // 정지 마찰력 극복 출력 (0 ~ 255)
int punch_time = 50; // 정지 마찰력 극복 시간 (단위 msec)
int stop_time = 300; // 전진후진 전환 시간 (단위 msec)

int max_ai_pwm = 130; // 자율주행 모터 최대 출력 (0 ~ 255)
int min_ai_pwm = 70;  // 자율주행 모터 최소 출력 (0 ~ 255)

int angle_offset = -8; // 서보 모터 중앙각 오프셋 (단위: 도)
int angle_limit = 55;  // 서보 모터 회전 제한 각 (단위: 도)

int center_detect = 200; // 전방 감지 거리 (단위: mm)
int center_start = 160;  // 전방 출발 거리 (단위: mm)
int center_stop = 70;    // 전방 멈춤 거리 (단위: mm)

int side_detect = 100; // 좌우 감지 거리 (단위: mm)

float cur_steering;
float cur_speed;
float compute_steering;
float compute_speed;

float max_pwm;
float min_pwm;

int start = 0;

void playSong()
{
    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
    {

        // calculates the duration of each note
        divider = melody[thisNote + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote) / abs(divider);
            noteDuration *= 1.5; // increases the duration in half for dotted notes
        }

        // we only play the note for 90% of the duration, leaving 10% as a pause
        tone(buzzer, melody[thisNote], noteDuration * 0.9);

        // Wait for the specief duration before playing the next note.
        delay(noteDuration);

        // stop the waveform generation before the next note.
        noTone(buzzer);
    }
}

// 초음파 거리측정
float GetDistance(int trig, int echo)
{
    digitalWrite(trig, LOW);
    delayMicroseconds(4);
    digitalWrite(trig, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig, LOW);

    unsigned long duration = pulseIn(echo, HIGH, 5000);
    if (duration == 0)
        return MAX_DISTANCE;
    else
        return duration * 0.17; // 음속 340m/s
}

int ir_sensing(int pin)
{
    return analogRead(pin);
}

// 앞바퀴 조향
void SetSteering(float steering)
{
    cur_steering = constrain(steering, -1, 1); // constrain -1~ 1 값으로 제한

    float angle = cur_steering * angle_limit;
    int servoAngle = angle + 90;
    servoAngle += angle_offset;

    servoAngle = constrain(servoAngle, 0, 180);
    servo.write(servoAngle);
}

// 뒷바퀴 모터회전
void SetSpeed(float speed)
{
    speed = constrain(speed, -1, 1);

    if ((cur_speed * speed < 0)            // 움직이는 중 반대 방향 명령이거나
        || (cur_speed != 0 && speed == 0)) // 움직이다가 정지라면
    {
        cur_speed = 0;
        digitalWrite(M1_PWM, HIGH);
        digitalWrite(M1_DIR1, LOW);
        digitalWrite(M1_DIR2, LOW);

        digitalWrite(M2_PWM, HIGH);
        digitalWrite(M2_DIR1, LOW);
        digitalWrite(M2_DIR2, LOW);

        if (stop_time > 0)
            delay(stop_time);
    }

    if (cur_speed == 0 && speed != 0) // 정지상태에서 출발이라면
    {
        if (punch_time > 0)
        {
            if (speed > 0)
            {
                analogWrite(M1_PWM, punch_pwm);
                digitalWrite(M1_DIR1, HIGH);
                digitalWrite(M1_DIR2, LOW);

                analogWrite(M2_PWM, punch_pwm);
                digitalWrite(M2_DIR1, HIGH);
                digitalWrite(M2_DIR2, LOW);
            }
            else if (speed < 0)
            {
                analogWrite(M1_PWM, punch_pwm);
                digitalWrite(M1_DIR1, LOW);
                digitalWrite(M1_DIR2, HIGH);

                analogWrite(M2_PWM, punch_pwm);
                digitalWrite(M2_DIR1, LOW);
                digitalWrite(M2_DIR2, HIGH);
            }
            delay(punch_time);
        }
    }

    if (speed != 0) // 명령이 정지가 아니라면
    {
        int pwm = abs(speed) * (max_pwm - min_pwm) + min_pwm; // 0 ~ 255로 변환

        if (speed > 0)
        {
            analogWrite(M1_PWM, pwm);
            digitalWrite(M1_DIR1, HIGH);
            digitalWrite(M1_DIR2, LOW);

            analogWrite(M2_PWM, pwm);
            digitalWrite(M2_DIR1, HIGH);
            digitalWrite(M2_DIR2, LOW);
        }
        else if (speed < 0)
        {
            analogWrite(M1_PWM, pwm);
            digitalWrite(M1_DIR1, LOW);
            digitalWrite(M1_DIR2, HIGH);

            analogWrite(M2_PWM, pwm);
            digitalWrite(M2_DIR1, LOW);
            digitalWrite(M2_DIR2, HIGH);
        }
    }
    cur_speed = speed;
}

int checkLine(int IR) // Check for valid line
{
    if (IR == 0) // Check for line
    {
        if (ir_sensing(IR_R) <= detect_ir && ir_sensing(IR_L) <= detect_ir)
        {
            SetSpeed(0);
            SetSteering(0);
            delay(100);
            if (ir_sensing(IR_R) <= detect_ir && ir_sensing(IR_L) <= detect_ir)
            {
                return 1;
            }
        }
        return 0;
    }
    else // Check for valid sensor
    {
        if (ir_sensing(IR) <= detect_ir)
        {
            SetSpeed(0);
            SetSteering(0);
            delay(100);
            if (ir_sensing(IR) <= detect_ir)
            {
                return 1;
            }
        }
        return 0;
    }
}

void straight()
{
    if (ir_sensing(IR_R) >= detect_ir && ir_sensing(IR_L) >= detect_ir) //차선이 검출되지 않을 경우 직진
    {
        compute_steering = 0;
        compute_speed = 0.1;
    }

    else if (ir_sensing(IR_R) <= detect_ir) // 오른쪽 차선이 검출된 경우
    {
        compute_steering = -1;
        compute_speed = 0.1;
    }

    else if (ir_sensing(IR_L) <= detect_ir) // 왼쪽 차선이 검출된 경우
    {
        compute_steering = 1;
        compute_speed = 0.1;
    }
}

void ParallelParking()
{

    while (true) // Move until right sensor detects wall
    {
        if (GetDistance(R_TRIG, R_ECHO) > 180)
            break;
        SetSteering(0);
        SetSpeed(0.5);
    }
    while (true) // Move until right sensor detects wall
    {
        if (GetDistance(R_TRIG, R_ECHO) <= 180)
            break;
        SetSteering(0);
        SetSpeed(-0.5);
    }
    delay(200);

    while (true) // Move until right sensor detects wall
    {
        if (checkLine(IR_R))
            break;
        SetSteering(0.8);
        SetSpeed(0.1);
        delay(20);
        turn_time++;
    }
    SetSteering(0);
    SetSpeed(-0.5);
    delay(750);
    for (int i = 0; i < turn_time; i++)
    {
        SetSteering(-0.8);
        SetSpeed(0.1);
        delay(20);
        if (checkLine(IR_L))
        {
            Serial.print("Detected 80 FC ");
            break;
        }
    }

    SetSteering(0.2);
    SetSpeed(-0.5);
    delay(900);

    SetSpeed(0);
    delay(2000);

    while (!(checkLine(IR_L)))
    {
        SetSpeed(0.5);
        SetSteering(-1);
    }
    SetSteering(-1);
    SetSpeed(-0.3);
    delay(800);
}

void T_Parking()
{
    SetSteering(0);
    SetSpeed(0);
    delay(100);
    SetSpeed(1);
    delay(100);
    while (1)
    {
        SetSpeed(0.4);
        SetSteering(-1);
        delay(50);
        if (ir_sensing(IR_R) <= detect_ir)
        {
            SetSteering(1);
            SetSpeed(-0.5);
            delay(200);
            continue;
        }
        else if (ir_sensing(IR_L) <= detect_ir)
        {
            break;
        }
    }
    SetSpeed(-0.5);
    SetSteering(-1);
    delay(120);

    SetSteering(0);
    while (ir_sensing(IR_R) > detect_ir or ir_sensing(IR_L) > detect_ir)
    {
        SetSpeed(-0.3);
    }
    SetSpeed(0);
    SetSteering(0);
    delay(2000);
    SetSpeed(0.5);
    delay(200);
}

void avoid_collision()
{
    SetSpeed(1);
    SetSteering(0);
    delay(180);

    while (!(checkLine(IR_L)))
    {
        SetSpeed(0.5);
        SetSteering(-1);
    }
    SetSpeed(-0.5);
    SetSteering(0);
    delay(200);
    angle_limit = 75;

    while (!(GetDistance(FC_TRIG, FC_ECHO) <= 50 && GetDistance(L_TRIG, L_ECHO) <= 200))
    {
        if (ir_sensing(IR_R) >= detect_ir && ir_sensing(IR_L) >= detect_ir) //차선이 검출되지 않을 경우 직진
        {
            SetSteering(0);
            SetSpeed(0.5);
        }

        else if (ir_sensing(IR_R) <= detect_ir) // 오른쪽 차선이 검출된 경우
        {
            SetSteering(1);
            SetSpeed(-0.1);
            delay(100);
            SetSteering(-1);
            SetSpeed(0.05);
        }

        else if (ir_sensing(IR_L) <= detect_ir) // 왼쪽 차선이 검출된 경우
        {
            SetSteering(-1);
            SetSpeed(-0.1);
            delay(100);
            SetSteering(1);
            SetSpeed(0.05);
        }
    }
}

void finish() // TODO Implement finish() functoin
{
    // break loop, to finish program.
    SetSpeed(0);
    playSong();
    exit(0);
}
void driving()
{

    compute_steering = cur_steering;
    compute_speed = cur_speed;

    straight();
    SetSpeed(compute_speed);
    SetSteering(compute_steering);

    if (checkLine(0)) // 양쪽 차선이 검출된 경우=>정지선 또는 차선과 정지선을 모두 걸친 경우
    {
        Serial.println("Detected Line!");
        SetSteering(0);
        SetSpeed(0); //일시정지(교차로에 정지선이 있기 때문에 무조건 멈춰야 함)
        delay(3000);

        SetSpeed(0.3); //초음파 감지 거리가 300 언저리라서 전방의 벽을 인식하려면 조금 앞으로 가는게 안전&정지마찰력때문에 속도는 너무 느리지 않게
        delay(100);
        SetSpeed(0);
        if (GetDistance(FC_TRIG, FC_ECHO) > 400) //전방에 벽이 안 느껴질 때---->교차로 진입이나 평행주차 중 하나구나
        {
            SetSpeed(0.5); //직진
            delay(400);
            SetSpeed(0);

            if (GetDistance(L_TRIG, L_ECHO) < 200) //왼쪽에서 벽이 감지되면 평행주차, 감지되지 않으면 교차로 진입(그대로 직진)
            {
                SetSpeed(0);
                Serial.println("Parallel Park!");

                ParallelParking();
            }
        }
        else if (GetDistance(FC_TRIG, FC_ECHO) < 300) //전방에 벽이 느껴질 때 T자 주차구나
        {
            Serial.print("T park");
            T_Parking();
            while (1)
            {
                straight();
                SetSpeed(compute_speed);
                SetSteering(compute_steering);
                if (ir_sensing(IR_R) <= detect_ir && ir_sensing(IR_L) <= detect_ir)

                    break;
            }
            avoid_collision();
            finish(); // Exit Program
        }
    }
}

void setup()
{
    Serial.begin(9600);
    servo.attach(SERVO1_PIN); // 서보모터 초기화

    pinMode(IR_R, INPUT);
    pinMode(IR_L, INPUT);

    pinMode(M1_PWM, OUTPUT);
    pinMode(M1_DIR1, OUTPUT);
    pinMode(M1_DIR2, OUTPUT);
    pinMode(M2_PWM, OUTPUT);
    pinMode(M2_DIR1, OUTPUT);
    pinMode(M2_DIR2, OUTPUT);

    pinMode(FC_TRIG, OUTPUT);
    pinMode(FC_ECHO, INPUT);

    pinMode(L_TRIG, OUTPUT);
    pinMode(L_ECHO, INPUT);

    pinMode(R_TRIG, OUTPUT);
    pinMode(R_ECHO, INPUT);

    max_pwm = max_ai_pwm;
    min_pwm = min_ai_pwm;

    SetSteering(0);
    SetSpeed(0);
}

void loop()
{
    if (GetDistance(FC_TRIG, FC_ECHO) <= center_detect && state == 0)
    {
        if (GetDistance(FC_TRIG, FC_ECHO) > center_detect)
        {
            playSong();
            state++;
        }
    }
    if (state != 0)
        driving();
}

/*
TODO: check IR sensing detect
TODO: check offset
TODO: check front wall length


*/