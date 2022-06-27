
//My variables

int delayspeed = 666;
int state = 0;

//end my varables

const int CHANNELS = 6;
const int LIGHT_PINS[CHANNELS] = {3, 5, 6, 9, 10, 11};
const int LED_PINS[CHANNELS] = {8, 12, 13};
const int MIN_BRIGHTNESS[CHANNELS] = {0, 0, 0, 0, 0, 0};
const int MAX_BRIGHTNESS[CHANNELS] = {255, 255, 255, 255, 255, 255};

int CurrentBrightness[CHANNELS] =  {0, 51, 102, 153, 204, 250};
int FadeAmount[CHANNELS] = {1, 1, 1, 1, 1, 1};



//handel rotory start
#include <Arduino.h>
#include <RotaryEncoder.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 4
#define PIN_IN2 2

#endif

// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

// Define some constants.

// the maximum acceleration is 10 times.
constexpr float m = 10;

// at 200ms or slower, there should be no acceleration. (factor 1)
constexpr float longCutoff = 50;

// at 5 ms, we want to have maximum acceleration (factor m)
constexpr float shortCutoff = 5;

// To derive the calc. constants, compute as follows:
// On an x(ms) - y(factor) plane resolve a linear formular factor(ms) = a * ms + b;
// where  f(4)=10 and f(200)=1

constexpr float a = (m - 1) / (shortCutoff - longCutoff);
constexpr float b = 1 - longCutoff * a;

// a global variables to hold the last position
static int lastPos = 0;

//handel rotory end


//Handle button
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 7;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

//end button


// the setup routine runs once when you press reset:
void setup() {
  for (int i = 0; i < CHANNELS; i++) {
    pinMode(LIGHT_PINS[i], OUTPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP); //button


  //rotory encoder
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("AcceleratedRotator example for the RotaryEncoder library.");
  Serial.print("a=");
  Serial.println(a);
  Serial.print("b=");
  Serial.println(b);


  //button
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

}

// the loop routine runs over and over again forever:
void loop() {
  for (int i = 0; i < CHANNELS; i++) { // this for loop block updates halogens
    analogWrite(LIGHT_PINS[i], CurrentBrightness[i]);
    CurrentBrightness[i] += FadeAmount[i];

    // reverse the direction of the fading at the ends of the fade:
    if (CurrentBrightness[i] <= MIN_BRIGHTNESS[i] ||
        CurrentBrightness[i] >= MAX_BRIGHTNESS[i]) {
      FadeAmount[i] = -FadeAmount[i];
    }
  }

  // wait for 90 milliseconds to see the dimming effect
  delayMicroseconds(delayspeed);



  //enoder
  encoder.tick();

  int newPos = encoder.getPosition();
  if (lastPos != newPos) {

    // accelerate when there was a previous rotation in the same direction.

    unsigned long ms = encoder.getMillisBetweenRotations();

    if (ms < longCutoff) {
      // do some acceleration using factors a and b

      // limit to maximum acceleration
      if (ms < shortCutoff) {
        ms = shortCutoff;
      }

      float ticksActual_float = a * ms + b;
      Serial.print("  f= ");
      Serial.println(ticksActual_float);

      long deltaTicks = (long)ticksActual_float * (newPos - lastPos);
      Serial.print("  d= ");
      Serial.println(deltaTicks);

      newPos = newPos + deltaTicks;
      encoder.setPosition(newPos);
    }

    Serial.print(newPos);
    Serial.print("  ms: ");
    Serial.println(ms);
    lastPos = newPos;
  } // if

  delayspeed = newPos + 1000;

  //button
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        digitalWrite(LED_PINS[state], HIGH);
        state++;
      }
      else{
        digitalWrite(LED_PINS[state], LOW);
    }
    }
  }
  if (state == 4) {
    state = 0;
  }

  // set the LED:
  //digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  //button end




}


