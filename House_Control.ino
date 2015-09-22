/*

INTEL EDISON PROJECT : SMART HOUSE

Control element in the house using a wireless devise
Elements in the house are:
  * Alarm
  * Security system
  * Geyser
  * Temperature adjuster
User interface in hosted on port 85 of server "xxx.xxx.xx"

Created 29 Jul 2015
by Othniel Konan
*/

/////////////////
//  LIBRARIES  //
/////////////////
// Wifi 
#include <SPI.h>
#include <WiFi.h>
// Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>
/////////////////
//  CONSTANTS  //
/////////////////

/*-----( Declare Constants )-----*/
#define ONE_WIRE_BUS 4 /*-(Connect to Pin 2 )-*/


///////////////
//  OBJECTS  //
///////////////
// Set up a oneWire instance to communicate with any OneWire device
OneWire ourWire(ONE_WIRE_BUS);
// Tell Dallas Temperature Library to use oneWire Library 
DallasTemperature tempSensor(&ourWire);

/////////////////
//  VARIABLES  //
/////////////////

// Wifi
int status = WL_IDLE_STATUS;
WiFiServer server(85);

// Menu
int iD = 0;

// Time
boolean oneMin = false;

// Boolean for menu
boolean bol = false;

//Geyser
boolean gStat = false; // Status of the geyser
int gTimeOn = 0;       // The time at which the geyser was turned on (in minutes)
int gTimeOff = 0;       // The time at which the geyser was turned off (in minutes)
int gTimeRemaining = 0;       // The time in which the geyser will be turned off (in minutes)
int gTimeCounter = -1;        // Keep track of time
boolean gOn = false;
boolean gProgram = false;

// Alarm
int aTime = -1;
int aGeyser = -1;
boolean aNorOn = false;
boolean aSmOn = false;
boolean aNor = false;
boolean aSm = false;
boolean aGeyOn = false;
int numTones = 27;
int tones[] = {261, 277, 261, 277, 294, 277, 294, 311, 294, 311 , 330, 311, 330, 349, 330,  349, 370, 349, 370, 392, 370, 392, 415, 392, 415, 440, 415};

// Temp

// Security
int tempSTime = 0;
int sTimeOn = 0;       // The time at which the geyser was turned on (in minutes)
int sTimeOff = 0;       // The time at which the geyser was turned off (in minutes)
int sTimeRemaining = 0;       // The time in which the geyser will be turned off (in minutes)
int sTimeCounter = -1;        // Keep track of time
boolean sOn = false;
boolean sProgram = false;
int numTones2 = 30;
int tones2[] = {300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500,300,500};

//Pins
int geyPin = 9;
int serverPin = 7;
int clientPin = 8;
int heaterPin = 3;
int alarmPin = 5;
int securityPin = 6;

// Others
char ssid[] = "etilab";      //  your network SSID (name) 
char pass[] = "@etilab2!";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
String HTTP_req;          // stores the HTTP request

String currentLine = "";                // make a String to hold incoming data from the client
boolean currentLineIsBlank = false;

////////////////////////////////////////////////////////////////////////////////
//                               SETUP                                        //
////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(geyPin,OUTPUT);
  pinMode(serverPin,OUTPUT);
  pinMode(clientPin,OUTPUT);
  pinMode(heaterPin,OUTPUT);
  pinMode(securityPin,INPUT);
   
  Serial.begin(9600);      // initialize serial communication
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }
  
  // check firmware version
  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    digitalWrite(serverPin,LOW);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }
  server.begin();                           // start the web server on port 80
  tempSensor.begin();
  digitalWrite(serverPin,HIGH);
  printWifiStatus();                        // you're connected now, so print out the status
}

