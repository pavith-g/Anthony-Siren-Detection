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

int last_triggered_time=0;
double frequency_sum=0;
int frequency_count=0;
int frequency_continuity =0;
float frequency;


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
  digitalWrite(P, LOW);
}

void one() {
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
  digitalWrite(P, LOW);
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
  digitalWrite(P, LOW);
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
  // Updating the 4 DIGIT DISPLAY
  value += 1;
  
  if (value > 9999){
    value = 0;
    Serial.print("Value overflow");
  }

  for (int i = 0; i < 4; i++){
    pickDigit(i);
    clearLEDs();
    digits[i] = (value % 10**(4-i)) / 10**(3-i);
    pickNumer(digits[i]);
    delay(del);
  }
  
  if (FreqMeasure.available()) {
    // Calculate the average frequency over 100 loops
    frequency_sum += FreqMeasure.read();
    frequency_count += 1;
    
    if (frequency_count > 100) {
      frequency = FreqMeasure.countToFrequency(frequency_sum / frequency_count);
      Serial.println(frequency);
      frequency_sum = 0;
      frequency_count = 0;
    }
  }
  /*
  if (frequency>2400 && frequency<3300) {
    // IC's guess at siren frequency. Possibly use a long response to rule out car horns.
    frequency_continuity++;
    Serial.print("frequency_continuity -> ");
    Serial.println(frequency_continuity);
    frequency=0;
  }

  if (frequency_continuity >= 3){ // We have detected a siren
        // RELAY_UP = true
        // RELAY_DOWN = false
        Serial.println("LIGHT TURNED ON ");
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
    */
}
