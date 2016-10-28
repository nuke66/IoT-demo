/*  IoT Demo
 *    
 *  INTRO: 
 *  I have created this IoT demonstration as a portfolio piece.  In this case 
 *  streaming temperature data from a sensor to a webpage via a cloud service.  
 *  I am able to run the device off a battery pack and connect it to my mobile 
 *  phone hotspot.  By putting my finger on the temperature sensor I can
 *  show the temperature changing on the device screen and it upating on 
 *  the web page. A nice touch is to get the people you are demo'ing it to to
 *  open the web page on their own phones/devices.  Typically the web page 
 *  updates within 1-2 seconds dependent on the phone connection.  
 *  
 *  The point of the demo isn't to make an accurate temp sensor, it's to 
 *  demonstrate connecting an IoT device securely (encrypted ) to a cloud 
 *  service for subscribers to consume.
 *  
 *  Importantly this demo is very low cost, as cheap as AU$25 for components 
 *  and Google Firebase has a free pricing tier.  Also because of how Firebase 
 *  works you do not need to actually host the web client (HTML page) on a 
 *  server.  You could get away with just running the page straight off your pc.  
 *  In fact if you don't connect the device to wifi it just runs as a 
 *  temperature sensor.  
 *   
 *  I have put LOTS of comments here to help people understand what
 *  is going on and some of limitations of the hardware.  Unfortunately the 
 *  ESP8266 on Arduino is an ongoing work in progress and I've already had to 
 *  rewrite the demo once due to library updates breaking the code base.  I can
 *  only guarantee that this demo worked as of the date I uploaded it to GitHub.
 *    
 *  DESCRIPTION:
 *  This is an IoT device using a BMP180 temp sensor on an ESP8266 12e 
 *  (nodeMCU v1.0) micro controller.  The device connects to Google Firebase 
 *  using the REST API to post sensor readings. Web clients can then access the
 *  temp data through the Firebase API.   For the demo I have a one page HMTL
 *  site using the Firebase Javascript API to receive dynamic callbacks as new
 *  data is posted.  The demo utilises the ESP8266's ability to connect via
 *  HTTPS which is required to talk to the Firebase cloud service.
 *   
 *  The device uses the following components:
 *   - ESP8266 12e (nodeMCU 1.0)
 *   - ST7735 display (SPI)
 *   - BMP180 temperature sensor (I2C)
 *   
 *  To run the full demo you will also need:
 *   - A Google Firebase account to create an application on and generate a 
 *     secret key.  You must fill in the database name, and database secret in 
 *     the demo.
 *   - WiFi for device to connect to (I have used both WiFi routers and mobile
 *     phone hotspots with no problems).  You must fill in the SSID and 
 *     password of the wifi to connect to.
 *   
 *  NOTE:
 *   - You can run the device just as a temperature sensor without connecting
 *     it to Wifi or Firebase :)
 *   - I'm using the BMP180 temp sensor uncalibrated and am getting temp 
 *     readings which are way off, however it's fine for demos.  If you want 
 *     accurate temp readings you'll have some work to do, probably including
 *     changing the sensor.
 *   - All communications with Firebase have to be secure meaning you have to
 *     use HTTPS (SSL/TLS).
 *   - The ESP8266 can only handle small payloads over HTTPS because of limited
 *     memory.  You may run into problems/stability issues trying to send 
 *     larger data packets.  This is the reason the display shows message
 *     payload size (the demo sends around 40 bytes).
 *   - Since I am using the UART pins (D9 => Rx & D10 => Tx) for driving the
 *     screen you can't print any debug info to the Arduino IDE serial monitor.
 *     You could fix this by using other free pins (like D3/D4) instead.
 *     I did it this way simply to keep the wiring on my breadboard cleaner. 
 *   - The Firebase onChange query only fires callback events when the data has
 *     changed.  So if sensor readings you just posted are exactly the same as
 *     the last post the client won't receive an update event.  If you want
 *     the client to receive an update everytime a post is made then add
 *     another field with a timestamp/unique value to the JSON object.
 *   - Depending on who made your ST7735 display you will probably have 
 *     different pin names/layout than mine (a no-name display from China).  
 *     You will have to work out this out for yourself when wiring it up. 
 *     This wikipedia page lists some of the different names used for SPI
 *     pins which might help you:
 *     https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
 *     
 *  ESP8266:   
 *  I have the demo running on Arduino 1.6.8 and it uses this esp8266 arduino
 *  library: https://github.com/esp8266/Arduino
 *     
 *  which you link in by adding this board manager link in Arduino IDE 
 *  preferences (check the GitHub page above for the latest version):
 *  http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *     
 *  DISPLAY: 
 *  Your ST7735 display will probably differ to mine (no-name & off eBay) so
 *  you will have to find out which pin does what in order to wire it up.  
 *  Also you may have to change the parameter when initialising it depending
 *  on whether its a green, red, or black tab display.  Originally I couldn't
 *  get it running using software SPI in the Adafruit_ST7735 library so I had
 *  to use the hardware SPI call.  This means that the following pins are 
 *  set: d5 for SCLK, d7 for MOSI, d6 for MISO, and d8 for CS.  You can set the
 *  other pins to any free pin available if you want.  The following table
 *  shows the pins on my ST7735 display (yours may differ), their SPI role, and
 *  the ESP8266 pin they connect to:
 *     
 *  ST7735    SPI       esp8266 (h/w SPI)
 *  -------+--------+---------------------
 *  cs     |  CS    |   d8  => GPIO 15 (CS)     
 *  sck    |  SCLK  |   d5  => GPIO 14 (SCLK)   
 *  sda    |  MOSI  |   d7  => GPIO 13 (MOSI)  
 *  a0     |  DC    |   d9  => GPIO 3       <= can change this to any free pin
 *  reset  |  RST   |   d10 => GPIO 1       <= can change this to any free pin
 *  -      |  MISO  |   d6  => GPIO 12 (MISO)   
 *  
 *  NOTE!! Even though we don't use MISO (d6) you cannot use this pin for 
 *  anything else, you must keep it free.  Due to the way the Arduino library
 *  works it is possible to map CS to any free pin.  However since d8 is
 *  already set to CS you can't use it for anything else either so you would
 *  effectively end up wasting a pin.
 *  
 *  SENSOR:
 *  The BMP180 temperature sensor uses I2C and I have used the following pins
 *  but they can be changed to any free pins available.
 *  
 *  BMP180   ESP8266
 *  -------+-------------
 *  SDA    | d1 => GPIO 5      <= can change this to any free pin
 *  SCL    | d2 => GPIO 4      <= can change this to any free pin
 *  
 *  FIREBASE-ARDUINO:
 *  The demo uses the Firebase-Arduino library off GitHub which handles the
 *  connection and comms with Firebase.  This demo is only possible because
 *  of this library so make sure to thank the authors for their great work!
 *  
 *  https://github.com/googlesamples/firebase-arduino
 *  
 *  
 *  FIREBASE:
 *  I'm not even going to try to give you instructions on setting up Firebase
 *  just follow the online references and tutorials.  Make sure you are using 
 *  the latest version which is Firebase v3 (https://firebase.google.com/) and 
 *  make sure any tutorials or reference doco you are reading aren't for v2.  
 *  The basic steps are:
 *  
 *  1) Register as a Google developer
 *  2) Sign up for Firebase (v3) and select the free tier (no charge)
 *  3) Create an application (doesn't matter what you call it)
 *  4) Generate a secret for the application
 *  5) Paste the app name (<yourAppName>.firebaseio.com) and secret into the
 *     code below.
 *  
 *  You do not need to create any database rules (although that is always 
 *  recommended for security).  You do not need to create anything in the
 *  database (fields, etc) due to the way Firebase works.  If the fields don't
 *  already exist they will be created when the data is posted, if they do they
 *  will be overwritten.  Once you get the demo going you can watch the data 
 *  being updated in real time throught the Firebase console.  
 *  
 *  The data structure in Firebase roughly looks like this. Remember if you 
 *  change this structure or the field names you're going to have to reflect 
 *  those changes in the HTML page as well or it won't find the data. Since 
 *  it's stored as JSON adding extra fields doesn't break anything and the 
 *  HTML page will just ignore them until you update the code.
 *  
 *  /(root)  
 *      /BMP180
 *         /temp: "99.9"
 *         /alt: "99"
 *         /hpa: "99"
 *  
 *  
 *  CHANGE LOG:   
 *  28Oct16 [sxh] - fixed code base, nolonger uses older ST7735 lib, 
 *     implemented new version of Firebase Arduino (v3 compliant?).  
 *     Can now start without wifi and handles wifi drop outs/ins.    
 */
