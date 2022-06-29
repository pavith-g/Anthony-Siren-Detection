/*Arduino Whitsle Detector Switch Program

 * Detects the whistles from pin 8 and toggles pin 13

 * Dated: 31-5-2019

 * Website: www.circuitdigest.com&nbsp;

 */
#include <FreqMeasure.h> //https://github.com/PaulStoffregen/FreqMeasure

#define RELAY_UP_PIN 13
#define RELAY_DOWN_PIN 12

// These pins refer to which digit on the display to update
#define D1 2
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
const int frequency_range[] = {0, 0}; // Use this to set the limits of the frequency range
int fake_clock[] = {0, 0}; // This is our time variable. First element is hours, second element is minutes. 
unsigned long last_clock_update = 0; // Will be used to update the clock. Every 1000 millis, incremenent 1 minute 
int del = 3; // Delay between updating display (ms)
int value = 2345; // This is the value that will be printed onto the display - The maxmimum value is 9999
int digits[] = {0, 0, 0, 0};

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
}

void loop() {
  // Update fake clock
  if (millis() - last_clock_update > 1000){
    // One second has passed
    fake_clock[1] += 1;
    last_clock_update = millis();
    
    if (fake_clock[1] > 59){
      fake_clock[0] += 1; 
      fake_clock[1] = 0;
    }
    if (fake_cock[0] > 23){
      fake_clock[0] = 0;
    }
    Serial.print(fake_clock[0]);
    Serial.print(":");
    Serial.println(fake_clock[1]);
    
  }
  
  // Siren Detection
  if (FreqMeasure.available()) {
    // Calculate the average frequency over 100 loops
    frequency_sum += FreqMeasure.read();
    frequency_count += 1;
    
    if (frequency_count > 100) {
      frequency = FreqMeasure.countToFrequency(frequency_sum / frequency_count);
      frequency_sum = 0;
      frequency_count = 0;
    }
  }

  if (frequency > frequency_range[0] && frequency < frequency_range[1]) {
    // IC's guess at siren frequency. Possibly use a long response to rule out car horns.
    //frequency_continuity++;
    frequency=0;
  }

  if (frequency_continuity >= 3){ // We have detected a siren
        Serial.println("SIREN DETECTED");
        // RELAY_UP = true
        // RELAY_DOWN = false
        // Pistons down
        digitalWrite(RELAY_UP_PIN, LOW);
        digitalWrite(RELAY_DOWN_PIN, HIGH);
        
        // We have heard a siren. RELAY_DOWN is HIGH. Now wait 15 seconds for the pistons to come down
        delay(15000);

        // Now wait "5 Minutes" while pistons are down
        delay(300000);
        // Reset the frequency_continuity to 0 -> reset the counter and recheck for frequencies.
        frequency_continuity = 0;

        // Set RELAY_UP to low and RELAY_DOWN to high to put pistons back up
        digitalWrite(RELAY_UP_PIN, HIGH);
        digitalWrite(RELAY_DOWN_PIN, LOW);
    }
    
  // Updating the 4 DIGIT DISPLAY
  
  value = fake_clock[0] * 100 + fake_clock[1];
  if (value > 9999){
    value = 0;
    Serial.print("Value overflow");
  }
  
  for (int i = 0; i < 4; i++){
    pickDigit(i);
    clearLEDs();
    digits[i] = (value % (int)pow(10,(4-i))) / pow(10,3-i);
    pickNumber(digits[i]);
    delay(del);
  }
  
  
  /* --- CODE FOR DISPLAYING EACH DIGIT --- 
  for (int i = 0; i < 4; i++){
    Serial.print(digits[i]);
  }
  Serial.println();
  */
}
