#include <ESP8266WiFi.h>

WiFiServer server(80);

const char *ssid = "smartmirror";
const char *password = "smartmirror";
String inputString = "";  //수신 데이터 저장 변수
bool stringComplete = false;  //수신 완료 플래그
volatile int actual_temp = 0;

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  inputString.reserve(200);
  
  Serial.begin(9600);
  server.begin();
}

void loop() {
  WiFiClient client1 = server.available();
  
  if (client1) {                          
    String currentLine = "";
    String serialLine = "";
    
    while (client1.connected()) {          
      if (client1.available()) {               
        char c = client1.read();
        currentLine += c;
        
        if (c == '}') {           
            client1.println("HTTP/1.1 200 OK");
            client1.println("{\"temp\":\"" + String(actual_temp) + "\"}");
            break;  //break #31 while
        } //end if
      } //end if
    } //end while

    client1.stop();

    String ctrl = currentLine.substring(currentLine.indexOf("ctrl")+7, currentLine.indexOf("temp")-3);
    String target_temp = currentLine.substring(currentLine.indexOf("temp")+7, currentLine.indexOf("}")-1);

    serialLine = "c" + ctrl + "t" + target_temp;

    Serial.println(serialLine);
  } //end if
} //end loop

//UART 인터럽트
void serialEvent() {
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();
    inputString += inChar;
    
    if (inChar == '\n')
      actual_temp = inputString.toInt();
      
  }//end while
} //end serialEvent()