////////////////////////////////////////////////////////////////////////////////
//                             MAIN LOOP                                      //
////////////////////////////////////////////////////////////////////////////////
void loop(){
  // Update the timer in the program
  oneMin=mTime();
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client)  // if you get a client,
  {  
    digitalWrite(clientPin,HIGH);
    Serial.println();
    Serial.println();    
    Serial.println("NEW CLIENT");           // print a message out the serial port
    currentLine = "";                // make a String to hold incoming data from the client
    currentLineIsBlank = true;
     while (client.connected()) {            // loop while the client's connected 
           oneMin=mTime();  // time
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        HTTP_req += c;                      // save the HTTP request 1 char at a time
        Serial.write(c);                    // print it out the serial monitor
                
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          
          if (currentLine.length() == 0) {
            // send a standard http response header
                    bol = false;
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                      switch(iD){
                        case 0: menuHtml(client); break;
                        case 10: geyserOffHtml(client); break; 
                        case 11: geyserOnHtml(client); break;
                        case 12: geyserProgrammedHtml(client); break;
                        case 20: TempMenuHtml(client); break;
                        case 21: TempOtherHtml(client); break;
                        case 22: TempOnHtml(client); break;
                        case 30: SecurityOffHtml(client); break; 
                        case 31: SecurityOnHtml(client); break;
                        case 32: SecurityProgrammedHtml(client); break;
                        case 40: AlarmModeHtml(client); break;
                        case 41: AlarmNormalHtml(client); break;
                        case 42: AlarmSmartHtml(client); break;
                        case 43: AlarmNormalOnHtml(client); break;
                        case 44: AlarmSmartOnHtml(client); break;
                        
                        default: menuHtml(client); break;
                      }
                      Serial.println("@@@@@@@");
                      Serial.print(iD);
                      Serial.print(" time elapsed : ");
                      Serial.println(millis()/60000);
                      Serial.println("@@@@@@@");
                      Serial.print(HTTP_req);
                    HTTP_req = "";    // finished with request, empty string
                    break;
          } // Send response
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
            if(!bol){
            switch(iD){
                       case 0: menuInput(client); break;
                       case 10: geyserOffInput(client); break;
                       case 11: geyserOnInput(client); break;
                       case 12: geyserProgammedInput(client); break;
                       case 20: TempMenuInput(client); break;
                       case 21: TempOtherInput(client); break;
                       case 22: TempOnInput(client); break;
                       case 30: SecurityOffInput(client); break; 
                       case 31: SecurityOnInput(client); break;
                       case 32: SecurityProgammedInput(client); break;
                       case 40: AlarmModeInput(client); break;
                       case 41: AlarmNormalInput(client); break;
                       case 42: AlarmSmartInput(client); break;
                       case 43: AlarmNormalOnInput(client); break;
                       case 44: AlarmSmartOnInput(client); break;
                       default: break;
                    }
            bol = true;
            }
         }
        }
        
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
      } // Client available
      // TIME CODE HERE
        oneMin=mTime();
        geyserStatus();
        alarmStatus();
        SecurityStatus();
    } // Client connected
    delay(1);      // give the web browser time to receive the data
    // close the connection:
    client.stop();
    digitalWrite(clientPin,LOW);
    Serial.println("client disonnected");
  } // If client
  
  // TIME CODE HERE
  oneMin=mTime();
  geyserStatus();
  alarmStatus();
  SecurityStatus();
} // void loop

////////////////////////////////////////////////////////////////////////////////
//                               METHODS                                      //
////////////////////////////////////////////////////////////////////////////////

/*
 *  Print the status of the the wifi connection
 */
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/*
 *  return id of html elements 
 */
 int idElement(WiFiClient cl, String st){
   return getHtmlValue(cl,st).toInt(); 88777777777
 }

/*
 * Get the string typed in the box
 * @WiFiClient, the user
 * @String, the name of the variable whose value is needed
 * @return, the input string
 */
 String getHtmlValue(WiFiClient cl, String st) {
    String Short_HTTP_req = HTTP_req.substring(0,HTTP_req.indexOf("Referer"));
    int i = Short_HTTP_req.indexOf(st);
    int j = Short_HTTP_req.indexOf("HTTP/1.1");
    return HTTP_req.substring(i+st.length()+1,j+1); 
 }
 
 /*
 * Get the string typed in the box
 * @WiFiClient, the user
 * @String, the name of the variable whose value is needed
 * @return, the input string
 */
 int getHtmlIntValue(WiFiClient cl, String st) {
    String Short_HTTP_req = HTTP_req.substring(0,HTTP_req.indexOf("Referer"));
    int i = Short_HTTP_req.indexOf(st);
    Short_HTTP_req = Short_HTTP_req.substring(i);
    int j = Short_HTTP_req.indexOf("&");
    return Short_HTTP_req.substring(st.length()+1,j).toInt();  
 }
 
 /*
 * Check if a word is in the request
 * @WiFiClient, the user
 * @String, the name of the variable whose value is needed
 * @return, the input string
 */
 boolean isHtmlValue(WiFiClient cl, String st) {
   String Short_HTTP_req = HTTP_req.substring(0,HTTP_req.indexOf("Referer"));
   int i = Short_HTTP_req.indexOf(st);
   if(i<0)return false;
   else return true; 
 }
 

