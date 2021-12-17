#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
MDNSResponder mdns;
String card,test;
const char* ssid = "Galaxy A127724";
const char* password = "ezaz0462";
ESP8266WebServer server(80);
#define SS_PIN D8
#define RST_PIN D0  
MFRC522 mfrc522(SS_PIN, RST_PIN);
//Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ) // enter ur username and password you want
    {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;

    }

    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  String content = "<!DOCTYPE html> <html>\n";
  content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  content += "<title>Rushin Door Lock</title>\n";
  content += "</head>\n";
  content += "<body style='background-color:#fff'><form action='/login' method='POST'><p  align ='center' style='font-size:300%;'><b>  Log In  </b></p><br>";
  content += " <p   align ='center' style='font-size:160%'><b> UserName: <input type='text' name='USERNAME' placeholder='user name' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><b>Password: <input type='password' name='PASSWORD' placeholder='password' required></b></p><br>";
  content += "<p  align ='center' style='font-size:160%'><input style='border: 1px solid red;padding:10px;color:#ff0000;border-radius:10px;font-size: 1.2em;' type='submit' name='submit' value='Log in'></form>" + msg + "</p><br> </body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  else
  {
  String content =  "<!DOCTYPE html> <html>\n";
  content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  content += "<title>Rushin Door Lock</title>\n";
  content += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  content += "body{margin-top: 50px;}\n";
  content += ".button {display: inline-block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 0px;cursor: pointer;border-radius: 10px;}\n";
  content += ".button-on {background-color: #1abc9c;}\n";
  content += ".button-on:active {background-color: #16a085;}\n";
  content += ".button-off {background-color: #34495e;}\n";
  content += ".button-off:active {background-color: #2c3e50;}\n";
  content += "p {font-size: 1.2em;color: #000;margin-bottom: 1%;}\n";
  content += ".log_out {border: 1px solid red;padding:10px;color:#ff0000;border-radius:10px;font-size: 1.2em;text-decoration: none;}\n";
  content += ".log_out:hover {background-color:#ff0000;color:#fff}\n";
  content += "</style>\n";
  content += "</head>\n";
  content += "<body>\n";
  content += "<b><h1><strong> Rushin Door Lock</strong></h1></b><hr><br><p align ='center'>Lock 1 <a class=\"button button-on\" href=\"switch0On\">Open</a>&nbsp;";
  content += "<br><p  align ='center'>Lock 2 <a class=\"button button-on\" href=\"switch1On\">OPEN</a>&nbsp;";
  content += "</body>\n";
  content += "</html>\n<br><br><br><br><br><br><br><br> ";
  content += "<br><p><marquee direction='right'>Developed by   Rushin Group </marquee></p>";
  content += "<footer><p><a href=\"cybele.ir\">cybele.ir</a></p></footer></body>";
  content += "<br><br>You can access this page until you <a class=\"log_out\" href=\"/login?DISCONNECT=YES\">Log Out</a></body></html>";
  server.send(200, "text/html", content);
  }
}

//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup(void)
{
  
  SPI.begin();
  mfrc522.PCD_Init();   // آماده کردن MFRC522 
  Serial.println("کارت خود را نزدیک ماژول کنید...");
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(10,INPUT);
  pinMode(2,INPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

 

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentification");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );


  server.on("/", []() {
    //
  });
  server.on("/switch0On", []() {
    //
    digitalWrite(4, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
    delay(500);
    handleRoot();
  });
 
  server.on("/switch1On", []() {
    //
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    delay(500);
    handleRoot();
  });
 
  


  server.begin();
  Serial.println("HTTP server started");

}
void loop(void)
{
  server.handleClient();
  
  
  while(digitalRead(2))
  {
    card="";
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // انتخاب و خواندن کارت
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //نمایش شناسه UID کارت
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
    while(digitalRead(2));
  }
  for(int i=1;i<12;i++)
  card+=content[i];
    card.toUpperCase();
    Serial.println(card);
  }
  while(digitalRead(10))
{
card="";
   
}
  //بررسی کارت جدید
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // انتخاب و خواندن کارت
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //نمایش شناسه UID کارت
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
    Serial.println();
//  Serial.print("Message : ");
  content.toUpperCase();
 
  if (content.substring(1) =="C6 3E 7D 2B" || content.substring(1) ==card) //شناسه UID کارت خود را اینجا جایگزین کنید
  {
    Serial.println("کارت مورد تایید است.");
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    delay(1000);
  }
  else
  {
    Serial.println(" کارت غیر مجاز");
    Serial.print(card);
    Serial.print(card);
    Serial.print(test);
    Serial.print(test);
    delay(1000);
  }
  
  
   
}
