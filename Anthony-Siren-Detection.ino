/*Arduino Whitsle Detector Switch Program

 * Detects the whistles from pin 8 and toggles pin 13

 * Dated: 31-5-2019

 * Website: www.circuitdigest.com&nbsp;

 */
#include <FreqMeasure.h> //https://github.com/PaulStoffregen/FreqMeasure

#define RELAY_UP_PIN 13
#define RELAY_DOWN_PIN 12

// These pins refer to which digit on the display to update
#define D1 A5
#define D2 3
#define D3 4
#define D4 5

// These pins refer to which segment to activate
#define A 6
#define B 7
#define C 9
#define D 10
#define E 11
#define F A0
#define G A1
#define P A2


unsigned long last_siren_trigger = 0; // When was the last time that a siren was triggered? Use this to calculate when the 5 minute window has passed
double frequency_sum = 0;
int frequency_count = 0;
int frequency_continuity = 0;
float frequency;
const int frequency_range[] = {700, 2400}; // Use this to set the limits of the frequency range
int fake_time = 400; // Calculate fake time in minutes - convert to hours and minutes when needed
unsigned long last_clock_update = 0; // Will be used to update the clock. Every 1000 millis, incremenent 1 minute 
int del = 3; // Delay between updating display (ms)
int value = 0; // This is the value that will be printed onto the display - The maxmimum value is 9999
int digits[] = {0, 0, 0, 0};
bool bump = true; // Keep track of the speed bump position (true = up, false = down)
int last_actuation = fake_time; // When was the last time that the pistons were actuated due to a siren
bool siren_delay = false; // This variable tracks whether we are waiting the "5 minutes" after detecting a siren
bool relayUpState = true;
bool relayDownState = false;
int pistonActuationDelay = 15000; // Time for the pistons to actuate up and down
bool schoolZone = false; // Tracks whether current time is within school zone time


// --------- 4D7S Functions --------- //

void pickDigit(int x) { 
  // Pick which digit we want to update
  // Note that setting the digit high, means that it is not selected. Setting it low, means we select it. 
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  switch (x)
  {
    case 0:
      digitalWrite(D1, LOW);//Light d1 up
      break;
    case 1:
      digitalWrite(D2, LOW); //Light d2 up
      break;
    case 2:
      digitalWrite(D3, LOW); //Light d3 up
      break;
    default:
      digitalWrite(D4, LOW); //Light d4 up
      break;
  }
}

void clearLEDs() //clear the 7-segment display screen
{
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
  digitalWrite(P, LOW);
}

// --- Different combinations of segments for each number 0 - 9 ---- // 
void zero() {
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, LOW);
}

void one() {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void two()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
}
void three() 
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
}
void four() 
{
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void five()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void six()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void seven()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
}

void eight()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void nine()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
}

void pickNumber(int x){
  // Based on the digit we have, print it to the display
  switch (x)
  {
    default:
      zero();
      break;
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
  }
}


void setup() {
  Serial.begin(9600);
  FreqMeasure.begin(); //Measures on pin 8 by default
  
  last_clock_update = millis();
  
  // Set pin values on arduino
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(P, OUTPUT);
  pinMode(RELAY_UP_PIN, OUTPUT); 
  pinMode(RELAY_DOWN_PIN, OUTPUT);
  digitalWrite(RELAY_UP_PIN, relayUpState);
  digitalWrite(RELAY_DOWN_PIN, relayDownState);
}

