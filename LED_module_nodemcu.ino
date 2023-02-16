#include <ESP8266WiFi.h>

const char *ssid = "smartmirror";
const char *password = "smartmirror";

WiFiServer server(80);

String inputString = "";
bool stringComplete = false;

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  inputString.reserve(200);
  
  Serial.begin(9600);
  server.begin();
}

void loop() {
 
  WiFiClient client1 = server.available();
  
  if (client1) {                          
    String currentLine = "";  //읽어온 데이터 초기화
    String serialLine = "";   //파싱된 데이터 초기화
    
    while (client1.connected()) {          
      if (client1.available()) { 
                  
        char c = client1.read();
        currentLine += c;
        
        if (c == '}') {           
            client1.println("HTTP/1.1 200 OK");
            break;
        } //end if #34
      } //end if #29
    } //end while #28

    client1.stop();

    String Ctrl = currentLine.substring(currentLine.indexOf("ctrl")+7, currentLine.indexOf("redValue")-3);
    String Redvalue = currentLine.substring(currentLine.indexOf("redValue")+11, currentLine.indexOf("greenValue")-3);
    String Greenvalue = currentLine.substring(currentLine.indexOf("greenValue")+13, currentLine.indexOf("blueValue")-3);
    String Bluevalue = currentLine.substring(currentLine.indexOf("blueValue")+12, currentLine.indexOf("}")-1);

    serialLine = "c" + Ctrl + "r" + Redvalue + "g" + Greenvalue + "b" + Bluevalue;
    Serial.println(serialLine);
  } //end if  
} //end loop
