/*
 *  This sketch recieves LED data over UDP and outputs it to LEDs
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>



IPAddress remoteIP(192,168,1,8);
int remotePort = 7777;

unsigned long currentMillis;
unsigned long startMillis;
const unsigned long period = 5000;

// WiFi network name and password:
const char * networkName = "InteractiveNature-2.4G";
const char * networkPswd = "ricksaidso";

//Are we currently connected?
boolean connected = false;


char outputBuffer[255];


//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}




void loop(){
  //only send data when connected
  if(connected){

    SendVoltage();
  }

}


void SendVoltage()
{
  float voltage = (float(analogRead(35))/4095)* 2 * 3.3 * 1.1;
  udp.beginPacket(remoteIP, remotePort);

  String message = WiFi.localIP().toString() +"/" + (String)voltage;
  Serial.println(message);

  message.toCharArray(outputBuffer, 255);

  //strcpy(outputBuffer, message);   
  udp.write((byte*)outputBuffer, strlen(outputBuffer));
  udp.endPacket();
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  WiFi.setSleep(false);
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),remotePort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
