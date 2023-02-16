#define PIN1 9
#define PIN2 8
#define PIN3 11
#define PIN4 10
#define STEP 2048

//UART
String inputString = ""; 
bool stringComplete = false;

int ctrl, motor_step, roll_count, motor_direction, check = 0;
int pinArray[4] = {PIN1, PIN2, PIN3, PIN4}; //모터 제어 신호 출력핀
int CW[4] = {0x0C, 0x06, 0x03, 0x09}; //2 phase excitation clock wise
int CCW[4] = {0x09, 0x3, 0x06, 0x0C}; //2 phase excitation counter clock wise

//상태값 정의(초기값 = WAIT)
enum {WAIT, RUN} state = WAIT;

void setup() {
  //핀 출력 설정
  for(int i = 0; i < 4; i++)
    pinMode(pinArray[i], OUTPUT);

  Serial.begin(9600);
} //end setup()

void loop() {
  switch(state) {
    
    case WAIT:  //대기상태
      //파싱
      if(stringComplete) {
        //필요한 데이터 = 커튼 상태(내림 = 1, 올림 = 0), 상태 전송 체크
        ctrl = String(inputString.substring(inputString.indexOf("c")+1)).toInt();;

        //동작 명령에 따른 상태 변화
        state = ctrl ? RUN : WAIT;


        //버퍼 초기화
        inputString = "";
        stringComplete = false;
      } //end if
    break;  //end WAIT

    case RUN:
      switch(motor_direction) {
        //정방향 회전(커튼 내림)
        case 0: //CW
          digitalWrite(pinArray[0], bitRead(CW[motor_step % 4], 0));  
          digitalWrite(pinArray[1], bitRead(CW[motor_step % 4], 1));  
          digitalWrite(pinArray[2], bitRead(CW[motor_step % 4], 2));  
          digitalWrite(pinArray[3], bitRead(CW[motor_step % 4], 3));
          motor_step++;
          if(motor_step == 2048){
            motor_step = 0;
            roll_count++;
          }

          //커튼이 닫히면 대기상태 전환
          if(roll_count == 3) {
            ctrl = 0;
            motor_direction = 1;
            state = WAIT;
          }
            
        break;  //case 0
        
        //역방향 회전(커튼 올림)
        case 1: //CCW
          digitalWrite(pinArray[0], bitRead(CCW[motor_step % 4], 0));  
          digitalWrite(pinArray[1], bitRead(CCW[motor_step % 4], 1));  
          digitalWrite(pinArray[2], bitRead(CCW[motor_step % 4], 2));  
          digitalWrite(pinArray[3], bitRead(CCW[motor_step % 4], 3));
          motor_step++;
          if(motor_step == 2048){
            motor_step = 0;
            roll_count--;
          }

          //커튼이 열리면 대기상태 전환
          if(roll_count == 0) {
            ctrl = 0;
            motor_direction = 0;
            state = WAIT;
          }
        break;  //end case 1
      } //end siwtch(motor_direction)
    delay(2); //모터 회전 보정시간 2ms
    break;  //end RUN

  } //end switch
}//end loop()

//UART 인터럽트
void serialEvent() {
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();
    inputString += inChar;
    
    if (inChar == '\n')
      stringComplete = true;
      
  }//end while
} //end serialEvent()
