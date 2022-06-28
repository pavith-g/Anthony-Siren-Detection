/*Arduino Whitsle Detector Switch Program

 * Detects the whistles from pin 8 and toggles pin 13

 * Dated: 31-5-2019

 * Website: www.circuitdigest.com&nbsp;

 */



#include <FreqMeasure.h>//https://github.com/PaulStoffregen/FreqMeasure

int last_triggered_time=0; //PG

double sum=0; //Moved here by I.C

int count=0; //Moved here by I.C

bool state = false; //Moved here by I.C

float frequency; //Moved here by I.C

int continuity =0; //Moved here by I.C




void setup() {

  Serial.begin(9600);

  FreqMeasure.begin(); //Measures on pin 8 by default

  pinMode(LED_BUILTIN, OUTPUT); //LED_BUILTIN is pin13 so same as RelayDown


}


void loop() {

  if (FreqMeasure.available()) {

    // average several reading together

    sum = sum + FreqMeasure.read();

    count = count + 1;

    if (count > 100) {

      frequency = FreqMeasure.countToFrequency(sum / count);

      Serial.println(frequency);

      sum = 0;

      count = 0;

    }

  }



    if (frequency>2400 && frequency<2700) // IC's guess at siren frequency. Possibly use a long response to rule out car horns.

        { continuity++; Serial.print("Continuity -> "); Serial.println(continuity); frequency=0;}


     // PG
    if (continuity >= 3){ // We have detected a siren

        // If we hear a siren for more than 3 counts (CONTINUITY), then we turn LED ON
        // THEN wait 15 seconds regardless of sensors.
        state = true;
        Serial.println("LIGHT TURNED ON ");
        digitalWrite(LED_BUILTIN, state);
        // We have heard a siren. The LED is turned on. Now wait 15 seconds.
        delay(15000);
        // Reset the continuity to 0 -> reset the counter and recheck for frequencies.
        continuity = 0;

        // Turn off LED and wait "5" minutes
        state = false;
        digitalWrite(LED_BUILTIN, state);
        delay(300000);
    }
    else // No siren detected
    {
      state = false;
      digitalWrite(LED_BUILTIN, state);
    }

  /* IC
    if (continuity >=3 && state==false)


      {if (millis()- last_triggered_time >= 1000){
       state = true;
        continuity=0;
        Serial.println("Light Turned ON");
        //delay(1000);
        last_triggered_time = millis();}}


    if (continuity >=3 && state==true)

       {if (millis()- last_triggered_time >= 15000){
        state = false;
         continuity=0;
        Serial.println("Light Turned OFF");
        //delay(1000);
        last_triggered_time = millis();}}
*/


}
