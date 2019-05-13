//-- Libraries Included --------------------------------------------------------------
#include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
// Define I/O Pins
#define     LED0      2          // WIFI Module LED
#define     LED2      D7          // Alarm
#define     BUZZER_PIN  D5
//------------------------------------------------------------------------------------
// Authentication Variables
char*       TKDssid;              // SERVER WIFI NAME
char*       TKDpassword;          // SERVER PASSWORD
//------------------------------------------------------------------------------------
WiFiServer  TKDServer(9001);      // THE SERVER AND THE PORT NUMBER
WiFiClient  TKDClient;            // THE SERVER CLIENT
//====================================================================================
int WaitACK = 1;

void setup()
{
  // Setting The Serial Port
  Serial.begin(9600);           // Computer Communication
  // Setting The Mode Of Pins
  pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
  pinMode(LED2, OUTPUT);          // Alarm
  pinMode(BUZZER_PIN, OUTPUT);          // Buzzer
  digitalWrite(BUZZER_PIN, HIGH);

  // Print Message Of I/O Setting Progress
  Serial.println();
  Serial.println("I/O Pins Modes Set .... Done");

  // Setting Up A Wifi Access Point
  SetWifi("TAKEONE", "");
}

//====================================================================================

void loop()
{

  IsClient();
  IsAlarm();

}

//====================================================================================

int Alarm(int dly)
{
  //digitalWrite(LED2, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  delay(dly);
  digitalWrite(BUZZER_PIN, HIGH);
}

//====================================================================================

void SetWifi(char* Name, char* Password)
{
  // Stop Any Previous WIFI
  WiFi.disconnect();

  // Setting The Wifi Mode
  WiFi.mode(WIFI_AP_STA);
  Serial.println("WIFI Mode : AccessPoint Station");

  // Setting The Access Point
  TKDssid      = Name;
  TKDpassword  = Password;

  // Starting The Access Point
  WiFi.softAP(TKDssid, TKDpassword);
  Serial.println("WIFI < " + String(TKDssid) + " > ... Started");

  // Wait For Few Seconds
  delay(1000);

  // Getting Server IP
  IPAddress IP = WiFi.softAPIP();

  // Printing The Server IP Address
  Serial.print("AccessPoint IP : ");
  Serial.println(IP);

  // Starting Server
  TKDServer.begin();
  Serial.println("Server Started");
}

//====================================================================================

void IsAlarm()
{
  if (TKDClient && TKDClient.connected())
  {
    Serial.println("Waiting alarm data");
    int timeOut = 500;
    while (timeOut > 0)
    {
      if (TKDClient.available())
      {
        // If Any Data Was Available We Read IT
        while (TKDClient.available())
        {
          // Read From Client
          //digitalWrite(LED2, LOW);
          String line = TKDClient.readStringUntil('\n');
          Serial.println(line);
          //digitalWrite(LED2, HIGH);
          if (line.indexOf("<ALARM>") > -1) {
            Alarm(250);
          } else {
            delay(250);
            //digitalWrite(LED2, LOW);
          }
        }
      } else {
        delay(250);
        timeOut -= 250;
      }
    }
  } else {
    WaitACK = true;
  }
}

//====================================================================================
void IsClient()
{
  if (!WaitACK) {
    return;
  }
  Serial.println("Waiting connection");
  if (digitalRead(LED0) == HIGH) digitalWrite(LED0, LOW);  
  if (TKDServer.hasClient())
  {
    if (WaitACK) {
      //find free/disconnected spot
      if (!TKDClient || !TKDClient.connected())
      {
        if (TKDClient) TKDClient.stop();
        TKDClient = TKDServer.available();
        WaitACK = 1;
        Serial.println("New Client");
      } 
      IsACK();
    } else {
       WiFiClient TKDClientX = TKDServer.available();
       TKDClientX.stop();
       Serial.println("Client allready assigned! so new connection is closing");
    }
  } else {
      // This LED Blinks If No Clients Where Available
      Serial.println("Waiting connection");
      digitalWrite(LED0, HIGH);
      delay(250);
      digitalWrite(LED0, LOW);
      delay(250);
  }

}

//====================================================================================

void IsACK()
{
  if (TKDClient && TKDClient.connected())
  {

    int timeOut = 5000;
    while (WaitACK && timeOut > 0)
    {
      Serial.println("Waiting ACK");      
      if (TKDClient.available())
      {
        // If Any Data Was Available We Read IT
        while (TKDClient.available())
        {
          // Read From Client
          String line = TKDClient.readStringUntil('\n');
          Serial.println(line);
          if (line.indexOf("<ACK>") > -1) {
            // Reply To Client
            TKDClient.println("<OK>");
            TKDClient.flush();
            digitalWrite(LED0, HIGH);
            WaitACK = 0;
          }
        }
      } else {
        delay(100);
        timeOut -= 100;
      }
    }

    //ACK gelmediyse baglantiyi dusur
    if (WaitACK) {
      TKDClient.stop();
    }
  }
}

//====================================================================================
