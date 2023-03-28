#include <NewPing.h>
#include <Servo.h>
#include <LiquidCrystal.h>


//header
#define TRIG_PIN A3
#define ECHO_PIN A2

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 3

#define UP_SERVO 2
#define WHEEL_SERV0 4

#define EE_ADDRESS 0

#define PIN_RS 12
#define PIN_EN 13
#define PIN_DB4 8
#define PIN_DB5 9
#define PIN_DB6 10
#define PIN_DB7 11

const int JOY_NULL = 500;
const int ULTRASONIC_AREA = 400;
const int SERVO_NULL = 170;


LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_DB4, PIN_DB5, PIN_DB6, PIN_DB7);
Servo wheel_servo;
Servo up_servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, ULTRASONIC_AREA);


//main class
class project {
  //setups
private:
  String a[8] = { "architecture", "museums", "literature", "traditions", "theaters", "lectures",
                  "music", "about robot" };
  int i = 0;
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

  //show a qr-code
  void pos_update() {
    wheel_servo.write(360 / 8 * i);
    up_servo.write(90);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Good choice!");
    lcd.setCursor(3, 1);
    lcd.print("Please, click");
    lcd.setCursor(4, 2);
    lcd.print("on joystick");
    lcd.setCursor(4, 3);
    lcd.print("to continue.");
    delay(200);
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
    up_servo.write(SERVO_NULL);
  }
};

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

void setup() {
  Serial.begin(9600);

  lcd.begin(20, 4);

  proj.disp_update();

  up_servo.attach(UP_SERVO);
  wheel_servo.attach(WHEEL_SERV0);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  proj.disp_update();
}

void loop() {
  //TODO: buy new HC=SR 04
  //while (sonar.ping_cm() < 20) {

  //Serial.println(sonar.ping_cm());
  joys_control();
  //}

  //Serial.println(digitalRead(JOY_SW));
}