#include <Wire.h>            // I2C interface
#include <SPI.h>             // SPI interface
#include <ESP8266WiFi.h>     // Arduino wifi library for ESP8266
#include <SFE_BMP180.h>      // BMP180 temp sensor
#include <FirebaseArduino.h> // ESP8266 lib to make Firebase API calls over SSL
#include <Adafruit_GFX.h>    // Graphics library
#include <Adafruit_ST7735.h> // ST7735 display

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>

SFE_BMP180 pressure;
#define ALTITUDE   30.0 // current Altitude in meters
#define READ_DELAY 1000 // delay (mS) before taking and posting next sensor reading

/*** ST7735 setup ***/
#define eCS    15    // d8  => GPIO 15 (CS/SS)
#define eDC     3    // d9  => GPIO 3
#define eMOSI  13    // d7  => GPIO 13 (MOSI)
#define eSCLK  14    // d5  => GPIO 14 (SCLK)
#define eRST    1    // d10 => GPIO 1
Adafruit_ST7735 tft = Adafruit_ST7735(eCS, eDC, eRST);   // uses hardware SPI

/*** User defined params for WiFI and FireBase App ***/
#define WIFI_SSID       "??????"  // WiFi SSID (name)
#define WIFI_PW         "??????"  // Wifi password
#define FIREBASE_APP    "??????"  // Firebase app address, example "<yourAppName>.firebaseio.com"
#define FIREBASE_SECRET "??????"  // Secret your generated for your FireBase app, example "ABcd1eF23gHIjKlMNOpqR456St7UVWXyZAB89cDE"

