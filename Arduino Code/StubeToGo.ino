/*
  A little RESTful Arduino Service to read sensors 

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 cobbled together 06 June 2013
 by David Grieshammer
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <TextFinder.h>


// Enter a MAC address (and IP address if you use static IP) for your controller below.
byte mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// The IP address will be dependent on your local network:
// IPAddress ip(192,168,1, 177); //We dont need an IP since we use DHCP 

char paramChars[60]; //make it public for easy access ! dont forget to reset it when client stops/end of loop!

//Refer to Server & myClient
EthernetServer myServer(80); // port 80 is default for HTTP, change it if you use another
EthernetClient myClient;

//Set Input & Output Pins
const int doorSensorPin = 2; // Reed Sensor @ door
const int tempSensor1_Pin = 0; // LM35 Temperature Sensor
const int tempSensor2_Pin = 1; // LM35 Temperature Sensor - this one is not in use yet

const int whiteLed = 3; //status leds
const int greenLed = 5;
const int redLed = 6;

void setup() {

  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial connect. Needed for Leonardo only
  }
  pinMode(doorSensorPin, INPUT);
  pinMode(whiteLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  ethernetConnect();

}


void loop() {

//Arduino listening to requests as Server
 respondRequests();

}

//Initialize & Establish the conncetion
void ethernetConnect() {

  Serial.println("++++++++++++++++++++++");
  // start the Ethernet connection and the server:
  // Ethernet.begin(mac, ip);
  if(Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP - maybe you need to adjust the example to an IP version?");
      for(;;)
        ; // no point in carrying on, so do nothing forevermore:
    }else{
      myServer.begin();
      Serial.print("Your Server is available at: ");
      Serial.println(Ethernet.localIP());
      Serial.println();
      // digitalWrite(greenLed, HIGH);
      digitalWrite(redLed, HIGH);
      // digitalWrite(whiteLed, HIGH);
      // blinkResponse(redLed, 1, 1000);
    }
}

void respondRequests() {
  myClient = myServer.available(); // listen for incoming clients

  // if myClient request
  if (myClient) {
    
    Serial.println("++++++++++++++++++++++");
    Serial.println("Somebody requests the Arduino server");

    TextFinder myFinder(myClient); //init TextFinder to search GET requests to react to specific strings -> JSONify

    if(myFinder.find("GET")) //actually you should find GET & then use .findUnitl after that, otherwise the TextFinder will parse forever
    {

        char beginChar[] = "/"; //start parsing params starting after / behind IP-adress || THATS IT!!
        char endChar[] = " HTTP"; //the End-String until the params will be searched

        myFinder.getString(beginChar, endChar, paramChars, 60); //cut out header and GET-request if existent

        Serial.println();
        Serial.print("paramChars: ");
        Serial.println(paramChars);
        Serial.println();
    }

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;

    while (myClient.connected()) {

      if (myClient.available()) {

        char c = myClient.read();
        Serial.write(c); //print header to Serial

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          //send depending on the params here >>>
          if(strstr(paramChars, "format=json")) {
            sendStatusJSON();
          }else{
            sendStatusHTML();
          }
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    myClient.stop();
    memset(paramChars, 0, 60); //clear Parameter-array
    Serial.println("myClient disonnected");
  }
}

void sendStatusHTML() {
    // send a standard http response header
    myClient.println("HTTP/1.1 200 OK");
    //add some tuning
    // myClient.println("Access-Control-Allow-Origin: *");
    myClient.println("Cache-Control: no-cache");
    myClient.println("Pragma: no-cache");

    myClient.println("Content-Type: text/html");
    myClient.println("Connection: close");
    myClient.println();
    myClient.println("<!DOCTYPE HTML>");
    myClient.println("<html>");
    

    // output the value of each analog input pin
    // int tempSensor1_value = analogRead(tempSensor1_Pin);
    myClient.print("Acutal temperature of pin");
    myClient.print(tempSensor1_Pin);
    myClient.print(" is ");
    // myClient.print(tempSensor1_value);
    // myClient.print((5.0 * tempSensor1_value * 100.0) / 1024);
    myClient.print(getTemperature(tempSensor1_Pin));
    myClient.println("<br />");

    int doorSensor_value = digitalRead(doorSensorPin);
    if(doorSensor_value == HIGH) {
      myClient.print("Door is closed ");
      myClient.println("<br />");
      }else{
      myClient.print("Door is open ");
      myClient.println("<br />");
    }
    
    myClient.println("</html>");
}

void sendStatusJSON() {
 
   // Send a standard http response header
   myClient.println("HTTP/1.1 200 OK");
   //a little bit of tuning?
   // myClient.println("Access-Control-Allow-Origin: *");
   myClient.println("Cache-Control: no-cache");
   myClient.println("Pragma: no-cache");

   myClient.println("Content-Type: application/json");
   myClient.println("Connnection: close");
   myClient.println();
   myClient.print("arduinoEthernetComCallback"); //this one needs to be similar as in the web-js fetching the data
   myClient.println("({");

   // Output the value of each analog input pin
   // float tmp_pin_1_value = analogRead(tempSensor1_Pin);
   // float tmp_pin_2_value = analogRead(tempSensor2_Pin);
   int doorSensor_value = digitalRead(doorSensorPin);

   myClient.print("\"tmp_pin_1\": ");
   myClient.print(getTemperature(tempSensor1_Pin));
   myClient.println(",");

   myClient.print("\"tmp_pin_2\": ");
   myClient.print(getTemperature(tempSensor2_Pin));
   myClient.println(",");

   myClient.print("\"door\": ");
   if(doorSensor_value == HIGH) {
      myClient.print(true);
    }else{
      myClient.print(false);
    }
   myClient.println("\n})");

}
//TODO include, not in use yet 
void send404JSON(EthernetClient myClient) {
    myClient.println("HTTP/1.1 404 OK");
    myClient.println("Content-Type: text/html");
    myClient.println("Connnection: close");
    myClient.println();
    myClient.println("<!DOCTYPE HTML>");
    myClient.println("<html><body>404</body></html>");
}

// function to read temperature value
float getTemperature(int sensorPin) {
  float tmpSensor_value = 0;
  for(int i = 0; i<5; i++) {
     tmpSensor_value += analogRead(sensorPin);
     delay(10);
  }
  tmpSensor_value = tmpSensor_value / 5;
  tmpSensor_value = ((5.0 * tmpSensor_value * 100.0) / 1024);
  return tmpSensor_value;
}

// generic function to blink the LEDs  
void blinkResponse(int myPin, int myCount, int myTime) {
  for(int i = 0; i < myCount; i++) {
    digitalWrite(myPin, HIGH);
    delay(myTime);
    digitalWrite(myPin, LOW);
    delay(myTime);
  }
}

