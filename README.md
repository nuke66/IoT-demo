# IoT-demo
*IoT temperature device utilising Arduino, ESP8266, Firebase v3, and HTML/JS client*

![IoT Demo](http://www.nuke66.com/temp/IoT_demo.jpg)

## Intro:
I have created this IoT demonstration as a portfolio piece.  The point of the demo isn't to make an accurate temp sensor (although you could do that), it's to demonstrate connecting an IoT device securely to a cloud service for subscribers to consume.  Importantly this demo is very low cost, as cheap as AU$25 for components and Google Firebase currently has a free pricing tier.

I have put **LOTS** of comments into the code to help people understand what is going on and some of limitations of the hardware.  Unfortunately the ESP8266 on Arduino is an ongoing work in progress and I've already had to rewrite the demo once due to library updates breaking the code base.   

## Description:
This is an IoT device that posts readings from a temperature sensor to a cloud service which then sends the data to web clients (typically I get a 1-2 second delay from device to client depending on the connection speed).  The device uses a BMP180 temp sensor on an ESP8266 micro controller running Arduino and connects to the internet via a supplied Wifi connection.  It connects to Google Firebase using the REST API to post sensor readings. Clients (web, iOS, Android) can then access the temp data through the Firebase APIs.  For the demo I have a one page HMTL site using the Firebase Javascript Web API to receive dynamic callbacks as new data is posted.  The demo utilises the ESP8266's ability to connect via HTTPS which is required to talk to the Firebase cloud service.
 
The device uses the following components:
 - **ESP8266** 12e (nodeMCU 1.0)
 - **ST7735 1.8" display** (SPI)
 - **BMP180** temperature sensor (I2C)
 
To run the full demo you will also need:
 - A **Google Firebase account** to create an application on and generate a secret key.  You must fill in the database name, and database secret in the arduino code.  You must fill in the domain, database URL, and API into the html code for the web page to connect to Firebase.
 - A **Wifi connection** for the device to connect to (I have used both Wifi routers and mobile phone hotspots with no problems).  You must fill in the SSID and password of the wifi to connect to.
 
You can run the device just as a temperature sensor without connecting it to Wifi or Firebase.

## Device features:
* Communications are secure (via https) and are direct to Firebase (no proxy).
* Handles wifi drop outs/ins (SSID green = connected, SSID red = not connected).
* Displays message payload and status (displays data size in bytes, green background = message success, red background = message failure).
* (web page) Due to the way Firebase works you do not need to host the web page, you can just run it from your PC.  However if you want to demo it on a mobile device (like I need to) you will need to host it.

## Future Updates:
I am already planning to add the ability to monitor multiple devices.  Although the changes to the arduino code is easy enough (I'll include the MAC address of the device in the post to Firebase) the client changes start to get a bit more complex especially as I want to have swipe navigation to swap between devices.
