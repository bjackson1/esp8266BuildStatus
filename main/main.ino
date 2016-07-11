#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define PIN 2
#define LED_COUNT 8

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "Test123";
const char* password = "<password>";
ESP8266WebServer server(80);

int pos = 0;
int status = 0;

int building[20][8] = {
    {21, 13, 7, 4, 3, 2, 1, 0},
    {13, 7, 4, 3, 2, 1, 0, 0},
    {7, 4, 3, 2, 1, 0, 0, 0},
    {21, 3, 2, 1, 0, 0, 0, 0},
    {13, 21, 1, 0, 0, 0, 0, 0},
    {7, 13, 21, 0, 0, 0, 0, 0},
    {4, 7, 13, 21, 0, 0, 0, 0},
    {3, 4, 7, 13, 21, 0, 0, 0},
    {2, 3, 4, 7, 13, 21, 0, 0},
    {1, 2, 3, 4, 7, 13, 21, 0},
    {0, 1, 2, 3, 4, 7, 13, 21},
    {0, 0, 1, 2, 3, 4, 7, 13},
    {0, 0, 0, 1, 2, 3, 4, 7},
    {0, 0, 0, 0, 1, 2, 3, 21},
    {0, 0, 0, 0, 0, 1, 21, 13},
    {0, 0, 0, 0, 0, 21, 13, 7},
    {0, 0, 0, 0, 21, 13, 7, 4},
    {0, 0, 0, 21, 13, 7, 4, 3},
    {0, 0, 21, 13, 7, 4, 3, 2},
    {0, 21, 13, 7, 4, 3, 2, 1}};
    
int failed[2][8] = {
    {255, 0, 255, 0, 255, 0, 255, 0},
    {0, 255, 0, 255, 0, 255, 0, 255}};

int red, green, blue;

void setup() {

    Serial.begin(115200);
    delay(100);
   
    // We start by connecting to a WiFi network
   
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
   
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", [](){
        server.send(200, "text/html", "OK");
    });

    server.on("/ok", [](){
        server.send(200, "text/html", "OK");
        clearPixels();
        status = 0;
    });

    server.on("/building", [](){
        server.send(200, "text/html", "OK");
        clearPixels();
        status = 1;
    });

    server.on("/failed", [](){
        server.send(200, "text/html", "OK");
        clearPixels();
        status = 2;
    });

    server.on("/rgb", [](){
        server.send(200, "text/html", "OK");
        clearPixels();
        status = 3;
        red = server.arg("red").toInt();
        green = server.arg("green").toInt();
        blue = server.arg("blue").toInt();
    });

    server.begin();
  
    leds.begin();

    int brightness = 40;

    //leds.setPixelColor(0, 80, 80, 80);

    for (int i = 0; i <= 6; i++)
    {
        leds.setPixelColor((i * 5) + 0, brightness, 0, 0);
        leds.setPixelColor((i * 5) + 1, 0, brightness, 0);
        leds.setPixelColor((i * 5) + 2, 0, 0, brightness);
        leds.setPixelColor((i * 5) + 3, brightness, brightness, brightness);
        leds.setPixelColor((i * 5) + 4, 0, 0, 0);
    }

    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 255, 0, 0);
        delay(20);
        leds.show();
    }

    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 0, 255, 0);
        delay(100);
        leds.show();
    }
    
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 0, 0, 255);
        delay(100);
        leds.show();
    }
    
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 0, 0, 0);
        delay(100);
        leds.show();
    }
}

void loop() {
  // put your main code here, to run repeatedly:

    server.handleClient();

    if (status == 0)
    {
        for (int i = 0; i < 8; i++)
        {
          red = random(255);
          green = random(255);
          blue = random(255);
          leds.setPixelColor(i, red, green, blue);
        }

        delay(100);
    }    
    else if (status == 1)
    {
        for (int i = 0; i < 8; i++)
        {
            leds.setPixelColor(i, building[pos][i] * 5, 0, building[pos][i] / 2, 0);
        }
    
        pos++;
    
        if (pos == 20)
            pos = 0;
    }
    else if (status == 2)
    {
        for (int i = 0; i < 8; i++)
        {
            leds.setPixelColor(i, failed[pos][i], 0, 0, 0);
        }
    
        pos++;
    
        if (pos >= 2)
            pos = 0;

        delay(400);
    }
    else if (status == 3)
    {
        for (int i = 0; i < 8; i++)
        {
            leds.setPixelColor(i, red, green, blue, 0);
        }
    }
    else
    {
        clearPixels();
    }

    leds.show();
    delay(80);
}

void clearPixels()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 0, 0, 0);
    }

    leds.show();
}


