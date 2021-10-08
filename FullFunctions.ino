
/* FullFunctions.ino */

/*--------------------------PRE-SETUP--------------------------*/
#include <Servo.h>         // note: Servo library disables PWM on pins 9 and 10
#include <NewPing.h>       // to include NewPing.h, go to Sketch --> Include Library --> Manage Libraries
#include <TimerFreeTone.h> // TimerFreeTone.h added manually from .zip file

#define SERVO_PIN   13   // signal pin for servo motor (neck)
#define SOUND_PIN    2   // signal pin for audio out
#define VOLUME      10   // volume level (1-10) for beeps using TimerFreeTone
#define TRIG_PIN    12   // trigger pin for NewPing
#define ECHO_PIN     4   // echo pin for NewPing
#define MAX_DIST   100   // maximum distance in centimeters for NewPing

#define M1         8     // constants for DC Motor pins
#define M2         9
#define M3         10
#define M4         11
#define ALOG1      5     // constants for analog output pins to control motor speed
#define ALOG2      6     //   (note: PWM only works on pins 3, 5, 6, 9, 10, and 11)
                         //   (also: Servo library disables PWM on 9 and 10, leaving 3, 5, 6, and 11)

int ping_dist = 0;       // variables for ping distance
int ping_right = 0;
int ping_left = 0;
int ping_center = 0;
int m_speed = 80;        // variable for motor speed
int neck_pos = 90;       // variable for servo position
int counter = 0;         // variable for counter tally

NewPing eyes(TRIG_PIN, ECHO_PIN, MAX_DIST);   // NewPing sets pinmodes and creates sonar object ("eyes")
Servo neck;                                   // create servo object ("neck")


/*____________________________SETUP__________________________________*/
void setup() {
  neck.attach(SERVO_PIN);         // attach neck servo object to specified pin (e.g. 13)

  pinMode(M1, OUTPUT);            // initialize dc Motor pins
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);
  pinMode(ALOG1, OUTPUT);         // initialize PWM pins
  pinMode(ALOG2, OUTPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);   // initialize LED_BUILTIN pin

  neck.write(90);  delay(200);    // move neck to 90 degree angle (straight forward)
  neck.write(105);  delay(150);  neck.write(90);    // overshoot then correct, makes it lifelike

  sp_creep();                                       // set motor speed to very slow
  go_forward(); delay(20); stop_moving();           // prime motors
  
  Serial.begin(9600);         Serial.print('\n');   // open Serial channel to use Serial.print
  Serial.print("Begin Test"); Serial.print('\n');   // to see output go to Tools --> Serial Monitor 
                              Serial.print('\n');   // or click on little magnifying glass at top
  delay(1000);                                      // right of window for Serial Monitor
  sp_med();
}

/*____________________________LOOP______________________________*/
void loop() {
  ping_it();
  if (ping_dist != 0 && ping_dist < 15) {
    stop_moving();
    Serial.println("OBSTACLE ");
    fast_talk();
    look_around();
  }
  sp_slow();
  go_forward();   delay(60);

  counter += 1;
  if (counter >= 2) {     // stop moving every so often
    stop_moving();
    delay(10);
    counter = 0;          // reset counter
  }

}


/*------------------------------FAST TALK---------------------------------*/
void fast_talk() {
  Serial.print('\n');  Serial.println("fast_talk");  Serial.print('\n');
  for (int i = 1; i <= 6; i++) {                                  // loop 6 times
    TimerFreeTone(SOUND_PIN, rand() % 1000 + 800, rand() % 50 + 40, VOLUME);
    delay(rand() % 50 + 25);
  }
  TimerFreeTone(SOUND_PIN, rand() % 800 + 600, rand() % 100 + 50, VOLUME);
  delay(rand() % 100 + 50);
  TimerFreeTone(SOUND_PIN, rand() % 800 + 600, rand() % 100 + 50, VOLUME);
  delay(rand() % 100 + 50);
  delay(100);
}

/*------------------------ SAY LEFT -------------------------*/
void say_left() {
  Serial.print('\n');  Serial.println("say_left");  Serial.print('\n');
  TimerFreeTone(SOUND_PIN, 1000 + i, 80, VOLUME);
  delay(50);
  for (int i = 1; i <= 20; i++) {
    TimerFreeTone(SOUND_PIN, 1000 - i, 20, VOLUME);
  }
  delay(50);
}

/*------------------------ SAY RIGHT -------------------------*/
void say_right() {
  Serial.print('\n');  Serial.println("say_right");  Serial.print('\n');
  Serial.print('\n');  Serial.println("say_left");  Serial.print('\n');
  TimerFreeTone(SOUND_PIN, 1000, 80, VOLUME);
  delay(50);
  for (int i = 1; i <= 20; i++) {
    TimerFreeTone(SOUND_PIN, 1000 + i, 20, VOLUME);
  }
  delay(50);
  
}


