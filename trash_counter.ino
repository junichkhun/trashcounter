#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 3
#define TRIG_1 9
#define ECHO_1 10
#define TRIG_2 11
#define ECHO_2 12
#define TRIG_3 4
#define ECHO_3 5
#define TRIG_4 6
#define ECHO_4 7

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 4, 1, PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_RGB + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };


String strToDisplay;
int len_str;
int buzzerPin = 2;
int buzzer_timer;

int object_detect[4] = {0,0,0,0};
int object_count[4] = {0,0,0,0};
String object_display[4] = {"vinyl=", "glass=", "can=", "plastic="};
int detect_displaying_timer = 0;
int distance[4] = {100,100,100,100};
int display_reset = 0;

void detect_process() {
  for(int i = 0; i < 4; i++) {
    if(object_detect[i] == 0) { //물체 인식 중이 아닐 때
      if(distance[i]<10) {
        object_detect[i] = 1; //물체 인식됨
        buzzer_timer = 10;
        object_count[i] += 1;
      }
    }
    else{ //물체 인식 중일 때
      if(distance[i] > 20) {
        object_detect[i] = 0;
      }
    }
  }
}

void display_process() {
  if(detect_displaying_timer > 0) {
    detect_displaying_timer --;
    if(detect_displaying_timer == 0) {
      display_reset = 1;
    }
  }
  else if(object_detect[0] == 0 && object_detect[1] == 0 && object_detect[2] == 0 && object_detect[3] == 0) {
    strToDisplay = "V:" + String(object_count[0]) + " G:" + String(object_count[1]) + " C:" + String(object_count[2]) + " P:" + String(object_count[3]);
  }
  else{
    for(int i = 0; i < 4; i++) {
      if(object_detect[i] == 1) { //물체 인식
        strToDisplay = object_display[i] + String(object_count[i]);
        detect_displaying_timer = 70;
        display_reset = 1;
      }
    }
  }
}

void buzzer_process() {
  if(buzzer_timer <= 0) {
    digitalWrite(buzzerPin, LOW);
  }
  else {
    digitalWrite(buzzerPin, HIGH);
    buzzer_timer--;
  }
}

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(15);
  matrix.setTextColor(colors[0]);
  pinMode(TRIG_1, OUTPUT);
  pinMode(ECHO_1,INPUT);
  pinMode(TRIG_2, OUTPUT);
  pinMode(ECHO_2, INPUT);
  pinMode(TRIG_3, OUTPUT);
  pinMode(ECHO_3, INPUT);
  pinMode(TRIG_4, OUTPUT);
  pinMode(ECHO_4, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

int x = matrix.width();
int pass = 0;

void loop() {
  digitalWrite(TRIG_1,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_1,LOW);
  distance[0] = pulseIn(ECHO_1, HIGH)/56;

  digitalWrite(TRIG_2,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_2,LOW);
  distance[1] = pulseIn(ECHO_2, HIGH)/56;
  
  digitalWrite(TRIG_3,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_3,LOW);
  distance[2] = pulseIn(ECHO_3, HIGH)/56;

  digitalWrite(TRIG_4,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_4,LOW);
  distance[3] = pulseIn(ECHO_4, HIGH)/56;

  detect_process();
  display_process();
  buzzer_process();

  len_str = strToDisplay.length()*9;

  if(display_reset == 1) {
    display_reset = 0;
    x = matrix.width();
  }

  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(strToDisplay);
  if(--x < -len_str) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(90);
}