void loop() {
  
  // Update fake clock
  if (millis() - last_clock_update >= 1000){
    // One second has passed since last update, so increment fake_time by 1 minute. 
    fake_time += 1;
    // Record when this update occured. 
    last_clock_update = millis();

    // Check if full day has passed
    if (fake_time >= 1440){
      //reset time to 0
      fake_time = 0;
    }

    // SCHOOL ZONE TIMES
    // We only need to check school zone times once the clock has been updated, not every time the Arduino loops.
    // Note that all time calculations are done in minutes
    // Actual school zone times: 8am - 9:30am and 2:30pm - 4pm
    // In minutes: 480 - 570 and 870 - 960

    if (fake_time <= 570 && fake_time >= 480){ 
      schoolZone = true;
    }
    else if (fake_time <= 960 && fake_time >= 870){
      schoolZone = true;
    }
    else {
      schoolZone = false;
    }
  }

  if (schoolZone == true && siren_delay == false && bump == false){
    // If school zone is true, and there is no siren_delay, and the speed bump isn't already up
    relayUpState = HIGH;
    relayDownState = LOW;
    digitalWrite(RELAY_UP_PIN, relayUpState);
    digitalWrite(RELAY_DOWN_PIN, relayDownState);
    bump = true;

    // During piston actuation, turn off the display
    for (int i = 0; i < 4; i++){
          clearLEDs();
    }
    // Wait "this amount of time" for the pistons to actuate. 
    delay(pistonActuationDelay);
  }
  if (schoolZone == false && bump == true && siren_delay == false){
    // If not in school zone, no siren delay, and the bump is up
    relayUpState = LOW;
    relayDownState = HIGH;
    digitalWrite(RELAY_UP_PIN, relayUpState);
    digitalWrite(RELAY_DOWN_PIN, relayDownState);
    bump = false;
    
    // During piston actuation, turn off the display
    for (int i = 0; i < 4; i++){
          clearLEDs();
    }
    // Wait "this amount of time" for the pistons to actuate. 
    delay(pistonActuationDelay);
  }

 
  // Siren Detection
  if (FreqMeasure.available()) {
    // Calculate the average frequency over 100 loops
    frequency_sum += FreqMeasure.read();
    frequency_count += 1;
    
    if (frequency_count > 20) {
      frequency = FreqMeasure.countToFrequency(frequency_sum / frequency_count);
      frequency_sum = 0;
      frequency_count = 0;
    }
  }

  if (frequency > frequency_range[0] && frequency < frequency_range[1]) {
    // IC's guess at siren frequency. Possibly use a long response to rule out car horns.
    frequency_continuity++;
    frequency = 0;
  }

  // Get time difference between last actuation 
  // If it has been more than 5 minutes and we have already have a siren (siren delay = true), then we can put the pistons back up
  if (fake_time - last_actuation > 5 && siren_delay == true && bump == false){
    // 5 minutes have passed since we detected the siren
    // Put the pistons back up
     
    // Set RELAY_UP to low and RELAY_DOWN to high to put pistons back up
    relayUpState = HIGH;
    relayDownState = LOW;
    digitalWrite(RELAY_UP_PIN, relayUpState);
    digitalWrite(RELAY_DOWN_PIN, relayDownState);
    siren_delay = false;
    bump = true;

    // During piston actuation, turn off the display
    for (int i = 0; i < 4; i++){
          clearLEDs();
    }
    // Wait "this amount of time" for the pistons to actuate. 
    delay(pistonActuationDelay);
  }
  
  // Get the fake time difference between last siren actuation 
  if (frequency_continuity >= 3 && bump == true && siren_delay == false){
     // We have detected a siren and the pistons are up
     Serial.println("SIREN DETECTED");

     // Pistons down
     relayUpState = LOW;
     relayDownState = HIGH;
     digitalWrite(RELAY_UP_PIN, relayUpState);
     digitalWrite(RELAY_DOWN_PIN, relayDownState);
     bump = false;
     siren_delay = true; // Siren has been detected, so now we start the delay of 5 minutes, before putting the pistson back up

     // Record when this actuation has occured
     last_actuation = fake_time;

     // Clear display while actuation occurs
     for (int i = 0; i < 4; i++){
       clearLEDs();
     }

     // Wait "this amount of time" for the pistons to actuate
     delay(pistonActuationDelay);
     
     // For now, because speed bump won't actually take 15 seconds to go up and down in "fake time",
     // (15 seconds in real time would be 15 minutes in fake time, which is unreasonable), 
     // I will just pause the fake clock while the 15 second window occurs, and resume once actuation has finished
        
     // Reset the frequency_continuity to 0 -> reset the counter and recheck for frequencies.
     frequency_continuity = 0;
  }
  
  // Updating the 4 DIGIT DISPLAY
  // Convert minutes to hours 
  // Because hours is an integer, dividing "fake_time" by 60 will give an integer back (the number of hours)
  int hours = fake_time / 60;
  
  // Fake_time % 60 will return the remainder (% means modulus - modulus 60 will return the remaining minutes)
  value = hours * 100 + fake_time % 60;
  Serial.println(value);

  
  // Update the display
  pickDigit(0);
  clearLEDs();
  pickNumber((value % 10000) / 1000); // Get the first digit and update
  digits[0] = (value % 10000) / 1000;
  delay(del);
  
  pickDigit(1);
  clearLEDs();
  pickNumber((value % 1000) / 100); // Get the second digit and update
  digits[1] = (value % 1000) / 100;
  delay(del);

  pickDigit(2);
  clearLEDs();
  pickNumber((value % 100) / 10); // Get the third digit and update
  digits[2] = ((value % 100) / 10);
  delay(del);

  pickDigit(3);
  clearLEDs();
  pickNumber((value % 10) / 1); // Get the fourth digit and update
  digits[3] = ((value % 10) / 1);
  delay(del);

}
