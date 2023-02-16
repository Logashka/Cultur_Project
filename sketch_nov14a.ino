#include <EEPROM.h>
#include <NewPing.h>
#include <Servo.h>
#include <AmperkaStepper.h>
#include <LiquidCrystal.h>


//header
#define TRIG_PIN A3
#define ECHO_PIN A2

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 3

#define UP_SERVO 2

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
AmperkaStepper motor(400);
Servo up_servo;
NewPing sonar(TRIG_PIN, ECHO_PIN, ULTRASONIC_AREA);


//main class
class project {
//setups
private:
    String a[8] = {"informtion", "all information", "21 century", "20 century", "19 century", "18 century",
                   "17 century", "16 century"};
    int i = 0;
public:
    //read from serial port
    byte pos; 
    void setup(byte a){
        pos = a;
        up_servo.write(SERVO_NULL);
    }
    void s_read() {
        char c = Serial.read();
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
    }

    //print in serial port(monitor)
    void s_print() {
        Serial.println("Your choice:" + a[i]);
    }

    //reindexing
    void reind() {
        if (i < 0) {
            i = 7 - abs(i) % 8;
        } else {
            i = i % 8;
        }

    }

    //edit index   
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
    }

    //update picture on lcd-display
    void disp_update() {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hi! Your choice:");
        lcd.setCursor(0, 1);
        lcd.print(a[i]);
        delay(500);
    }

    //show a qr-code
    void stepper_update(){
        motor.step(50 * (i - pos), WAVE_DRIVE);
        pos = i;
        EEPROM.update(EE_ADDRESS, i);
        Serial.println("err");
        up_servo.write(90);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("press button");
        lcd.setCursor(0, 1);
        lcd.print("to continue.");
        while(digitalRead(JOY_SW)){
            up_servo.write(90);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hi! Your choice:");  
        lcd.setCursor(0, 1);
        lcd.print(a[i]);
        up_servo.write(SERVO_NULL);
        delay(300);
        
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

        proj.reind();
        proj.disp_update();
    } else if (abs(now_y - JOY_NULL) > 200) {
        if (now_y < JOY_NULL) {
            proj.r_read('l');
        } else {
            proj.r_read('r');
        }
        proj.reind();
        proj.disp_update();
    }
    if (Serial.available()) {
        c = Serial.read();
    }
    if (c == 'p' or digitalRead(JOY_SW) == 0) {
        proj.stepper_update();
    }
}

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    proj.disp_update();
    motor.setSpeed(30);
    byte a;
    EEPROM.get(EE_ADDRESS, a);
    proj.setup(a);
    Serial.println(a);
    up_servo.attach(UP_SERVO);

    pinMode(JOY_X, INPUT);
    pinMode(JOY_Y, INPUT);
    pinMode(JOY_SW, INPUT_PULLUP);
}

void loop() {
    while(sonar.ping_cm() < 35){
        joys_control();
        Serial.println(digitalRead(JOY_SW));
    }
    
    //Serial.println(digitalRead(JOY_SW));
}