char MAC_char[18];

void setup()
{
  Wire.begin(5,4); // I2C for temp sensor

  // Initialise ST7735 display
  tft.initR(INITR_BLACKTAB);   // initialize ST7735S chip, black tab
  tft.setRotation(0);  // set screen orientation
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.println("IoT Demo");
  tft.setTextSize(1);
  tft.println("v1.0");
  tft.setTextColor(ST7735_WHITE);
  tft.println();
  tft.println("ST7735 Init..OK");
  tft.print("BMP180 init..");  
  
  // Initialize the sensor
  if (pressure.begin())
    tft.println("OK");
  else
  {
    tft.setTextColor(ST7735_RED);
    tft.println("FAILED");
    while(1); // Pause forever.
  }

  // connect to wifi.
  tft.print("SSID: ");
  tft.println(WIFI_SSID);
  tft.print("Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PW);
  int x = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    tft.print(".");
    delay(500);
    if(x++>20)  // timeout
       break;
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    tft.println("OK");
    tft.println(WiFi.localIP());

    uint8_t MAC_array[6];
    WiFi.macAddress(MAC_array);
    for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
    }
    tft.println("MAC ");
    tft.println(MAC_char);
  }
  else
    tft.println("TIMEOUT");

  Firebase.begin(FIREBASE_APP, FIREBASE_SECRET);   // Initialise Firebase object
  delay(3000);

  // set up main screen
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(40, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.print("IoT DEMO");
  tft.setTextColor(ST7735_YELLOW);
  drawTemp(0);
  drawPressure(0.0);
  drawAltitude(0.0);

  // ssid details (bottom screen)
  tft.setTextColor(ST7735_WHITE);
  tft.drawLine(0, tft.height()-12, tft.width()-1, tft.height()-12, ST7735_WHITE);   // drawLine(x,y,x1,y1,colour)
  tft.setCursor(5,tft.height()-8);
  tft.setTextSize(1);
  tft.print("SSID: ");
  drawSSID();
  tft.setTextColor(ST7735_YELLOW);
}

