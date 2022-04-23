
#include <ESP8266WiFi.h>
#include <espnow.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

 int biegung = A0;


 
#define TFT_RST 16
#define TFT_CS 15
#define TFT_DC 2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const uint grey = 0x0000;
const uint white = 0xFFFF;

int speed  = 0 ;
uint8_t broadcastAddress[] = {0x80, 0x7D, 0x3A, 0xEE, 0xD6, 0xC0};// REPLACE WITH RECEIVER MAC ADDRESS

//structure the message, send pwm receive vitesse 
typedef struct struct_message {
  int vitesse ;
  int pwm ;
} struct_message;

struct_message myData;

// message to print when data is sent (pwm)

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}



// message to print when data is received (vitesse)
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("from ESP 32 : Vitesse = ");
  Serial.println(myData.vitesse);
  Serial.println();
}

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  pinMode(0, INPUT_PULLUP);  // pin 0 arduino = pin D3 esp 8266               input = 0 when we press 

  pinMode(biegung,INPUT); // pin A0 
  
// check if esp now is correctly initialized
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

 // define the relation btw the 2 esp : combo = communication in both directions 
 
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);


  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);



  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

 tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(white);
 
  tft.setTextColor(grey);

  tft.setTextSize(2);
  tft.setCursor(30,20);
  tft.println("Speed:");


  
 tft.setTextSize(2);
tft.setCursor(90,70);
  tft.println("km/h");

}





void loop() {
int valBiegung = analogRead(biegung);
  int sensorVal = digitalRead(0);
if ( sensorVal == 0) {
if (valBiegung > 900 ) {
  speed = 0 ; 
}
else{
 speed = map(valBiegung, 800, 50 , 0 , 7);
}
}
if (sensorVal == 1 ) {

  
}
delay(50);
  tft.fillRect (20,40,60,60,white);
// read value of captor 
  
  // print value of captor 
  Serial.println(valBiegung);
  if (sensorVal == 0) {
   myData.pwm =  valBiegung ;
  }
  else { myData.pwm = -10; }
  Serial.print(sensorVal);
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));


    
  
   

    tft.setTextSize(5);
    tft.setCursor(20,60);
  tft.println(speed);

  




}