/*------------------------------PING_IT---------------------------------*/
void ping_it() {
  ping_dist = eyes.ping_cm();                                    // __.ping_cm() is a NewPing function to return distance
  Serial.print(ping_dist);     Serial.print(" cm -- ");          // output ping distance to Serial Monitor
  neck_pos = neck.read();
  if   (neck_pos <= 70) {                            // three ping variables for looking around
    ping_right   = ping_dist;  Serial.println(" = ping_right   ");
  }
  if   (neck_pos >= 120) {
    ping_left  = ping_dist;    Serial.println(" = ping_left   ");
  }
  if ( (neck_pos > 70) && (neck_pos < 120) )  {  
    ping_center = ping_dist;   Serial.println(" = ping_center   ");
  }
  delay(200);
}

/*------------------------------LOOK_AROUND---------------------------------*/
void look_around() {  delay(100);
  neck.write(50);     delay(100);
  neck.write(60);     delay(50);
  neck.write(55);     delay(100);    // move neck to 55 degrees
  ping_it();          delay(200);    // get ping

  neck.write(150);    delay(100);
  neck.write(140);    delay(50);    // move neck to 140 degrees
  ping_it();          delay(200);    // get ping
  
  neck.write(80);     delay(100);
  neck.write(90);     delay(50);    // move neck to 90 degrees
  ping_it();          delay(200);    // get ping

  decide_direction();
}


/*------------------------------DECIDE DIRECTION---------------------------------*/
void decide_direction() {
  if ( ping_right == 0 )  { ping_right = 400; }
  if ( ping_left  == 0 )  { ping_left  = 400; }
  if ( ping_right > ping_left ) { say_right(); }
  else { say_left(); }

}


/*------------------------------GO LEFT---------------------------------*/
void go_left() {
  Serial.println("   go_left ");
  sp_med();       delay(200);
  hard_left();    delay(600);
  stop_moving();  delay(200);
}

/*------------------------------GO RIGHT---------------------------------*/
void go_right() {
  Serial.println("   go_right ");
  sp_med();       delay(200);
  hard_right();   delay(600);
  stop_moving();  delay(200);
}

/*------CHANGE_SPEEDS------*/
void sp_creep() {  m_speed = 80;   analogWrite(ALOG1, m_speed);  analogWrite(ALOG2, m_speed);  }
void sp_slow()  {  m_speed = 90;   analogWrite(ALOG1, m_speed);  analogWrite(ALOG2, m_speed);  }
void sp_med()   {  m_speed = 100;  analogWrite(ALOG1, m_speed);  analogWrite(ALOG2, m_speed);  }
void sp_fast()  {  m_speed = 120;  analogWrite(ALOG1, m_speed);  analogWrite(ALOG2, m_speed);  }


/*-------------------------BASIC_MOVEMENTS---------------------------------*/
void stop_moving() {
  digitalWrite(M1, LOW);   digitalWrite(M2, LOW);           // left stop
  digitalWrite(M3, LOW);   digitalWrite(M4, LOW);      }    // right stop
void go_forward() {
  digitalWrite(M1, HIGH);   digitalWrite(M2, LOW);          // left forward
  digitalWrite(M3, HIGH);   digitalWrite(M4, LOW);     }    // right forward
void go_backward() {
  digitalWrite(M1, LOW);   digitalWrite(M2, HIGH);          // left reverse
  digitalWrite(M3, LOW);   digitalWrite(M4, HIGH);     }    // right reverse
void turn_left() {
  digitalWrite(M1, LOW);   digitalWrite(M2, LOW);           // left stop
  digitalWrite(M3, HIGH);  digitalWrite(M4, LOW);      }    // right forward
void turn_right() {
  digitalWrite(M1, HIGH);   digitalWrite(M2, LOW);          // left forward
  digitalWrite(M3, LOW);    digitalWrite(M4, LOW);     }    // right stop
void hard_left() {
  digitalWrite(M1, LOW);   digitalWrite(M2, HIGH);          // left reverse
  digitalWrite(M3, HIGH);   digitalWrite(M4, LOW);     }    // right forward
void hard_right() {
  digitalWrite(M1, HIGH);   digitalWrite(M2, LOW);          // left forward
  digitalWrite(M3, LOW);   digitalWrite(M4, HIGH);     }    // right reverse


/*------------------------------SCOUTING---------------------------------*/
void scouting() {               delay(200);
  sp_creep();   go_backward();  delay(200);  stop_moving();  delay(100);
  sp_slow();    go_forward();   delay(400);  stop_moving();  delay(800);

//                look_around();  delay(300);

  sp_creep();   go_backward();  delay(200);  stop_moving();  delay(200);  sp_med();
                go_forward();   delay(200);  sp_slow();      delay(100);  sp_creep();    delay(400);
                stop_moving();  delay(800);
  
}

/*------------------------------TURN_AROUND---------------------------------*/
void turn_around() {
  sp_slow();                   delay(200);
               hard_left();    delay(400);
               stop_moving();  delay(200);
               hard_right();   delay(500);
  sp_creep();  hard_left();    delay(200);
               stop_moving();  delay(500);
}

/*--------------------------------CRAWL-------------------------------------*/
void crawl() {
  sp_creep();  go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
               go_forward();  delay(50); stop_moving();  delay(50); 
}




/*      by cosmos      */

