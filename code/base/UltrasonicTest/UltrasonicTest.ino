// Pin map
#define FC_TRIG 13   // 전방 초음파 센서 TRIG 핀
#define FC_ECHO 10  // 전방 초음파 센서 ECHO 핀
#define L_TRIG  A2  // 좌측 초음파 센서 TRIG 핀
#define L_ECHO  A1  // 좌측 초음파 센서 ECHO 핀
#define R_TRIG  2   // 우측 초음파 센서 TRIG 핀
#define R_ECHO  A5  // 우측 초음파 센서 ECHO 핀

float f_center;
float left;
float right;

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

void setup()
{
  pinMode(FC_TRIG, OUTPUT);
  pinMode(FC_ECHO, INPUT);
  
  pinMode(L_TRIG, OUTPUT);
  pinMode(L_ECHO, INPUT);
  
  pinMode(R_TRIG, OUTPUT);
  pinMode(R_ECHO, INPUT);
  
  Serial.begin(9600);
}

void loop()
{
  f_center = GetDistance(FC_TRIG, FC_ECHO);
  Serial.print("FC:");
  Serial.print(f_center);
  Serial.print("\n");

  left = GetDistance(L_TRIG, L_ECHO);
  Serial.print("L:");
  Serial.print(left);
  Serial.print("\n");
  
  right = GetDistance(R_TRIG, R_ECHO);
  Serial.print("R:");
  Serial.print(right);
  Serial.print("\n");
 
  delay(100);
}
