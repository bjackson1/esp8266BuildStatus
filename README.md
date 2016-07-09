# Overview

The utility runs on the ESP8266 using the Arduino core.  It supports four states to indicate three potential build states (OK, Building, Failed), plus an additional option to set all LEDs to a specific RGB value.

Communication with the device is via WiFi and a very basic web server, accepting the following paths:
 - /ok - Show OK status
 - /building - Show Building status
 - /failed - Show Failed status
 - /rgb?red=<0-255>&green=<0-255>&blue=<0-255> - Set all LEDs to the specified RGB values

 The LEDs are controlled via the Adafruit Neopixel library, and expected to be WS2812B or similar variant

