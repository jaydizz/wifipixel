/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  Control a color gradient on NeoPixel strip using a slider!

  For this example you need NeoPixel library:
    https://github.com/adafruit/Adafruit_NeoPixel

  App project setup:
    Slider widget (0...500) on V1
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

#define ONBUTTON V0
#define RAINBOWBUTTON V1
#define CHASEBUTTON V2
#define RGBBUTTON V3
#define DIMMER V4
#define SPEEDSLIDER V5


#define STATEFIX 0
#define STATERNB 1
#define STATERGB 2
#define STATECHASE 3
//#define DEBUG

char auth[] = "YourAuthToken";

volatile float waittime = 0.5;

volatile uint8_t is_on;
volatile uint8_t state;
volatile int chasecntr = 0;

volatile uint8_t r, g, b;
volatile uint8_t brightness = 255;

volatile uint8_t step = 0;
volatile boolean ticked = false;
uint8_t chaseColor = 0;
uint8_t rainbowColor = 0;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Heimnetz";
char pass[] = "4432372284899";

#define PIN D6
Ticker tick;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(86, PIN, NEO_GRB + NEO_KHZ800);

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

//Read Buttonvalues.

void on_ticked() {
    ticked = true;
    if(state ==  STATECHASE) {
        chasecntr++;
    }
#ifdef DEBUG
    Serial.println("ticked");
#endif
}

BLYNK_WRITE(ONBUTTON) {
    is_on = param.asInt();
    if(is_on) {
        strip.setBrightness(brightness);

    } else {
        strip.setBrightness(0);
    }
}

BLYNK_WRITE(RAINBOWBUTTON) {
    state = STATERNB;
    step = 0;
    ticked = false;
    tick.detach();
    tick.attach(0.5, on_ticked);
#ifdef DEBUG
    Serial.println("RNB Pressed");
#endif

}

BLYNK_WRITE(CHASEBUTTON) {
    state = STATECHASE;
    tick.detach();
    tick.attach(waittime,on_ticked);
}

BLYNK_WRITE(RGBBUTTON) {
    step = 0;
    tick.attach(waittime, on_ticked);
    state = STATERGB;
}

BLYNK_WRITE(V6) {
    r = param[0].asInt();
    g = param[1].asInt();
    b = param[2].asInt();
    state = STATEFIX;
#ifdef DEBUG
    Serial.print("STATEFIX");
#endif
    tick.detach();
}

BLYNK_WRITE(SPEEDSLIDER) {
    waittime = param.asFloat()/1023;
    tick.attach((waittime), on_ticked);
}

BLYNK_WRITE(DIMMER) {
    if(is_on){
        brightness = param.asInt();
        strip.setBrightness(brightness);
    }

}



uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



void colorwipe() {

    switch(chaseColor) {
        case 0:
            strip.setPixelColor(step, strip.Color(255,0,0));
            break;
        case 1:
            strip.setPixelColor(step, strip.Color(0,255,0));
            break;
        case 2:
            strip.setPixelColor(step, strip.Color(0,0,255));
            break;
        default:
            chaseColor = 0;
            break;
    }

    strip.show();
    step++;
    if(step > strip.numPixels()) {
        step = 0;
        chaseColor++;
    }
}

void rainbow() {
    for(uint8_t i=0; i< strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + rainbowColor) & 255));
    }
    strip.show();
}


void chase() {
    for(int i =0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(step));
    }
    strip.show();
}
void setup() {
    // Debug console
    Serial.begin(9600);

    Blynk.begin(auth, ssid, pass);
    // You can also specify server:
    //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
    //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

    strip.begin();
    strip.show();
}

void loop() {
    //strip.setBrightness(brightness);

    switch (state) {

        case STATEFIX:

            for (int i = 0; i < strip.numPixels(); i++) {
                strip.setPixelColor(i, r, g, b);

            }
            strip.show();
            break;
        case STATERGB:
            if(ticked) {
                colorwipe();
                Serial.println("wipe");
                ticked = false;
            }
        case STATERNB:
            if(ticked) {
                rainbowColor++;
                rainbow();
                Serial.println("Rainbow");
                ticked = false;
            }
            break;
        case STATECHASE:
            if(ticked && (chasecntr > 20)) {
                step++;
                chase();

                chasecntr = 0;
                Serial.println("CHASE");
            }
            ticked = false;
            break;
    }
    Blynk.run();
}

