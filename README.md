# Overview

The utility runs on the ESP8266 using the Arduino core.  It supports four states to indicate three potential build states (OK, Building, Failed), plus an additional option to set all LEDs to a specific RGB value.

Communication with the device is via WiFi and a very basic web server, accepting the following paths:
 - /success - Show OK status
 - /building - Show Building status
 - /failed - Show Failed status
 - /unstable - Show Unstable status
 - /monitor?url=http://your.jenkins.server/view/yourview/api/json - Change The Monitored Url
 - /rgb?r=<0-255>&g=<0-255>&b=<0-255>&i=<0-100> - Set all LEDs to the specified RGB values and intensity (%)

 The LEDs are controlled via the Adafruit Neopixel library, and expected to be WS2812B or similar variant

