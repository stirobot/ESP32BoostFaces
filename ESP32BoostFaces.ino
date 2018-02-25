//128*64 (double height of brz gauge)
//i2c interface
//not sure what to use wifi for (webserve it with wifi direct webpage?)

//this uses the squix ssd1306 library...it is faster and has more functions
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"
#include "SSD1306Brzo.h"
// Include the UI lib
#include "OLEDDisplayUi.h"

// Include custom images
#include "images.h"
int atmo = 0;
SSD1306  display(0x3c, 4, 15); //for the heltec with the built in oled...SSD1306  display(0x3c, D3, D5); for normal
int rBoost = 0;
bool isInverted = false;

void setup() {
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 to high
  //Wire.begin(4,15);

  display.init();

  display.flipScreenVertically(); //usb to the left
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  //ui.setTargetFPS(30);
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);

  //show logo
  //display.drawFastImage(59, 21, splash_Logo_width, splash_Logo_height, splash_Logo_bits);
  //display.drawXbm(0,0, splash_Logo_width, splash_Logo_height, splash_Logo_bits);
  //display.display();
  //delay(2000);
  //do the gauge sweep dance
  display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge0_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge1_width, gauge1_height, gauge1_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge2_width, gauge2_height, gauge2_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge3_width, gauge3_height, gauge3_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge4_width, gauge4_height, gauge4_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge5_width, gauge5_height, gauge5_bits);
  display.display();
  delay(300);
  display.clear();
  display.drawXbm(0, 0, gauge4_width, gauge4_height, gauge4_bits);
  display.display();
  delay(400);
  display.clear();
  display.drawXbm(0, 0, gauge3_width, gauge3_height, gauge3_bits);
  display.display();
  delay(500);
  display.clear();
  display.drawXbm(0, 0, gauge2_width, gauge2_height, gauge2_bits);
  display.display();
  delay(600);
  display.clear();
  display.drawXbm(0, 0, gauge1_width, gauge1_height, gauge1_bits);
  display.display();
  delay(600);
  display.clear();
  display.clear();
}

//TODO: on neg reading do psi to in/Hg and display units
//TODO: do the smiley face thing

void loop() {
  //I guess this lib XOR's everything because it doesn't seem to flicker with this horribly straightforward code
  //turbo logo
  //get boost value
  int boost = getBoost() - 14;  //1 atmosphere is roughtly 14.6 psi
  String Sboost = String(boost);
  //update boost value print at 90, 10
  if (boost >= 0 ) {
    display.drawString(120, 10, Sboost);
    display.drawString(125, 30, "psi");
  }
  else {
    Sboost = String(boost * 2);
    //psi to in/Hg
    display.drawString(115, 10, Sboost);
    display.drawString(125, 30, "Hg");
  }
  //select the right xbm for the number
  if ((boost > 16) && (isInverted == false)) {
    display.invertDisplay();
    isInverted = true;
  }
  if ((boost <= 16) && (isInverted == true)) {
    display.normalDisplay();
    isInverted = false;
  }
  if (boost < 0) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge0_bits);
  }
  if ((boost >= 0) && (boost < 3)) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge1_bits);
  }
  if ((boost >= 3) && (boost < 6)) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge2_bits);
  }
  if ((boost >= 6) && (boost < 9)) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge3_bits);
  }
  if ((boost >= 9) && (boost < 12)) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge4_bits);
  }
  if (boost > 12) {
    display.drawXbm(0, 0, gauge0_width, gauge0_height, gauge5_bits);
  }


  display.display();
  delay(20);
  display.clear();
}

int getBoost() {
  float rboost = analogRead(36);
  //float ResultPSI = (rboost*(.00488)/(.022)+20)/6.89 - atmo; //leave of the /6.89 for kpa
  float ResultPSI = (((rboost / 4095) + 0.04) / 0.004) * 0.145 - atmo; //by 0.145 to calc psi //4096 values on esp32
  /*rBoost = rBoost + 1 ;
    if (rBoost >= 20) {
    rBoost = 0;
    }*/
  return (ResultPSI);
}

float getOilPSI() {
  float psival = analogRead(A1);
  //Serial.print(psival);
  if (psival > 722) {
    return (0);
  }
  if (psival < 257) {
    return (110); //110 swings higher than the gauge goes which will alert the user to something being wrong
  }
  if ((psival <= 722) && (psival > 619)) { //722 = 1.42
    return 174.7 - (psival * 240) / 1000;
  }
  if ((psival <= 619) && (psival > 520)) {
    return 180.2 - (psival * 250) / 1000;
  }
  if ((psival <= 520) && (psival > 411)) {
    return 169.4 - (psival * 230) / 1000;
  }
  if ((psival <= 411) && (psival > 257)) { //257 = 100.68
    return 141.8 - (psival * 160) / 1000;
  }
}
