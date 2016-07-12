#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define PIN 2
#define LED_COUNT 8

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "Test123";
const char* password = "{w<To20*-]}[D1(4mv,X";

//const char* ssid     = "HHGTTG";
//const char* password = "i like security on my network";

ESP8266WebServer server(80);

int _white[3] = {255,255,255};
int _red[3] = {255,0,0};
int _green[3] = {0,255,0};
int _blue[3] = {0,0,255};
int _amber[3] = {255,100,0};

int _solid[1][LED_COUNT] = {
  {100, 100, 100, 100, 100, 100, 100, 100 }
};

int _busy[14][LED_COUNT] = {
  {40, 0, 0, 0, 0, 0, 0, 0},
  {20, 40, 0, 0, 0, 0, 0, 0},
  {10, 20, 40, 0, 0, 0, 0, 0},
  {0, 10, 20, 40, 0, 0, 0, 0},
  {0, 0, 10, 20, 40, 0, 0, 0},
  {0, 0 ,0, 10, 20, 40, 0, 0},
  {0, 0, 0 , 0, 10, 20, 40, 0},
  {0, 0, 0,  0 ,0, 10, 20, 40},
  {0, 0, 0 , 0, 0, 0, 40, 20},
  {0, 0, 0 , 0, 0, 40, 20, 10},
  {0, 0, 0 , 0, 40, 20, 10, 0 },
  {0, 0, 0 , 40, 20, 10, 0, 0 },
  {0, 0, 40, 20, 10, 0, 0, 0 },
  {0, 40, 20, 10, 0, 0, 0, 0 }
};
    
int _status[2][LED_COUNT] = {
    {100, 0, 100, 0, 100, 0, 100, 0},
    {0, 100, 0, 100, 0, 100, 0, 100}};

int _pos = 0;
int (*_pattern)[LED_COUNT];
int _pattern_len = 1;
int *_color;
int *_last_status_color = NULL;
int _brightness;
int (*_last_pattern)[LED_COUNT];

int _delay = 250;

int _since_last_check = 0;
int _monitor = 0;
int _checking_status = 0;
String _monitor_url = "http://31.210.241.166/view/Build%20Monitor/api/json";

void setBusy(int *color, int brightness){
  _pattern = _busy;
  _pattern_len = 14;
  _color = color;
  _brightness = brightness;
  if(_pattern != _last_pattern) {
    _last_pattern = _pattern;
    _pos = 0;
  }
}

void setStatus(int *color, int brightness){
  _last_status_color = color;
  _pattern = _status;
  _pattern_len = 2;
  _color = color;
  _brightness = brightness;
  if(_pattern != _last_pattern) {
    _last_pattern = _pattern;
    _pos = 0;
  }
}

void setSolid(int *color, int brightness) {
  _last_status_color = color;
  _pattern = _solid;
  _pattern_len = 1;
  _color = color;
  _brightness = brightness;
  if(_pattern != _last_pattern) {
    _last_pattern = _pattern;
    _pos = 0;
  }
}

void setFailed(){
  Serial.println("set failed ===>");
  _delay = 500;
  setStatus(_red, 20);
}

void setSuccess(){
  Serial.println("set success ===>");
  _delay = 500;
  setStatus(_green, 5);
}

void setUnstable(){
  Serial.println("set unstable ===>");
  _delay = 500;
  setStatus(_amber, 10);
}

void setBuilding(int *color){
  Serial.println("set building ===>");
  _delay = 250;
  setBusy(color, 20);
}



void startMonitoring(){
  _delay = 1000;
  _monitor = 1;
}


void rotatePattern() {
  if(_pos >= _pattern_len) {
    _pos = 0;
  }
  int* row = _pattern[_pos];
  clearPixels();

  for (int i = 0; i < LED_COUNT; i++)
  {
      int r = _color[0] * (_brightness / 100.0f) * (row[i] / 100.0f);
      int g = _color[1] * (_brightness / 100.0f) * (row[i] / 100.0f);
      int b = _color[2] * (_brightness / 100.0f) * (row[i] / 100.0f);
      leds.setPixelColor(i, r, g, b, 0);
  }
  leds.show();
  _pos++;
}

void setup() {

    Serial.begin(115200);
    delay(100);

    leds.begin();

    setBusy(_white, 20);
    rotatePattern();
   
    // We start by connecting to a WiFi network
   
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(400);
      rotatePattern();
      Serial.print(".");
    }
   
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", [](){
        server.send(200, "text/html", "OK");
    });

    server.on("/success", [](){
      _monitor = 0;
      setSuccess();
      server.send(200, "text/html", "OK");
    });

    server.on("/building", [](){
      _monitor = 0;
      setBuilding(_last_status_color);
      server.send(200, "text/html", "OK");
    });

    server.on("/unstable", [](){
      _monitor = 0;
      setUnstable();
      server.send(200, "text/html", "OK");
    });

    server.on("/failed", [](){
      _monitor = 0;
      setFailed();
      server.send(200, "text/html", "OK");
    });
    server.on("/monitor", [](){
      String url = server.arg("url");
      if(url != NULL) {
        _monitor_url = url;
      }
      startMonitoring();
      server.send(200, "text/html", "OK");
    });

    server.on("/rgb", [](){
      String r = server.arg("r");
      if(r == NULL) {
        r = "0";
      }
      String g = server.arg("g");
      if(g == NULL) {
        g = "0";
      }
      String b = server.arg("b");
      if(b == NULL) {
        b = "0";
      }
      String i = server.arg("i");
      if(i == NULL) {
        i = "100";
      }
      Serial.printf("r=%d g=%d b=%d i=%d\n", atoi(r.c_str()), atoi(g.c_str()), atoi(b.c_str()), atoi(i.c_str()));
      int col[3] = {atoi(r.c_str()), atoi(g.c_str()), atoi(b.c_str())};
      _monitor = 0;
      setSolid(col, atoi(i.c_str()));
      
      server.send(200, "text/html", "OK");
    });
    
    server.begin();

    startMonitoring();
}

void checkBuildStatus() {
  if(_monitor == 1) {

    if(_since_last_check < 2000) {
      Serial.print(".");
      _since_last_check += _delay;
      return;
    }

    _since_last_check = 0;
    
    HTTPClient http;
    _checking_status = 1;
    http.begin(_monitor_url); //HTTP

    Serial.printf("[HTTP] GET... %s\n", _monitor_url.c_str());
    
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
            //"color":"aborted"
            //"color":"yellow"
            //"color":"red"
            //"color":"yellow_anime"

            bool has_failed = payload.indexOf("\"color\":\"red\"") > -1;
            bool has_unstable = payload.indexOf("\"color\":\"yellow\"") > -1 || payload.indexOf("\"color\":\"aborted\"") > -1;
            int *building_color = NULL;

            if(has_failed || payload.indexOf("\"color\":\"red_anime\"") > -1) {
              building_color = _red;
            }
            else if(has_unstable || payload.indexOf("\"color\":\"yellow_anime\"") > -1 || payload.indexOf("\"color\":\"aborted_anime\"") > -1) {
              building_color = _amber;
            }
            else {
              building_color = _green;
            }

            if(payload.indexOf("_anime\"") > -1) {
              setBuilding(building_color);
            }
            else if(has_failed) {
              setFailed();
            }
            else if(has_unstable) {
              setUnstable();
            }
            else {
              setSuccess();
            }
            
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
  server.handleClient();

  checkBuildStatus();

  rotatePattern();
  
  delay(_delay);
}

void clearPixels()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds.setPixelColor(i, 0, 0, 0, 0);
    }

    leds.show();
}


