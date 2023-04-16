#include <LiquidCrystal.h>
#include <Servo.h>


//header
#define IR_PIN A5

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 3

#define UP_SERVO 7
#define WHEEL_SERVO 4

#define PIN_RS 12
#define PIN_EN 13
#define PIN_DB4 8
#define PIN_DB5 9
#define PIN_DB6 10
#define PIN_DB7 11

const int JOY_NULL = 500;
const int UP_SERVO_NULL = 0;
const int WHEEL_SERVO_NULL = 0;

LiquidCrystalRus lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
Servo wheel_servo;
Servo up_servo;


//main class
class project {
  //setups
private:
  String a[8] = { "architecture", "museums", "literature", "traditions", "theaters", "lectures",
                  "music", "about robot" };  // array with themes
  int i = 0, lan = 1;  
public:
  //controll robot from joystick
  void r_read(char c) {
    if (c == 'u') {
      i -= 1;
    }
    if (c == 'd') {
      i += 1;
    }
    if (c == 'r') {
      i += 2;
    }
    if (c == 'l') {
      i -= 2;
    }

    if (i < 0) {
      i = 7 - abs(i) % 8;
    } else {
      i = i % 8;
    }
  }

  //update picture on lcd-display
  void disp_update() {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Hi! Please, select");
    lcd.setCursor(1, 1);
    lcd.print("article of topic.");
    lcd.setCursor(4, 2);
    lcd.print("Your choice:");
    lcd.setCursor((20 - a[i].length()) / 2, 3);
    lcd.print(a[i]);
    delay(150);
  }

  void language(){
    String lans;
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Select language:");
    lcd.setCursor(4, 1);
    lcd.print("1.Русс.");
    lcd.setCursor(4, 2);
    lcd.print("2.Eng.");
    lcd.setCursor(0, 3);
    if(lan == 0){
      lans = "1";
    }
    else{
      lans = "2";
    }
    lcd.print("now:" + String(lans));
    while(digitalRead(JOY_SW)){
      int now_x = analogRead(JOY_X), now_y = analogRead(JOY_Y), now_sw = digitalRead(JOY_SW);
      bool change = false;
      if (abs(now_x - JOY_NULL) > 200) {
        if (now_x < JOY_NULL) {
          lan += 1;
        } else {
          lan -= 1;
        }
        change = true;
      }else if (abs(now_y - JOY_NULL) > 200) {
        if (now_y < JOY_NULL) {
          lan -= 1;
        } else {
          lan += 1;
        }
        change = true;
      }  
      delay(300);
      if(change){
        lan = abs(lan) % 2;   
        if(lan == 0){
          lans = "1";
        }
        else{
          lans = "2";
        }
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Select language:");
        lcd.setCursor(4, 1);
        lcd.print("1.Русский");
        lcd.setCursor(4, 2);
        lcd.print("2.English");
        lcd.setCursor(0, 3);
        lcd.print("now:" + lans);
        change = false;
      }
    }    
  }

  //show a qr-code
  void pos_update() {
    wheel_servo.write(24 * i);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Good choice!");
    lcd.setCursor(3, 1);
    lcd.print("Please, click");
    lcd.setCursor(4, 2);
    lcd.print("on joystick");
    lcd.setCursor(4, 3);
    lcd.print("to continue.");
    delay(1750);
    up_servo.write(53);
    while (digitalRead(JOY_SW)) {
    }
    lcd.clear(); 
    lcd.setCursor(1, 0);
    lcd.print("Hi! Please, select");
    lcd.setCursor(1, 1);
    lcd.print("article of topic.");
    lcd.setCursor(4, 2);
    lcd.print("Your choice:");
    lcd.setCursor((20 - a[i].length()) / 2, 3);
    lcd.print(a[i]);
    up_servo.write(0);
  }
};

//create global object
project proj;

//function, that control the joystik
void joys_control() {
  int now_x = analogRead(JOY_X), now_y = analogRead(JOY_Y), now_sw = digitalRead(JOY_SW);
  char c = ' ';

  if (abs(now_x - JOY_NULL) > 200) {
    if (now_x < JOY_NULL) {
      proj.r_read('u');
    } else {
      proj.r_read('d');
    }
    proj.disp_update();
  } else if (abs(now_y - JOY_NULL) > 200) {
    if (now_y < JOY_NULL) {
      proj.r_read('l');
    } else {
      proj.r_read('r');
    }
    proj.disp_update();
  }



  if (digitalRead(JOY_SW) == 0) {
    proj.pos_update();
    delay(200);
  }
}

bool infrared_dist(){
  Serial.println(analogRead(IR_PIN));
  if (analogRead(IR_PIN) > 300){
    return true;
  }
  else{
    return false;
  }
}

void setup() {
  Serial.begin(9600);  //setup serial port
  lcd.begin(20, 4);  // setup lcd display
  //proj.language();
  proj.disp_update();

  //setup infrared sensor
  pinMode(IR_PIN, INPUT);

  //setup motors
  up_servo.attach(UP_SERVO);
  wheel_servo.attach(WHEEL_SERVO);
  up_servo.write(UP_SERVO_NULL);
  wheel_servo.write(WHEEL_SERVO_NULL);

  //setup joystik
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);
}

void loop() {
  if (infrared_dist()){
    lcd.display();
  }
  else{
    lcd.noDisplay();
  }
  while (infrared_dist()) {
    joys_control();
  }
}
