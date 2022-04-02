#include "SoftwareSerial.h"
String ssid ="One Plus 5";
String password="123456789";
SoftwareSerial esp(2, 3);//(RX,TX)
String data;
String server = "192.168.43.213";
String uri = "/write_val.php";
int isObstaclePin = 8;  //The value of this pin determines whether car is present or not(Sent by irsensor)
int isObstaclePin2 = 9;
int isObstaclePin3 = 10;
int isObstaclePin4 = 11;
int gatePin = 12;

void setup() {
pinMode (isObstaclePin,INPUT);
pinMode (isObstaclePin2,INPUT);
pinMode (isObstaclePin3,INPUT);
pinMode (isObstaclePin4,INPUT);
pinMode (gatePin,INPUT);
esp.begin(115200);
Serial.begin(9600);
reset();
connectWifi();
}

//Reset the esp8266 module
void reset(){
esp.println("AT+RST");//Module restart
delay(1000);
if(esp.find("OK")) 
  Serial.println("Module Restart");
}

//Connect to your wifi network
void connectWifi() {
String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";         //AT+CWJAP="Champ","abcd1234"
esp.println(cmd);
delay(1000);
if(esp.find("OK")){ 
  Serial.println("Connected to "+ssid);
}
else {
connectWifi();
Serial.println("Cannot connect to wifi"); 
}
}

void loop () {

uint8_t irvalue=digitalRead(isObstaclePin);
uint8_t irvalue2=digitalRead(isObstaclePin2);
uint8_t irvalue3=digitalRead(isObstaclePin3);
uint8_t irvalue4=digitalRead(isObstaclePin4);
uint8_t gate=digitalRead(gatePin);

data = "irvalue="+String(irvalue)+"&irvalue2="+String(irvalue2)+"&irvalue3="+String(irvalue3)+"&irvalue4="+String(irvalue4)+"&gate="+String(gate);//Data sent must be under this form name1=value1&name2=value2
httppost();
//delay(1000);
}

void httppost () {
esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//Start a TCP connection as AT+CIPSTART="TCP","192.168.43.213","80"
if(esp.find("OK")){
  Serial.println("TCP connection ready");
} 
//delay(1000);
String postRequest =
"POST " + uri + " HTTP/1.0\r\n" + "Host: " + server + "\r\n" + "Accept: *" + "/" + "*\r\n" +
"Content-Length: " + data.length() + "\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "\r\n" + data;
String sendCmd = "AT+CIPSEND=";
esp.print(sendCmd);
esp.println(postRequest.length());
//delay(500);
if(esp.find(">")){
  Serial.println("Sending..."); 
  esp.print(postRequest);
  if(esp.find("SEND OK")){
    Serial.println("Packet sent");
  while (esp.available()){
    String tmpResp = esp.readString();
    Serial.println(tmpResp);
  }
  esp.println("AT+CIPCLOSE");//Close the TCP connection
}
}
}
