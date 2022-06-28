/*Arduino Whitsle Detector Switch Program

 * Detects the whistles from pin 8 and toggles pin 13

 * Dated: 31-5-2019

 * Website: www.circuitdigest.com&nbsp;

 */
#include <FreqMeasure.h>  //https://github.com/PaulStoffregen/FreqMeasure

#define RELAY_UP_PIN 13
#define RELAY_DOWN_PIN 12

int last_triggered_time=0; //PG
double frequency_sum=0; //Moved here by I.C
int frequency_count=0; //Moved here by I.C
int frequency_continuity =0; //Moved here by I.C
float frequency; //Moved here by I.C

bool state = false; //Moved here by I.C


void setup() {
  Serial.begin(9600);
  FreqMeasure.begin(); //Measures on pin 8 by default
  pinMode(RELAY_UP_PIN, OUTPUT); 
  pinMode(RELAY_DOWN_PIN, OUTPUT);
}

void loop() {
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

  if (frequency>2400 && frequency<3300) {
    // IC's guess at siren frequency. Possibly use a long response to rule out car horns.
    frequency_continuity++;
    Serial.print("frequency_continuity -> ");
    Serial.println(frequency_continuity);
    frequency=0;
  }

  // PG
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
}
