//7/10/12
#include <math.h>
#include <Ethernet.h>
#include <SPI.h>
#include <MemoryFree.h>
#include <urlParser.h>
#include <aJSON.h>

//int colorRGB[3];
const int LEDPinReds[3] = {
  2,5,8};
const int LEDPinBlues[3] = {
  3,6,9};
const int LEDPinGreens[3] = {
  4,7,13};
const int channels = 3;


//int delayVal = 50;
int fadeValues[3] = {
  0,0,0};
long intervals[3] = {
  50,50,50};

//functions for colorfade algorithm
int h_int; // i think this can be removed
float h; // i think this can be moved in the scope of the loop
int currentReds[3] = {
  0,0,0};
int currentGreens[3] = {
  0,0,0};
int currentBlues[3] = {
  0,0,0};

int blnFades[3] = {
  0,0,0};

//variables to hold override values.

int setReds[3] = {
  255,255,255};
int setGreens[3] = {
  255,255,255};
int setBlues[3] = {
  255,255,255};


long previousFadeMillis[3] = {
  0,0,0};
long currentMillis; 
int debug = 1;

byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0C, 0x00, 0x01 };
//IPAddress egServerIP (192,168,0,116); 

EthernetClient client;
EthernetServer server(80);
//String getRequest = "";

//urlParser quickTest = urlParser();

//function declarations
void h2rgb(float h, int &R, int &G, int &B);
void colorFade();
void refreshColors();
void setColor (int channel, int intRed, int intGreen, int intBlue);


void setup() {
  Serial.begin(57600); 
  aJsonObject *root,*fmt;
  root=aJson.createObject();

  //quickTest.setURLHeader("GET /rest/set/r/1 HTTP/1.1");
  //Serial.println(quickTest.parseURLHeader());
  //Serial.println(quickTest.returnPath(2));
  Serial.println("----restart----");
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("--------");



  if (Ethernet.begin(mac) == 0) {
    if (debug){    
      Serial.println("Failed to configure Ethernet using DHCP");
    }
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  server.begin();
  if (debug) {
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());

  }


  for (int i=0;i<channels;i++){

    pinMode(LEDPinReds[i], OUTPUT);
    pinMode(LEDPinGreens[i], OUTPUT);
    pinMode(LEDPinBlues[i], OUTPUT);
    blnFades[i] = 1;
  }




}