/*
 *  Get temperature of the temperature sensor
 *  @DallasTemperature, the temperature sensor oblect
 *  @return, the value of the temperature in degree Celcus
 */
 int getTemp() {
  tempSensor.requestTemperatures();
  return tempSensor.getTempCByIndex(0);
 }
 
/*
 * Convert time in minutes
 * Return -1 if the format of time is invalid
 * @return the equivalent number of minutes
 */
 int convertToMin (int theHour, int theMin) {
  if(theHour < 0 || theMin < 0){return -1;}
  return theHour*60 + theMin;
 }
 int convertToMin (int theDay, int theHour, int theMin) {
  if(theDay < 0 || theHour < 0 || theMin < 0){return -1;}
  return theDay*1440 + theHour*60 + theMin;
 }
 int convertToMin (int theWeek, int theDay, int theHour, int theMin) {
  if(theWeek < 0 || theDay < 0 || theHour < 0 || theMin < 0){return -1;}
  return theWeek*10080 + theDay*1440 + theHour*60 + theMin;
 }

 /*
  * Start of the html page
  */
  void printHtmlStart(WiFiClient cl) {
    cl.println("<!DOCTYPE html>");
    cl.println("<html>");
    cl.println("<head>");
    cl.println("<title>Arduino LED Control</title>");
    cl.println("</head>");
    cl.println("<body>");
  }
  /*
   * End of the html page
   */
   void printHtmlEnd(WiFiClient cl) {
    cl.println("</body>");
    cl.println("</html>");
   }
   
  //---------------------------------------------------------------------------//
  //                                  GEYSER methods                           //
  //---------------------------------------------------------------------------//
   
 /*
  * Print html format for user interface with geyser
  * and make the setting of the geyser
  * @WiFiCLient, the user
  */
  void geyserOffHtml(WiFiClient cl) { // Id = 10
    cl.println("<br><h1>GEYSER SETTINGS</h1><br>");
    cl.println("<form>");
    cl.println("Status: Off");
    cl.println("<br>Turn on in:");
    cl.println("<input type=\"text\" name=\"gh_on\" size=\"1\"> h");
    cl.println("<input type=\"text\" name=\"gm_on\" size=\"1\"> m<br>");
    cl.println("Turn off in:");
    cl.println("<input type=\"text\" name=\"gh_off\" size=\"1\"> h");
    cl.println("<input type=\"text\" name=\"gm_off\" size=\"1\"> m<br><br>");
    cl.println("<input type=\"submit\" name=\"gValid\" value=\"Validate\">");
    cl.println("</form>");
    cl.println("<br>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");
    cl.println("<br>");
  }
  /*
   * GeyserOff coding
   */
   void geyserOffInput(WiFiClient cl) {
    if(isHtmlValue(cl,"gValid")) {// if submition
    // then take data
      gTimeOn = convertToMin(getHtmlIntValue(cl,"gh_on"),getHtmlIntValue(cl,"gm_on"));
      gTimeOff = convertToMin(getHtmlIntValue(cl,"gh_off"),getHtmlIntValue(cl,"gm_off"));
      gTimeRemaining = abs( gTimeOff - gTimeOn);    
      gTimeCounter = gTimeOn;
      if(gTimeOn>=gTimeOff){gTimeOn=0; gTimeOff=0;gTimeRemaining=0;gTimeCounter=-1;return;}
     if(gTimeCounter==0){gTimeCounter=-1;gOn=true; iD=11;  }// Geyser is programmed for now !!!!!!!!!!!! boolean??
     else{ gProgram=true;iD=12;} // Geyser is programmed for later  
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  
  /*
  * Print html format for user interface with geyser
  * and make the setting of the geyser
  * @WiFiCLient, the user
  */
  void geyserOnHtml(WiFiClient cl) { //Id = 11
    cl.println("<br><h1>GEYSER SETTINGS</h1><br>");
    cl.println("<form method=\"GET\">");
    cl.println("Status: On<br>");
    cl.println("Geyser was on for  :");
    cl.println(gTimeOff-gTimeOn-gTimeRemaining);
    cl.println("<br>");
    cl.println("Time remaining    : ");
    cl.println(gTimeRemaining);
    cl.println("<br>");
    cl.println("Turn it off :");
    cl.println("<input type=\"radio\" name=\"turnGerseyOff\" value=\"1\"> Yes ");
    cl.println("<input type=\"radio\" name=\"turnGerseyOff\" value=\"0\" checked> No ");
    cl.println("<input type=\"submit\" name=\"gValid\" value=\"OK\"><br>");
    cl.println("</form>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");
  }
  /*
   * GeyserOn coding
   */
   void geyserOnInput(WiFiClient cl) {
    if(isHtmlValue(cl,"gValid")) {// if submition
    // then take data
      if(getHtmlIntValue(cl,"turnGerseyOff")==1) {
        gOn = false;  // Geyser is off
        // Reset parameters
        gTimeOn = 0;
        gTimeOff = 0;
        gTimeRemaining = 0;
        gTimeCounter = -1;
        iD = 10;
      }
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
   
 /*
  * Print html format for user interface with geyser
  * and make the setting of the geyser
  * @WiFiCLient, the user
  */
  void geyserProgrammedHtml(WiFiClient cl) { // Id = 12
    cl.println("<br><h1>GEYSER SETTINGS</h1><br>");
    cl.println("<form method=\"GET\">");
    cl.println("Status: Programmed<br>");
    cl.println("Geyser was programmed to be turned on in ");
    cl.println(gTimeCounter);
    cl.println(" min<br>");
    cl.println("Geyser was programmed to be turned off in  ");
    cl.println(gTimeOff-gTimeOn);
    cl.println(" min after<br>");
    cl.println("Change settings :");
    cl.println("<input type=\"radio\" name=\"geyserPro\" value=\"1\"> Reprogram ");
    cl.println("<input type=\"radio\" name=\"geyserPro\" value=\"0\" checked> No ");
    cl.println("<input type=\"submit\" name=\"gValid\" value=\"OK\"><br>");
    cl.println("</form>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");
  }
  /*
   * GeyserOn coding
   */
   void geyserProgammedInput(WiFiClient cl) {
     if(isHtmlValue(cl,"gValid")) {
      if(getHtmlIntValue(cl,"geyserPro")==1) {
        gProgram=false;
        sOn=false;
        iD=10;
      }
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  
  /*
     * Update status for geyser
     */
    void geyserStatus(){
      if(oneMin){ // One minutes has passed
            if(gOn && gTimeRemaining>0)gTimeRemaining--;
            
            if(gProgram && gTimeCounter>0)gTimeCounter--; // Decrese counter every minute
             delay(1); // IMPORTANT
        }
        if(gTimeRemaining==0)gOn=false;
        if(gTimeCounter==0){gTimeCounter=-1;gOn=true;gProgram=false;} // Turn on geyser if it's the programmed time
        if(gOn)digitalWrite(geyPin,HIGH);
        else digitalWrite(geyPin,LOW);
    }
  //---------------------------------------------------------------------------//
  //                                  ALARM methods                            //
  //---------------------------------------------------------------------------//
  
  /*
   * Display htmal page of AlarmMode
   */
   void AlarmModeHtml(WiFiClient cl){
    cl.println("<br><h1>ALARM SETTINGS</h1>");
    cl.println("<h4>Mode selection </h4>");
    cl.println("<form>");
    cl.println("Alarm mode: ");
    cl.println("<input type=\"radio\" name=\"alarmSetting\" value=\"1\"> Normal");
    cl.println("<input type=\"radio\" name=\"alarmSetting\" value=\"0\"> Smart alarm");
    cl.println("<input type=\"submit\" name=\"aValid\" value=\"Validate\">");
    cl.println("</form><br>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");   
   }
   /*
    * 
    */
   void AlarmModeInput(WiFiClient cl) {
     if(isHtmlValue(cl,"aValid")) {
      if(getHtmlIntValue(cl,"alarmSetting")==1) {
        aNor=true;aSm=false;
        iD=41;
      }
      else iD=42;
     }
     
     if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
   /*
   * Display htmal page of AlarmNormalMode
   */
   void AlarmNormalHtml(WiFiClient cl){
    cl.println("<br><h1>ALARM SETTINGS</h1>");
    cl.println("<h4>Normal mode </h4>");
    cl.println("<form>");
    cl.println("Ring in:");
    cl.println("<input type=\"text\" name=\"ah_on\" size=\"1\"> h");
    cl.println(" <input type=\"text\" name=\"am_on\" size=\"1\"> m<br>");
    cl.println("<input type=\"submit\" name=\"aValid\" value=\"Validate\">");
    cl.println("</form><br>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"alarmPage\" value=\"Alarm mode\" >");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");   
   }
   /*
    * 
    */
    void AlarmNormalInput(WiFiClient cl) {
     if(isHtmlValue(cl,"aValid")) {
      aTime = convertToMin(getHtmlIntValue(cl,"ah_on"),getHtmlIntValue(cl,"am_on"));
      if(aTime==0){};// alarm Ring
      if(aTime>0){
        iD=43;
        aNorOn=true;aSmOn=false;aNor=false;aSm=false;
      }
     }
     if(isHtmlValue(cl,"alarmPage")){
      if(getHtmlIntValue(cl,"alarmPage")==0)iD=40;
     }
     if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  /*
   * Display htmal page of AlarmSmartMode
   */
   void AlarmSmartHtml(WiFiClient cl){
    cl.println("<br><h1>ALARM SETTINGS</h1>");
    cl.println("<h4>Smart mode </h4>");
    cl.println("<form>");
    cl.println("Ring in:");
    cl.println("<input type=\"text\" name=\"ah_on\" size=\"1\"> h");
    cl.println(" <input type=\"text\" name=\"am_on\" size=\"1\"> m<br>");
    cl.println("<input type=\"checkbox\" name=\"alarmGeyserOn\" value=\"1\"> Geyser <br>");
    cl.println("&emsp;	&emsp;	Turn for :");
    cl.println("<input type=\"text\" name=\"agh_on\" size=\"1\"> h");
    cl.println("<input type=\"text\" name=\"agm_on\" size=\"1\"> m<br><br>");
    cl.println("<input type=\"submit\" name=\"aValid\" value=\"Validate\">");
    cl.println("</form><br>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"alarmPage\" value=\"Alarm mode\" >");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");   
   }
  /*
    * 
    */
    void AlarmSmartInput(WiFiClient cl) {
     if(isHtmlValue(cl,"aValid")) {
      aTime= convertToMin(getHtmlIntValue(cl,"ah_on"),getHtmlIntValue(cl,"am_on"));
      if(aTime==0){};// alarm Ring
      if(aTime>0){
         if(getHtmlIntValue(cl,"alarmGeyserOn")==1){
          gTimeRemaining = convertToMin(getHtmlIntValue(cl,"agh_on"),getHtmlIntValue(cl,"agm_on"));
          if(gTimeRemaining<0);
          else{
            aGeyOn = true; // alarm geyset was set
            gTimeOn = 0;
            gTimeOff = 0;
            gTimeCounter = aTime-gTimeRemaining;
            aGeyser = gTimeRemaining;
            gOn=false; gProgram=true; 
          }
        }
        iD=44;
        aNorOn=false;aSmOn=true;aNor=false;aSm=false;
      }
     }
     if(isHtmlValue(cl,"alarmPage")){
      if(getHtmlIntValue(cl,"alarmPage")==0)iD=40;
    }
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  /*
   * Display htmal page of AlarmModeNormal
   */
   void AlarmNormalOnHtml(WiFiClient cl){
    cl.println("<br><h1>ALARM SETTINGS</h1>");
    cl.println("<h4>STATUS: Normal mode </h4>");
    cl.println("<form>");
    cl.println("Ring in:");
    cl.println(aTime);
    cl.println("</form><br>");
    cl.println("<form>");
    cl.println("Desactivate alarm : ");
    cl.println("<input type=\"radio\" name=\"alarmPro\" value=\"1\"> Yes ");
    cl.println("<input type=\"radio\" name=\"alarmPro\" value=\"0\" checked> No ");
    cl.println("<input type=\"submit\" name=\"aValid\" value=\"OK\"><br>");
    cl.println("</form>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"alarmPage\" value=\"alarmMode\" >");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");   
   }
  /*
   *
   */
   void AlarmNormalOnInput(WiFiClient cl){
     if(isHtmlValue(cl,"aValid")) {
      if(getHtmlIntValue(cl,"alarmPro")==1) {
       aNorOn=false;aSmOn=false;aNor=false;aSm=false;
       aGeyOn = false; // alarm geyset was set
            gTimeOn = 0;
            gTimeOff = 0;
            gTimeCounter = -1;
            aGeyser = -1;
            gTimeRemaining=-1;
        iD=40;
      }
     }  
     // check alarm button
     if(isHtmlValue(cl,"alarmPage")) {
      if(getHtmlIntValue(cl,"alarmPage")==0)iD=40;
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  
   /*
   * Display htmal page of AlarmModeNormal
   */
   void AlarmSmartOnHtml(WiFiClient cl){
    cl.println("<br><h1>ALARM SETTINGS</h1>");
    cl.println("<h4>STATUS: Normal mode </h4>");
    cl.println("<form>");
    cl.println("Ring in:");
    cl.println(aTime);
    cl.println("<br>Geyser on for : ");
    cl.println(aGeyser);
    cl.println("</form><br>");
    cl.println("<form>");
    cl.println("Desactivate smart alarm : ");
    cl.println("<input type=\"radio\" name=\"alarmPro\" value=\"1\"> Yes ");
    cl.println("<input type=\"radio\" name=\"alarmPro\" value=\"0\" checked> No ");
    cl.println("<input type=\"submit\" name=\"aValid\" value=\"OK\"><br>");
    cl.println("</form>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"alarmPage\" value=\"alarmMode\" >");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");   
   } 
  
  /*
   *
   */
   void AlarmSmartOnInput(WiFiClient cl){
      if(isHtmlValue(cl,"aValid")) {
        if(getHtmlIntValue(cl,"alarmPro")==1) {
         aNorOn=false;aSmOn=false;aNor=false;aSm=false;
          aGeyOn = false; // alarm geyset was set
            gTimeOn = 0;
            gTimeOff = 0;
            gTimeCounter = -1;
            aGeyser = -1;
            gTimeRemaining=-1;
          iD=40;
        }
      }
     // check alarm button
     if(isHtmlValue(cl,"alarmPage")) {
      if(getHtmlIntValue(cl,"alarmPage")==0)iD=40;
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
    if(oneMin && aGeyser>0)aGeyser--;
   }
   
  /*
   *
   */
   void alarmStatus(){
     if(oneMin){
       if((aSmOn || aNorOn) && aTime>0)aTime--;
       if(aTime==0 && (aSmOn || aNorOn)){
         // to be changed
         alarmRingtone();
         aNorOn=false;aSmOn=false;  
       }//Ring
       if(!gProgram || !gOn)aGeyOn=false;
     }
   }
  /*
   *
   */
   void alarmRingtone(){
         for (int i = 0; i < numTones; i++)
      {
        tone(alarmPin, tones[i]);
        delay(200);
      }
      noTone(alarmPin);
   }
  //---------------------------------------------------------------------------//
  //                                  SECURITY methods                         //
  //---------------------------------------------------------------------------//
  
  /*
   * Display htmal page of AlarmModeNormal
   */
   void TempMenuHtml(WiFiClient cl){
      cl.println("<br><h1>TEMPERATURE SETTINGS</h1><br>");
      cl.println("<p>Status: Off</p>");
      cl.println("<form>");
      cl.println("House temperature : ");//////
      cl.println(getTemp());
      cl.println("degree C<br>");
      cl.println("Adjust house temperature to :");
      cl.println("<input type=\"radio\" name=\"Temp\" value=\"1\"> Room temperature");
      cl.println("<input type=\"radio\" name=\"Temp\" value=\"2\" checked> Other");
      cl.println("<input type=\"submit\" value=\"OK\"><br>");
      cl.println("</form><br>");
      cl.println("<form>");
      cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
      cl.println("</form>");
   }
  /*
   *
   */
   void TempMenuInput(WiFiClient cl) {
   }
  
  /*
   * Display htmal page of AlarmModeNormal
   */
   void TempOtherHtml(WiFiClient cl){
      cl.println("<br><h1>TEMPERATURE SETTINGS</h1><br>");
      cl.println("<p>Status: off</p>");
      cl.println("<form oninput=\"x.value=parseInt(a.value)+parseInt(b.value)\">");
      cl.println("House temperature : ");//////
      cl.println(getTemp());
      cl.println("degree C<br>");
      cl.println("Adjust house temperature to :");
      cl.println("<input type=\"text\" id=\"a\" name=\"houseTemp\" value=\"0\" size=\"1\"> +");
      cl.println("<input type=\"range\" id=\"b\" min=\"0\" max=\"25\" name=\"tempRange\" value=\"0\">");
      cl.println("= <output name=\"x\" for=\"a b\"></output>");
      cl.println("degree C ");
      cl.println("<input type=\"submit\" value=\"OK\"><br>");
      cl.println("</form><br>");
      cl.println("<form>");
      cl.println("<input  type=\"submit\" name=\"tempPage\" value=\"Temp. Mode\" >");
      cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
      cl.println("</form>");
   }
   /*
   *
   */
   void TempOtherInput(WiFiClient cl) {
   }
   
   /*
   * Display htmal page of AlarmModeNormal
   */
   void TempOnHtml(WiFiClient cl){
      cl.println("<br><h1>TEMPERATURE SETTINGS</h1><br>");
      cl.println("<p>Status: on</p>");
      cl.println("<form>");
      cl.println("House temperature : ");//////
      cl.println(getTemp());
      cl.println(" degree C<br>");
      cl.println("Temperature set to : ");//////
      cl.println(" degree C<br>");
      cl.println("Temp. adjuster was on for : ");//////
      cl.println(" min<br>");
      cl.println("Turn it off :");
      cl.println("<input type=\"radio\" name=\"turnTempOff\" value=\"1\"> Yes ");
      cl.println("<input type=\"radio\" name=\"turnTempOff\" value=\"0\" checked>");
      cl.println("<input type=\"submit\" value=\"OK\"><br>");
      cl.println("<input type=\"submit\" value=\"OK\"><br>");
      cl.println("</form><br>");
      cl.println("<form>");
      cl.println("<input  type=\"submit\" name=\"tempPage\" value=\"Temp. Mode\" >");
      cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
      cl.println("</form>");
   }
   /*
   *
   */
   void TempOnInput(WiFiClient cl) {
   }
   
  //---------------------------------------------------------------------------//
  //                                  MENU methods                             //
  //---------------------------------------------------------------------------//
  /*
   *
   */
   void SecurityOffHtml(WiFiClient cl) {
     cl.println("<br><h1>SECURITY SETTINGS</h1><br>");
     cl.println("<p>Status : Off</p>");
     cl.println("<form>");
     cl.println("Turn on in:");
     cl.println("<input type=\"text\" name=\"sh_on\" size=\"1\"> h");
     cl.println("<input type=\"text\" name=\"sm_on\" size=\"1\"> m<br>");
     cl.println("Turn off in: ");
     cl.println("<input type=\"text\" name=\"sh_off\" size=\"1\"> h");
     cl.println("<input type=\"text\" name=\"sm_off\" size=\"1\"> m<br><br>");
     cl.println("<input type=\"submit\" name=\"sValid\" value=\"Validate\">");
     cl.println("</form><br>");
     cl.println("<form>");
     cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
     cl.println("</form>");
   }
   /*
    *
    */
    void SecurityOffInput(WiFiClient cl) {
        if(isHtmlValue(cl,"sValid")) {// if submition
      // then take data
        sTimeOn = convertToMin(getHtmlIntValue(cl,"sh_on"),getHtmlIntValue(cl,"sm_on"));
        sTimeOff = convertToMin(getHtmlIntValue(cl,"sh_off"),getHtmlIntValue(cl,"sm_off"));
        sTimeRemaining = abs( sTimeOff - sTimeOn);    
        sTimeCounter = sTimeOn;
        if(sTimeOn>=sTimeOff){sTimeOn=0; sTimeOff=0;sTimeRemaining=0;sTimeCounter=-1;return;}
       if(sTimeCounter==0){sTimeCounter=-1;sOn=true; iD=31;  }// Geyser is programmed for now !!!!!!!!!!!! boolean??
       else{ sProgram=true;iD=32;} // Geyser is programmed for later  
      }
      // check home page button
      if(isHtmlValue(cl,"menu")){
        if(getHtmlIntValue(cl,"menu")==0)iD=0;
      }
    }
    /*
   *
   */
   void SecurityOnHtml(WiFiClient cl) {
     cl.println("<br><h1>SECURITY SETTINGS</h1><br>");
     cl.println("<p>Status : On</p>");
     cl.println("<form>");
     cl.println("Security device was activated"); ////
     cl.println(sTimeOff-sTimeOn-sTimeRemaining);
     cl.println(" ago <br>");
     cl.println("Security device detected "); ////
     cl.println(tempSTime);
     cl.println("  intruder(s) <br>");
     cl.println("</form><br>");
     cl.println("<form>");
     cl.println("Turn it off :");
     cl.println("<input type=\"radio\" name=\"turnSOff\" value=\"1\"> Yes ");
     cl.println("<input type=\"radio\" name=\"turnSOff\" value=\"0\" checked> No ");
     cl.println("<input type=\"submit\" name=\sValid\" value=\"OK\"><br>");
     cl.println("</form>");
     cl.println("<form>");
     cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
     cl.println("</form>");
   }
   void SecurityOnInput(WiFiClient cl) {
    if(isHtmlValue(cl,"sValid")) {// if submition
    // then take data
      if(getHtmlIntValue(cl,"turnSOff")==1) {
        sOn = false;  // Geyser is off
        // Reset parameters
        tempSTime = 0;
        sTimeOn = 0;
        sTimeOff = 0;
        sTimeRemaining = 0;
        sTimeCounter = -1;
        iD = 30;
      }
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
   /*
  * Print html format for user interface with geyser
  * and make the setting of the geyser
  * @WiFiCLient, the user
  */
  void SecurityProgrammedHtml(WiFiClient cl) { // Id = 12
    cl.println("<br><h1>SECURITY SETTINGS</h1><br>");
    cl.println("<form method=\"GET\">");
    cl.println("Status: Programmed<br>");
    cl.println("Security system was programmed to be turned on in ");
    cl.println(sTimeCounter);
    cl.println(" min<br>");
    cl.println("Security system was programmed to be turned off in  ");
    cl.println(sTimeOff-sTimeOn);
    cl.println(" min after<br>");
    cl.println("Change settings :");
    cl.println("<input type=\"radio\" name=\"sPro\" value=\"1\"> Reprogram ");
    cl.println("<input type=\"radio\" name=\"sPro\" value=\"0\" checked> No ");
    cl.println("<input type=\"submit\" name=\"sValid\" value=\"OK\"><br>");
    cl.println("</form>");
    cl.println("<form>");
    cl.println("<input  type=\"submit\" name=\"menu\" value=\"Menu Page\" >");
    cl.println("</form>");
  }
  /*
   * GeyserOn coding
   */
   void SecurityProgammedInput(WiFiClient cl) {
     if(isHtmlValue(cl,"sValid")) {
      if(getHtmlIntValue(cl,"sPro")==1) {
        sProgram=false;
        sOn=false;
        iD=30;
      }
    }
    // check home page button
    if(isHtmlValue(cl,"menu")){
      if(getHtmlIntValue(cl,"menu")==0)iD=0;
    }
   }
  
  /*
     * Update status for geyser
     */
    void SecurityStatus(){
      if(oneMin){ // One minutes has passed
            if(sOn && sTimeRemaining>0)sTimeRemaining--;
            
            if(sProgram && sTimeCounter>0)sTimeCounter--; // Decrese counter every minute
             delay(1); // IMPORTANT
        }
        if(sTimeRemaining==0)sOn=false;
        if(sTimeCounter==0){sTimeCounter=-1;sOn=true;sProgram=false;} // Turn on geyser if it's the programmed time
        //if(sOn)digitalWrite(securityPin,HIGH);
        //else digitalWrite(securityPin,LOW);
        if(sOn && digitalRead(securityPin)){
          securityRingtone();
          // save time
          tempSTime ++;
        }//tempSTime
    }
    
    void securityRingtone(){
         for (int i = 0; i < numTones2; i++)
      {
        tone(alarmPin, tones2[i]);
        delay(200);
      }
      noTone(alarmPin);
   }
  //---------------------------------------------------------------------------//
  //                                  MENU methods                             //
  //---------------------------------------------------------------------------//
  
  /*
  * Print html format for user interface with the menu
  * @WiFiCLient, the user
  */
  void menuHtml(WiFiClient cl) { // Id = 0;
    cl.println("<br><h1>WELCOME TO SMART HOUSE V0</h1>");
    cl.println("<h4>Select menu</h4>");
    cl.println("<form method=\"GET\">");
    cl.println("<select name=\"menu\">");
    cl.println("<option value=\"10\">Geyser</option>");
    cl.println("<option value=\"20\">Temperature</option>");
    cl.println("<option value=\"30\">Security</option>");
    cl.println("<option value=\"40\" selected>Alarm</option>");
    cl.println("</select>");
    cl.println("<br><br>");
    cl.println("<input type=\"submit\" value=\"Next\">");
    cl.println("</form>");
  }
  
  /*
   *  Menu coding
   */
   void menuInput(WiFiClient cl) {
     if(isHtmlValue(cl,"menu"))iD = idElement(cl,"menu");
     if(iD==10){ // Geyser was selected
        if(gOn)iD=11; // Jump to the geyserOn page
        else if(gProgram)iD=12; // Jump to the geyserProgrammed page
      }
      else if(iD==20);
      else if(iD==30){
        if(sOn)iD=31; // Jump to the geyserOn page
        else if(sProgram)iD=32; // Jump to the geyserProgrammed page
      }
      else if(iD==40){
        if(aNor)iD=41;
        else if(aSm)iD=42;
        else if(aNorOn)iD=43;
        else if(aSmOn)iD=44;
      }
     else iD = 0;
   }
    // Time
    /*
     *  Check if a minute has passed
     */
    boolean mTime(){
      if(millis()%10000==0 ){
              return true;
            }
           else{return false;}
    }
    
