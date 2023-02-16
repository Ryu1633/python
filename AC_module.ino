#define TEMP A7 //LM35 ADC Input Pin
#define ENA 9   //L298N Enable A Output
#define IN1 8   //L298N MotorA IN1 High Output
#define IN2 7   //L298N MotorA IN2 Low Output

//UART
String inputString = "";     //RX buffer
bool stringComplete = false; //RX flag

//ctrl = on,off 제어, target_temp = 목표온도
int ctrl, target_temp = 0;

int reading = 0;  //ADC INPUT
float actual_temp = 0;  //측정온도

//상태값 정의(초기값 = WAIT)
enum {WAIT, START, STOP} state = WAIT;

void setup() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = (int)142;
  DDRB = 0x02;

  TCCR1A |= (1<<COM1A1);
  TCCR1A |= (1<<WGM10);
  TCCR1B |= (1<<WGM12);
  interrupts();

  analogReference(INTERNAL);
  for(int i = 0; i < 3; i++) {
    reading = analogRead(TEMP);
    actual_temp = reading / 9.31;
    delay(500);
  }

  Serial.begin(9600);
} //end setup()

void loop() {
  switch(state) {
    case WAIT:
      if(stringComplete) {
        Serial.println(String(actual_temp));

        //on, off여부, 목표온도값
        ctrl = String(inputString.substring(inputString.indexOf("c")+1, inputString.indexOf("t"))).toInt();
        target_temp = String(inputString.substring(inputString.indexOf("t")+1)).toInt();

        //동작 명령에 따른 상태 변화
        state = ctrl ? START : STOP;

        //버퍼, 플래그 초기화
        inputString = "";
        stringComplete = false;
      } //end if

      //온도 계산
      reading = analogRead(TEMP);
      actual_temp = reading / 9.31;
      actual_temp = floor(actual_temp);

      //온도 설정에 따른 상태 변화
      if(target_temp && ctrl){
        state = int(actual_temp) <= target_temp ? STOP : state;
      }

    break;  //end WAIT

    case START:
      TCCR1B |= (1<<CS12);
      TCCR1B |= (1<<CS10);
      DDRB = 0x02;
      
      state = WAIT;
    break;  //end START

    case STOP:
      DDRB = 0;
      state = WAIT;
    break;  //end STOP
  } //end switch
}//end loop()

//UART 인터럽트
void serialEvent() {
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();
    if (inChar == '\n')
      stringComplete = true;
  }//end while
} //end serialEvent()
