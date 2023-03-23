#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <LiquidCrystal_I2C.h> // INclude the display

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);

float temperature=20;

const int led = D0;
const int display_1=D1;
const int display_2=D2;


void handleRoot();  
void handleLED();  
void handleNotFound();

void setup() {
  delay(1000);
   Serial.begin(115200);
  delay(1000);

  pinMode(led, OUTPUT);
  digitalWrite(led,1);

  //display
  pinMode(D1, INPUT);
  pinMode(D2, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.print("Welcome");
  delay(2000);
  setLCD();
  // Connect to WiFi network
  Serial.println();
  wifiMulti.addAP("ONE", "oybq9273");  // add Wi-Fi networks you want to connect to
  //wifiMulti.addAP("DTUguest", "");  
  
  Serial.println();
  Serial.print("Connecting ...");
  //WiFi.begin(ssid, password);
 
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected to ");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("iot")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }



  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_POST, handleLED);
  server.on("/LEDBLINK", HTTP_POST, handleLEDBLINK);
  server.on("/change_temperature", HTTP_POST, change_temperature);
  
  server.onNotFound(handleNotFound);
    
  // Start the server
  server.begin();
  
  Serial.print("Server started"); 
}

void change_temperature(){
    temperature=temperature+1;
    setLCD();
    server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}
void setLCD(){
  char array1[]= "  Current Temp   ";
char array2[15];   
  lcd.setCursor(0,0);
  lcd.print(array1);
  lcd.setCursor(5,1);  // set the cursor to column 15, line 1
    
  lcd.print(temperature);
  
  char str_temp[6];

  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf(temperature, 4, 2, str_temp);
  sprintf(array2,"%s          ", str_temp);
}
void loop() {
  // Check if a client has connected
  server.handleClient();
  
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\">\
      </head><body><h1>Velkommen til denne WebServer</h1> \
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED\" style=\"width:500px; height:100px; font-size:24px\"></form>  \
      <form action=\"/LEDBLINK\" method=\"POST\" ><input type=\"submit\" value=\"Få LED til at blinke\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <input type=\"/number\" default=temperature> <form action=\"/change_temperature\" method=\"POST\" ><input type=\"submit\" value=\"change temperature\" style=\"width:500px; height:100px; font-size:24px\"></form>\
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
}

void handleLED() {                          // If a POST request is made to URI /LED
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleLEDBLINK() {                          // If a POST request is made to URI /LED
  for (int i =0;i<30;i++){
  digitalWrite(led,!digitalRead(led));      // Change the state of the LED
  delay(100);
  }
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
