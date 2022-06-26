void T_Parking()
{
{
    
    
    SetSteering(0);
    SetSpeed(0);
    delay(100);
    SetSpeed(1);
    delay(400);
    while(1) {
      SetSpeed(0.5);
      SetSteering(-1);
      delay(20);
      if (ir_sensing(IR_R) <= detect_ir){
        SetSteering(1);
        SetSpeed(-0.5);
        delay(100);
        continue;
        }
      else if (ir_sensing(IR_L) <= detect_ir){
        break;
      }
    }
    SetSpeed(-0.5);
    SetSteering(-1);
    delay(100);
    while(1) {
        if (ir_sensing(IR_L) <= detect_ir && ir_sensing(IR_R) <= detect_ir)
             break;
        else if (ir_sensing(IR_L) <= detct_ir)
             SetSpeed(-0.3);
             SetSteering(-1);
             delay(20);
        else
             SetSpeed(-0.3);
             SetSteering(1);
             SetSpeed(20);
        SetSteering(0);
        SetSpeed(20);
    }
    SetSteering(0);
    while (ir_sensing(IR_R) > detect_ir or ir_sensing(IR_L) > detect_ir){
        SetSpeed(-0.3);
    }
    SetSpeed(0);
    SetSteering(0);
    delay(2000);
    SetSpeed(0.5);
    delay(200);

}