void loop() {



  currentMillis = millis();
  EthernetClient client = server.available();

  if (client) {
    urlParser objRequest = urlParser();
    Serial.println("----new client----");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        objRequest.appendHeader(c);


        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {

          // send a standard http response header

          Serial.println("---checckkkkk---");
          Serial.print("Memory Available:  ");
          Serial.println(freeMemory(), DEC);


          //once request is done, parse the headers
          //          objRequest.parseHeaders();
          Serial.print("path 1:");
          Serial.println(objRequest.returnPath(0));

          Serial.print("path 2:");
          Serial.println(objRequest.returnPath(1));

          Serial.print("path 3:");
          Serial.println(objRequest.returnPath(2));

          Serial.print("path 4:");
          Serial.println(objRequest.returnPath(3));

          Serial.print("path 5:");
          Serial.println(objRequest.returnPath(4));


          Serial.println("---endchecckkkkk---");

          //*
          if(objRequest.returnPath(0).equalsIgnoreCase("rest")){
            //restful request

            if(objRequest.returnPath(1).equalsIgnoreCase("set")){
              //set request 
              char carray[6];
              int setChannel = 0;
              if (objRequest.returnPath(2).length()>0){
                objRequest.returnPath(2).toCharArray(carray, sizeof(carray));
                setChannel = atoi(carray);
              }

              if(objRequest.returnPath(3).equalsIgnoreCase("red")){
                Serial.println("RED");
                if (objRequest.returnPath(4).length()>0){
                  objRequest.returnPath(4).toCharArray(carray, sizeof(carray));
                  setReds[setChannel] = atoi(carray);
                }


              }
              else if(objRequest.returnPath(3).equalsIgnoreCase("blue")){
                if (objRequest.returnPath(4).length()>0){
                  Serial.println("BLUE");
                  objRequest.returnPath(4).toCharArray(carray, sizeof(carray));
                  setBlues[setChannel] = atoi(carray);
                }


              }
              else if(objRequest.returnPath(3).equalsIgnoreCase("green")){
                if (objRequest.returnPath(4).length()>0){
                  Serial.println("Green");
                  objRequest.returnPath(4).toCharArray(carray, sizeof(carray));
                  setGreens[setChannel] = atoi(carray);
                }


              }
              else if(objRequest.returnPath(3).equalsIgnoreCase("fade")){
                if (objRequest.returnPath(4).length()>0){
                  objRequest.returnPath(4).toCharArray(carray, sizeof(carray));
                  blnFades[setChannel] = atoi(carray);
                }

              }




            }
            else if(objRequest.returnPath(1).equalsIgnoreCase("get")){
              //get request


            }
          }

          //*/
          refreshColors();


          //now print reply:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Transfer-Encoding: UTF-8");
          client.println("Connnection: close");
          client.println();
          //client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");




          client.print("{"); //open JSON

          client.print("\"");
          client.print("millis");
          client.print("\":");
          client.print(millis());

          client.print(",");

          client.print("\"");
          client.print("fadeOn");
          client.print("\":");
          client.print("[");
          client.print(blnFades[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(blnFades[i]);        
          }
          client.print("]");





          client.print(",");

          client.print("\"");
          client.print("intervals");
          client.print("\":");

          client.print("[");
          client.print(intervals[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(intervals[i]);        
          }
          client.print("]");   

          client.print(",");

          client.print("\"");
          client.print("setReds");
          client.print("\":");
          client.print("[");
          client.print(setReds[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(setReds[i]);        
          }
          client.print("]");

          client.print(",");

          client.print("\"");
          client.print("setGreens");
          client.print("\":");
          client.print("[");
          client.print(setGreens[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(setGreens[i]);        
          }
          client.print("]");  

          client.print(",");

          client.print("\"");
          client.print("setBlues");
          client.print("\":");
          client.print("[");
          client.print(setBlues[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(setBlues[i]);        
          }
          client.print("]");

          client.print(",");

          client.print("\"");
          client.print("curReds");
          client.print("\":");
          client.print("[");
          client.print(currentReds[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(currentReds[i]);        
          }
          client.print("]");

          client.print(",");

          client.print("\"");
          client.print("curGreens");
          client.print("\":");
          client.print("[");
          client.print(currentGreens[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(currentGreens[i]);        
          }
          client.print("]");

          client.print(",");

          client.print("\"");
          client.print("curBlues");
          client.print("\":");

          client.print("[");
          client.print(currentBlues[0]);        
          for(int i = 1; i<channels; i++){
            client.print(",");
            client.print(currentBlues[i]);        
          }
          client.print("]");

          client.print("}"); //close JSON



          //client.print("<millis>");
          //client.print(millis());
          //client.println("</millis>");

          break;
        } // end if (c == '\n' && currentLineIsBlank) 
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }//end else if
      } // end  if (client.available()) {
    } // end while (client.connected()) {
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("----client disconnected----");



  }// end if(client)







  //  colorFade();

  refreshColors();



}//end Loop

void refreshColors(){
  for (int i = 0; i < channels; i++){
    if (blnFades[i]){

      if (currentMillis > (previousFadeMillis[i] + intervals[i]) || (currentMillis) < previousFadeMillis[i]){  

        if (fadeValues[i] >1024) fadeValues[i] = 0;
        h = ((float)fadeValues[i])/1024;
        h_int = (int) 360*h; // i don't think this does anything
        h2rgb(h,currentReds[i],currentGreens[i],currentBlues[i]);

        fadeValues[i] +=5;
        previousFadeMillis[i] = currentMillis;

      }
      //delay(delayVal);
    }
    else {
      //    for (int i = 0; i < channels; i++){
      currentReds[i] = setReds[i];
      currentGreens[i] = setGreens[i];
      currentBlues[i] = setBlues[i]; 
      //  }

    }//end blnFade
    setColor(i,currentReds[i],currentGreens[i],currentBlues[i]);

  }//end for

}




void h2rgb(float H, int& R, int& G, int& B) {
  int var_i;
  float S=1, V=1, var_1, var_2, var_3, var_h, var_r, var_g, var_b;
  if ( S == 0 ) {
    R = V * 255;
    G = V * 255;
    B = V * 255;
  } 
  else {
    var_h = H * 6;
    if ( var_h == 6 ) var_h = 0;
    var_i = int( var_h ) ;
    var_1 = V * ( 1 - S );
    var_2 = V * ( 1 - S * ( var_h - var_i ) );
    var_3 = V * ( 1 - S * ( 1 - ( var_h - var_i ) ) );

    if ( var_i == 0 ) {
      var_r = V     ;
      var_g = var_3 ;
      var_b = var_1 ;
    } 
    else if ( var_i == 1 ) {
      var_r = var_2 ;
      var_g = V     ;
      var_b = var_1 ;
    } 
    else if ( var_i == 2 ) {
      var_r = var_1 ;
      var_g = V     ;
      var_b = var_3 ;
    } 
    else if ( var_i == 3 ) {
      var_r = var_1 ;
      var_g = var_2 ;
      var_b = V     ;
    } 
    else if ( var_i == 4 ) {
      var_r = var_3 ;
      var_g = var_1 ;
      var_b = V     ;
    } 
    else {
      var_r = V     ;
      var_g = var_1 ;
      var_b = var_2 ;
    }
    R = (1-var_r) * 255;
    G = (1-var_g) * 255;
    B = (1-var_b) * 255;
  }
}

void setColor (int channel, int intRed, int intGreen, int intBlue) {

  analogWrite(LEDPinReds[channel], intRed);     
  analogWrite(LEDPinGreens[channel], intGreen);     
  analogWrite(LEDPinBlues[channel], intBlue);     

}





