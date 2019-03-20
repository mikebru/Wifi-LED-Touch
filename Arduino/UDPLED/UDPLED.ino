/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

#include <FastLED.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>

// Here's how to control the LEDs from any two pins:
#define DATAPIN 13

// How many leds in your strip?
#define NUM_LEDS 60

// Define the array of leds
CRGB leds[NUM_LEDS];
const int dataChunkSize = 3;
int maxBright = 64;
const int numOfBytes = NUM_LEDS * dataChunkSize;
int byteReturnLen = 0;
int byteReturnCounter = 0;
char inputBuffer[numOfBytes];
int led = 0;
int led_r = 0;
int led_g = 0;
int led_b = 0;


// WiFi network name and password:
const char * networkName = "yourNetwork";
const char * networkPswd = "yourPassword";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.0.20";
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){

  FastLED.addLeds<NEOPIXEL, DATAPIN>(leds, NUM_LEDS);

  FastLED.show();
  
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}


// ------------------- Gama Lookup table -------------------- //
const byte dim_curve[] = {
  0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   4,   4,
  4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,
  8,   8,   9,   9,   9,  10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,
 14,  15,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,
 22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  30,  30,  31,  32,
 33,  33,  34,  35,  36,  36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  45,
 46,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,
 61,  62,  63,  64,  65,  67,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,
 80,  81,  82,  83,  85,  86,  87,  89,  90,  91,  93,  94,  95,  97,  98,  99,
101, 102, 104, 105, 107, 108, 110, 111, 113, 114, 116, 117, 119, 121, 122, 124,
125, 127, 129, 130, 132, 134, 135, 137, 139, 141, 142, 144, 146, 148, 150, 151,
153, 155, 157, 159, 161, 163, 165, 166, 168, 170, 172, 174, 176, 178, 180, 182,
184, 186, 189, 191, 193, 195, 197, 199, 201, 204, 206, 208, 210, 212, 215, 217,
219, 221, 224, 226, 228, 231, 233, 235, 238, 240, 243, 245, 248, 250, 253, 255 };

void loop(){
  //only send data when connected
  if(connected){
    //Send a packet
    //udp.beginPacket(udpAddress,udpPort);
    //udp.printf("Seconds since boot: %u", millis()/1000);
    //udp.endPacket();


    //processing incoming packet, must be called before reading the buffer
    udp.parsePacket();

    if(udp.read(inputBuffer, numOfBytes) > 0){
        for (int j = 0; j < NUM_LEDS; j++) { // for loop through our total led's set from above.
            led_r = constrain(dim_curve[inputBuffer[(j*dataChunkSize)]], 0, maxBright); // set each r,g, and b value through the gamma table and using offsets and multiple of three to traverse the array.
            led_g = constrain(dim_curve[inputBuffer[(j*dataChunkSize)+1]], 0, maxBright);
            led_b = constrain(dim_curve[inputBuffer[(j*dataChunkSize)+2]], 0, maxBright);

            inputBuffer[(j*dataChunkSize)] = 0;
            inputBuffer[(j*dataChunkSize) + 1] = 0;
            inputBuffer[(j*dataChunkSize) + 2] = 0;
               
            leds[j].setRGB( led_r, led_g, led_b);
          }
  }
        FastLED.show();
        udp.flush();
      //  delay(10);

  }
  //Wait for 1 second
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
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}



