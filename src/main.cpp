#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define SS_LCD 15
#define RST_LCD 4
#define DC_LCD 14
#define MOSI_LCD 23
#define SCLK_LCD 18
#define TXD2 17
#define RXD2 16

String oldPayload = "";
String newDeviceId;
String newDeviceMode;
String newPinLength;
String newConnectionStatus; 
String oldDeviceId;
String oldDeviceMode;
String oldPinLength;
String oldConnectionStatus; 
Adafruit_ILI9341 display = Adafruit_ILI9341(SS_LCD, DC_LCD, MOSI_LCD, SCLK_LCD, RST_LCD);

void setUpText(String txt) {
  display.setTextWrap(false);
  display.setTextColor(ILI9341_WHITE);        
  display.setTextSize(1);             
  display.setCursor(10,7);             
  display.println(txt);
  display.setTextSize(4);             
  display.setCursor(76,64);             
  display.println(F("WELCOME"));
  display.setCursor(40,106);             
  display.println("SMART DOOR");
  display.setCursor(90,148);             
  display.println("SYSTEM");
  delay(2000);
}

void setGoodConnection(){
  display.fillRect(5,7, 90, 14, ILI9341_NAVY);
  display.fillRoundRect(5,7, 80, 14, 7, ILI9341_DARKGREEN);
  display.setTextSize(1);             
  display.setCursor(17,10);             
  display.println("connected");
}

void setLossConnection(){
  display.fillRect(5,7, 90, 14, ILI9341_NAVY);
  display.fillRoundRect(5,7, 90, 14, 7, ILI9341_RED);
  display.setTextSize(1);             
  display.setCursor(14,10);             
  display.println("disconnected");
}

void setDeviceId(String id){
  display.setTextSize(1);
  display.setTextColor(ILI9341_WHITE);
  display.setCursor(255, 7);
  display.println("Device ID:");
  display.setCursor(280, 18);
  display.println(id);
}


void wirteBottomSectionForCheckIn(){
  display.fillRect(40, 145, 240, 47, ILI9341_NAVY);
  display.setTextWrap(true);
  display.setTextSize(1);
  display.setTextColor(ILI9341_WHITE);
  display.setCursor(55, 148);
  display.println("IF YOU USE ONE STEP AUTHENTICATION,");
  display.setCursor(110, 160);
  display.println("JUST TAP YOUR CARD");
  display.setCursor(55, 172);
  display.println("IF YOU USE TWO STEP AUTHENTICATION,");
  display.setCursor(40, 182);
  display.println("ENTER YOUR PIN FIRST, THEN TAP YOUR CARD");
}

void wirteBottomSectionForAuth(){
  display.fillRect(40, 145, 240, 47, ILI9341_NAVY);
  display.setTextWrap(true);
  display.setTextSize(1);
  display.setTextColor(ILI9341_WHITE);
  display.setCursor(93, 148);
  display.println("TO ENTER INTO ADMIN MODE");
  display.setCursor(103, 160);
  display.println("PLEASE ENTER ROOM PIN");
  display.setCursor(107, 172);
  display.println("THEN PRES 'D' BOTTON");
}

void wirteBottomSectionForRegister(){
  display.fillRect(40, 145, 240, 47, ILI9341_NAVY);
  display.setTextWrap(true);
  display.setTextSize(1);
  display.setTextColor(ILI9341_WHITE);
  display.setCursor(100, 148);
  display.println("FOR CARD REGISTRATION,");
  display.setCursor(40, 160);
  display.println("ENTER YOUR PIN FIRST, THEN TAP YOUR CARD");
}

void writeTopSection(String mode) {
  display.fillRect(40, 63, 240, 30, ILI9341_NAVY);
  display.setTextSize(4);             
  display.setTextColor(ILI9341_WHITE);
  if(mode == "CIN"){        
    display.setCursor(40,64);             
    display.println("SMART DOOR");
    wirteBottomSectionForCheckIn();
  }

  if(mode == "REG"){
    display.setCursor(65,64);             
    display.println("REGISTER");
    wirteBottomSectionForRegister();
  }

  if(mode == "AUT"){
    display.setCursor(42,64);             
    display.println("ADMIN AUTH");
    wirteBottomSectionForAuth();
  }
}

