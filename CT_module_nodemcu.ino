#include <ESP8266WiFi.h>

WiFiServer server(80);

const char *ssid = "smartmirror";
const char *password = "smartmirror";
String inputString = "";
bool stringComplete = false;

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
            break;
        } //end if
      } //end if
    } //end while

    client1.stop();

    String ctrl = currentLine.substring(currentLine.indexOf("ctrl")+7, currentLine.indexOf("}")-1);

    serialLine = "c" + ctrl;

    Serial.println(serialLine);
    
  } //end if
} //end loop
