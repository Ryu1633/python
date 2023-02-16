#define LED_R 9
#define LED_G 10
#define LED_B 11

int ctrl, PWM_R, PWM_G, PWM_B = 0;
int led_change[] = {0, 0};

String inputString = "";
bool stringComplete = false;

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  inputString.reserve(200);

  Serial.begin(9600);
}//end setup()

void loop() {
  //파싱
  if (stringComplete) {
    
    //pc.println("데이터= " + inputString);
    
    ctrl = String(inputString.substring(inputString.indexOf("c")+1, inputString.indexOf("r"))).toInt();
    PWM_R = String(inputString.substring(inputString.indexOf("r")+1, inputString.indexOf("g"))).toInt();
    PWM_G = String(inputString.substring(inputString.indexOf("g")+1, inputString.indexOf("b"))).toInt();
    PWM_B = String(inputString.substring(inputString.indexOf("b")+1)).toInt();

    led_change[0] = ctrl + PWM_R + PWM_G + PWM_B;

    //수신 데이터 버퍼 초기화
    inputString = "";
    stringComplete = false;
  } //end if #29

  //데이터 변화가 있으면 상태 갱신
  if(led_change[0] != led_change[1]) {
       
    led_change[1] = led_change[0];

    //동작 명령시 PWM 출력
    if(ctrl) {
      analogWrite(LED_R, PWM_R);
      analogWrite(LED_G, PWM_G);
      analogWrite(LED_B, PWM_B);
    }

    //정지 명령시 LOW 출력
    else if(!ctrl) {
      digitalWrite(LED_R, 0);
      digitalWrite(LED_G, 0);
      digitalWrite(LED_B, 0);
    }
    
  } //end if(led_change)
  
} //end loop()

//UART 인터럽트
void serialEvent() {
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();
    inputString += inChar;
    
    if (inChar == '\n')
      stringComplete = true;
      
  }//end while
} //end serialEvent()
