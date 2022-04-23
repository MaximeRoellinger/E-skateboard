

// A4:CF:12:6C:2A:90 esp32  sender!
// esp 8266 0x44, 0x17, 0x93, 0x1F, 0xAE, 0x75

#include <ESP32Servo.h>
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
uint8_t broadcastAddress[] = {0x44, 0x17 ,0x93, 0x1F, 0xAE, 0x75};// REPLACE WITH RECEIVER MAC ADDRESS

int somme = 0 ;
const byte PIN_SIGNAL = 15;
int SendPwm;
Servo myservo;
Servo myservo2;

typedef struct struct_message {
  int  vitesse;
  int pwm;

} struct_message;

struct_message myData;



void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nMaster packet sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}



void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("From ESP8266 : PWM =  ");
  Serial.println(myData.pwm);
  Serial.println();
}
void setup() {

 myservo.attach(13);
  pinMode(PIN_SIGNAL, INPUT);
pinMode(19, OUTPUT);




  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("Error initializing ESP-NOW"));
    return;
  }


  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("Failed to add peer"));
    return;

    myservo.write(90);
    
    delay(5000);
    myservo.write(180);
       

    delay(5000);
  }
}
void loop() {

digitalWrite(19, HIGH);
int biegung = myData.pwm;

if (biegung < 0 ) {
   myservo.detach();

 // digitalWrite(13 , LOW);
}
else {
  myservo.attach(13);
SendPwm = map(biegung, 1024, 50, 90,180 );    // modifier ici la plage de la résistance 
  // noInterrupts();
 
myservo.write(SendPwm);
}
 


//  for(int i = 0;  i<10 ; i++){                      // modifier la boucle ici si calculs trop longs 
 //int duration = pulseIn(PIN_SIGNAL, HIGH);
//   somme = somme + duration; }

//   interrupts();
//somme = somme /100;

// somme = 35 - duration ;


 // myData.vitesse = somme;
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

 delay(100);

}
