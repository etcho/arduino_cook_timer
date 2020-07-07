#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397

int buzzer_pin = 6;
int ok_pin = 4;
int pot_pin = A5;

//variáveis para diminuir variação do potenciômetro
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;

int ok = HIGH;
int pot = 0;
long loops = 0;
int timer_minutes = 0;
int timer_seconds = 0;
bool running = false;
int ok_press_count = 0;
bool ignore_ok_press = false;

void setup(){
  pinMode(buzzer_pin, OUTPUT);
  pinMode(ok_pin, INPUT_PULLUP);
  pinMode(pot_pin, INPUT);

  display.begin();
  display.setContrast(55);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setTextSize(2);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop(){
  int i = 0;
  display.clearDisplay();

  ok = digitalRead(ok_pin);
  if (ok == LOW){
    if (!ignore_ok_press){
      ok_press_count++;
    }
    //tone(buzzer_pin, 900);
  } else {
    ok_press_count = 0;
    ignore_ok_press = false;
    //noTone(buzzer_pin);
  }

  if (ok_press_count > 20 && !running){
    running = true;
    timer_seconds = average * 60;
    ok_press_count = 0;
    ignore_ok_press = true;
  } else if (ok_press_count > 20 && running){
    ok_press_count = 0;
    running = false;
    ignore_ok_press = true;
  }

  if (running){
    if (timer_seconds % 2 == 0){
      display.drawLine(0, 8, 81, 8, BLACK);
      display.drawLine(0, 25, 81, 25, BLACK);
    }
    tick();
  } else {
    for (i=0; i<4; i++){
      display.drawLine(0, 4 + i, 81, 4 + i, BLACK);
      display.drawLine(0, 26 + i, 81, 26 + i, BLACK);
    }
  }

  pot = analogRead(pot_pin);
  if (pot < 700){
    pot = map(pot, 0, 700, 1, 60);
  } else {
    pot = map(pot, 700, 1023, 60, 183);
  }
  if (pot > 180){
    pot = 180;
  }
  total = total - readings[readIndex];
  readings[readIndex] = pot;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;
  if (!running){
    print_minutes(average);
  }
  
  loops++;
  if (loops == 2000000000){
    loops = 0;
  }
  delay(10);
}

void tick(){
  if (loops % 82 == 0){
    timer_seconds--;
    print_seconds(timer_seconds);
  }
  if (timer_seconds == 0){
    running = false;
    ring_the_bell();
  }
}

void ring_the_bell(){
  int i, j;
  for (i=0; i<5; i++){
    for (j=0; j<4; j++){
      play(2700, 50);
      stop(70);
    }
    stop(500);
  }
  /*for (i=0; i<2; i++){
    play(NOTE_C6, 300);
    stop(33);
    play(NOTE_C6, 100);
    stop(33);
    play(NOTE_C6, 100);
    stop(33);
    play(NOTE_D6, 300);
    stop(33);
    play(NOTE_C6, 300);
    stop(300);
    play(NOTE_E6, 300);
    stop(33);
    play(NOTE_F6, 300);
    stop(300);
  
    play(NOTE_C6, 300);
    stop(33);
    play(NOTE_C6, 100);
    stop(33);
    play(NOTE_C6, 100);
    stop(33);
    play(NOTE_D6, 300);
    stop(33);
    play(NOTE_C6, 300);
    stop(300);
    play(NOTE_A5, 300);
    stop(33);
    play(NOTE_F5, 300);
    stop(300);
  }*/
}

void play(int note, int d){
  tone(buzzer_pin, note);
  delay(d);
}

void stop(int d){
  noTone(buzzer_pin);
  delay(d);
}

void print_minutes(int min){
  int h, m = 0;
  if (min > 59){
    h = min / 60;
    m = min % 60;
  } else {
    h = 0;
    m = min;
  }
  print_clock(h, m, 0);
}

void print_seconds(int secs){
  int h = 0;
  int m = 0;
  int s = 0;
  if (secs >= 60 * 60){
    h = secs / 60;
    secs = secs % 60;
  }
  if (secs >= 60){
    m = secs / 60;
    s = secs % 60;
  } else {
    s = secs;
  }
  print_clock(h, m, s);
}

void print_clock(int h, int m, int s){
  int y = 10;
  if (h > 9){
    h = 9;
  }
  display.setTextSize(2);
  display.setCursor(0, y);
  if (h < 10){
    display.print(0);
  }
  display.print(h);
  display.setCursor(21, y);
  display.print(":");
  display.setCursor(30, y);
  if (m < 10){
    display.print(0);
  }
  display.print(m);
  display.setCursor(51, y);
  display.print(":");
  display.setCursor(60, y);
  if (s < 10){
    display.print(0);
  }
  display.print(s);
  display.display();
}
