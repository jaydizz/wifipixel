# wifipixel
This application allows you to control your neopixel-strips with your cellphone using Blynk

# What you need:

* ESP8266 (preferrably WeMos D1 Mini)
* Micro-USB cable
* Laptop
* Arduino IDE _or_ CLion and PlatformIO
* NeoPixel Strip (or any other strip with the ws2812b Controller)
* Blynk App on your Cellphone

# How-To compile

_If_ you use the CLion IDE with PlatformIO you can simply import the directory as a project.

_If_ you use the Arduino IDE copy the file from the source directory to a new directory und rename the .cpp file to .ino. Then you have to import the libraries in the lib folder to your Arduino Workspace. 

# How-To configure Blynk

TODO: Add picture

# What to do in the sourcecode?

Add your WiFi-Settings to the code (SSID and password). 
Add the Blynk-App-UID.
Compile.
Upload.
Done!
