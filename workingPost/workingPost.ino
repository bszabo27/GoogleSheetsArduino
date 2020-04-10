/*  HTTPS on ESP8266 to write to Gogole Sheets via script.google.com
 *  Version 1.0
 *  Author: Barna Szabo
 *  Github: 
 *  
 *  Copyrights of HTTPSRedirect and DebugMacros belong to:
 *  Copyright (C) 2018 Sujay Phadke <electronicsguy123@gmail.com>
 *  All rights reserved.
 *
 */

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// Fields to Customize for yourself only on Client-Side
const char* ssid = "<ssid>";
const char* password = "<pass>";
const char *GScriptId = "<scriptID>";


//Fields to customize for yourself Cleint-Side and adjust serverside.

//// Write and read is not dependent on the URI, but ont he HTTP Method used.
//// Altough /exec value matters on GETs since backend expects parameters and you cant send data via GET otherwise anyways
//// If you POST, the data should be included in the HTTP Body = "payload" 

String url = String("/macros/s/") + GScriptId + "/exec?value=customparam";
String url3 = String("/macros/s/") + GScriptId + "/exec?read";
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Munkalap1\", \
                    \"values\": ";

const char* host = "script.google.com";
const int httpsPort = 443;

String payload = "";

HTTPSRedirect* client = nullptr;

void setup() {
  Serial.begin(9600);
  Serial.flush();  
  
  Serial.printf("Running Setup \n");  
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);  
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }  
  Serial.println("WiFi on");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Failed to connect: ");
    Serial.println(host);
    Serial.println("Exiting..");
    return;
  }
  
  delete client;
  client = nullptr;
}

void loop() {
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;
  Serial.printf("New loop iteration \n");  
  
  if (!flag){   
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);     
    }
  }
  else{
    DPRINTLN("Error creating client object!");
    error_count = 5;
  }
  
  if (connect_count > MAX_CONNECT){    
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }  

  Serial.println("POST append Cica and Mica to spreadsheet:");
  // Payload base contains the baisc parameters like sheet name, and command.
  // Backend reads params and acts as needed. Adjsut both to customize.
  // payload = payload_base + "\" Cica,Mica\"}";
  
  String str1 = "Cirmos";
  String str2 = "Cica";
  payload = payload_base + "\"" + str1 + "," + str2 + "\"}";
  if(client->POST(url, host, payload)){
    ;
  }
  else{
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }  

  // I left this here if you need to get data from the sheet for any reason
  // Adjust returned values in the backend, it not specified in below code.
  // If need to be done dynamiclly, cell ID could be passed as a query param.  
   Serial.println("GET Data from backend-specified cell:");
  if (client->GET(url3, host)){
    ++connect_count;
  }
  else{
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }
  
  if (error_count > 3){
    Serial.println("Halting processor.."); 
    delete client;
    client = nullptr;   
    Serial.flush();
    ESP.deepSleep(0);
  }  
  
  delay(10000);
                          
}