void loop()
{
  char status;
  double T,P,p0,a;
 
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      drawTemp(T);  //display temp on screen
 
      // Start a pressure measurement:
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);  // wait for measurement to complete
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          drawPressure(P); // display pressure on screen
          p0 = pressure.sealevel(P,ALTITUDE);
          a = pressure.altitude(P,p0);
          drawAltitude(a); // display altitude on screen
          clearStatus();
          drawSSID();
          if (WiFi.status() ==  WL_CONNECTED) // push readings to Firebase
          {
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& data = jsonBuffer.createObject();
            data["temp"] = String(T,1);
            data["alt"]  = String(a,0);
            data["hpa"]  = String(P,0);

            Firebase.set("/BMP180", JsonVariant(data));
            drawPayload(data.measureLength(), Firebase.failed());
          }
        }
      }
    }
  }

  delay(READ_DELAY);  // Delay before starting next reading
}

/** 
 * Display the temperature reading on screen 
 */
void drawTemp(double dTemp)
{
  int y_pos = 16;
 
  tft.setTextSize(1);
  tft.setCursor(8,y_pos);
  tft.print("Temp:");
  tft.fillRoundRect( 5, y_pos+10, 118, 34, 3, ST7735_RED); //fillRoundRect(x, y, x length, y length, radius, colour)
  tft.setCursor(10,y_pos+32);
  tft.setTextSize(1);
  tft.setFont(&FreeSans18pt7b);
  tft.print(dTemp,1);
  tft.setFont();   //reset to classic font
  tft.setFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  tft.setCursor(78,y_pos+26);
  tft.print(" o");
  tft.setFont(&FreeSans12pt7b);
  tft.setTextSize(1);
  tft.setCursor(93,y_pos+33);
  tft.print("C");
  tft.setFont();   //reset to classic font
}

/**
 * Display the pressure reading on screen 
 */
void drawPressure(double dPressure)
{  
  int y_pos = 68;

  tft.setTextSize(1);
  tft.setCursor(8,y_pos);
  tft.print("Pressure:");
  tft.fillRoundRect( 5, y_pos+10, 118, 20, 3, ST7735_RED); //fillRoundRect(x, y, x length, y length, radius, colour)
  tft.setCursor(10,y_pos+20);
  tft.setTextSize(1);
  tft.setFont(&FreeSans9pt7b);
  tft.print(dPressure,0); 
  tft.print(" hPa");
  tft.setFont();
}

/** 
 *  Display the altitude reading on screen
 */
void drawAltitude(double dAltitude)
{ 
  int y_pos = 106;

  tft.setTextSize(1);
  tft.setCursor(8,y_pos);
  tft.print("Altitude:");
  tft.fillRoundRect( 5, y_pos+10, 118, 20, 3, ST7735_RED);
  tft.setCursor(10,y_pos+19);
  tft.setTextSize(1);
  tft.setFont(&FreeSans9pt7b);
  tft.print(dAltitude,0);
  tft.print("m");
  tft.setFont();
}

/** 
 *  Display the Wifi SSID (name) on screen.  Make it green if
 *  we have a connection, red if no connection
 */
void drawSSID()
{
  if(WiFi.status() == WL_CONNECTED)
    tft.setTextColor(ST7735_GREEN);
  else
    tft.setTextColor(ST7735_RED);

  tft.setCursor(36, 152);
  tft.print(WIFI_SSID);
  tft.setTextColor(ST7735_YELLOW);
}

/**
 * Display message payload (size of data we are sending)
 * in bytes.  Give it a green background if post to 
 * Firebase was successful, red if post failed.
 */
void drawPayload(int nPayload, bool bFail)
{
  tft.setTextColor(ST7735_BLACK);
  if(bFail)
  {
    tft.fillRect(100, 150 , 27, 9, ST7735_RED);
  }
  else
  {
    tft.fillRect(100, 150 , 27, 9, ST7735_GREEN);
  }
  tft.setCursor(105, 151);
  tft.print(nPayload);
  tft.setTextColor(ST7735_YELLOW);
}

/**
 * Clear status info at the bottom of the display
 * (SSID and payload info)
 */
void clearStatus()
{
  tft.fillRect(35, 150 , 93, 9, ST7735_BLACK); // fillRect(x, y, x length, y length, colour)
}