void wirteMiddleSection(int size){
  display.setTextSize(4);
  display.fillRect(40, 106, 240, 30, ILI9341_NAVY);
  // display.setCursor(90,115);   
  display.setCursor(90,107);             
  if(size == 0){
    display.println("------");
  }else{
    String text = "";
    for (size_t i = 0; i < size; i++) {
      text+="*";
    }
            
    for (size_t i = 0; i < (6 - (size)); i++) {
      text+="-";
    }
    display.println(text);
  }
}

void writeAlert(String top_txt, String mid_txt, String bot_txt){
  display.fillRect(40, 63, 240, 30, ILI9341_NAVY);
  display.setTextSize(4);
  display.setTextColor(ILI9341_WHITE);
  int top_len = top_txt.length();
  if(top_len <= 10){
    int top_pot = ((320-(top_len*24))/2)+2;
    display.setCursor(top_pot,64);             
    display.println(top_txt);
  }

  display.fillRect(40, 106, 240, 30, ILI9341_NAVY);
  int mid_len = mid_txt.length();
  if(mid_len <= 10){
    int mid_pot = ((320-(mid_len*24))/2)+2;
    display.setCursor(mid_pot,107);             
    display.println(mid_txt);
  }

  display.fillRect(40, 145, 240, 47, ILI9341_NAVY);
  int bot_len = bot_txt.length();
  if(bot_len <= 10){
    int bot_pot = ((320-(bot_len*24))/2)+2;
    display.setCursor(bot_pot,147);             
    display.println(bot_txt);
  }
}

String stringSpliter(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
 
  for(int i=0; i<=maxIndex & found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  } 
 
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
  display.begin();
  display.setRotation(1);
  display.fillScreen(ILI9341_NAVY);
  setUpText("initializing");
  Serial.print("Waiting for serial data");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000){
    if(Serial2.available()){
        String dataIN = Serial2.readStringUntil('!');
        if(dataIN == "DIS#CON"){
          setGoodConnection();
        }
        break;
    }
  }
}

String dataIN;
void loop() {
  // put your main code here, to run repeatedly:
  if(Serial2.available() > 0){
    dataIN = Serial2.readStringUntil('!');
    Serial.println("----------------------");
    Serial.print("STRING DATA: ");
    Serial.println(dataIN);
    Serial.print("DATA LENGTH: ");
    Serial.println(dataIN.length());
    newConnectionStatus = stringSpliter(dataIN, '#', 1);
    newDeviceId = stringSpliter(dataIN, '#', 2);
    newDeviceMode = stringSpliter(dataIN, '#', 3);
    newPinLength = stringSpliter(dataIN, '#', 4);

    if((dataIN.substring(0,3) == "DIS") && (dataIN != oldPayload)){
      Serial.println("Prepare to update screan");
      if(newConnectionStatus == "CON"){
        // SETING CONNECTION STATUS
        if(newConnectionStatus != oldConnectionStatus){
          setGoodConnection();
        }

        // UPDATE DEVICE INFO
        if(newDeviceId != oldDeviceId){
          setDeviceId(stringSpliter(dataIN, '#', 2));
        }

        // WIRE PIN or MIDDLE SECTION
        if(newPinLength != oldPinLength){
          wirteMiddleSection(stringSpliter(dataIN, '#', 4).toInt());
        }

        // SETING TOP & BOTTOM SECTION
        if(newDeviceMode != oldDeviceMode){
          writeTopSection(stringSpliter(dataIN, '#', 3));
        }
      }else{
        setLossConnection();
      }
    }

    if((dataIN.substring(0,3) == "ALT") && (dataIN != oldPayload)){
      String info = stringSpliter(dataIN, '#', 1);
      writeAlert(stringSpliter(info, ' ', 0), stringSpliter(info, ' ', 1), stringSpliter(info, ' ', 2));
    }
    
    Serial.print("New Data: ");
    Serial.println(dataIN);
    Serial.print("Old Data: ");
    Serial.println(oldPayload);
    Serial.print("Compair: ");
    Serial.println(oldPayload != dataIN);
    oldDeviceId = newDeviceId;
    oldDeviceMode = newDeviceMode;
    oldPinLength = newPinLength;
    oldConnectionStatus = newConnectionStatus;
    oldPayload = dataIN;
    Serial2.flush();
  }
}