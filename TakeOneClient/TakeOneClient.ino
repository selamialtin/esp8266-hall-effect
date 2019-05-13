//------------------------------------------------------------------------------------
  #include <Wire.h>
  #include <ESP8266WiFi.h>
//------------------------------------------------------------------------------------
// Defining I/O Pins
//------------------------------------------------------------------------------------
  #define       LED0      2         // WIFI Module LED
  #define       LED1      D7        // Connectivity With Client #1
  #define       TWI_FREQ  400000L   // I2C Frequency Setting To 400KHZ
//------------------------------------------------------------------------------------
// LED Delay Variables
//------------------------------------------------------------------------------------
  int           LEDState          = LOW;
  unsigned long CurrMillis        = 0;
  unsigned long PrevMillis        = 0;
  unsigned long Interval          = 1000;
//------------------------------------------------------------------------------------
// Authentication Variables
//------------------------------------------------------------------------------------
  char*         TKDssid;
  char*         TKDpassword;
  IPAddress     TKDServer(192,168,4,1);
  WiFiClient    TKDClient;
//====================================================================================
  char STATUS_NORMAL[] = "<NORMAL>";
  char STATUS_ALARM[] = "<ALARM>";


//------------------------------------------------------------------------------------
// Hall effect sensor Variables
//------------------------------------------------------------------------------------
  #define       DIGITAL_PIN      D6 
  #define       ANALOG_PIN       A0      
  int Alarm;
  int TimerPos;

  void setup() 
  {
    /* ----------------------------------------------------------------------
     * Setting The I2C Pins SDA, SCL
     * Because We Didnt Specify Any Pins The Defult
     * SDA = D4 <GPIO2>, SCL = D5 <GPIO14> For ESP8266 Dev Kit Node MCU v3
     --------------------------------------------------------------------- */
    Wire.begin();                   // Begginning The I2C
    
    // Setting Up The I2C Of The MPU9250 ------------------------------------
    Wire.setClock(TWI_FREQ);        // Setting The Frequency MPU9250 Require
        
    // Setting The Serial Port ----------------------------------------------
    Serial.begin(9600);           // Computer Communication
    
    // Setting The Mode Of Pins ---------------------------------------------
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light
    pinMode(LED1, OUTPUT);          // Indicator For Client #1 Connectivity
    pinMode(DIGITAL_PIN, INPUT);     
    digitalWrite(LED0, !LOW);       // Turn WiFi LED Off
    
    // Print Message Of I/O Setting Progress --------------------------------
    Serial.println("\nI/O Pins Modes Set .... Done");

    SetupWifiClient();

  }
//====================================================================================
  void SetupWifiClient()
  {
    WiFi.disconnect();
    // Starting To Connect --------------------------------------------------
    WiFi.mode(WIFI_STA);            // To Avoid Broadcasting An SSID
    WiFi.begin("TAKEONE");          // The SSID That We Want To Connect To

    // Printing Message For User That Connetion Is On Process ---------------
    Serial.println("!--- Connecting To " + WiFi.SSID() + " ---!");

    // WiFi Connectivity ----------------------------------------------------
    CheckConnectivity();            // Checking For Connection

    // Stop Blinking To Indicate Connected ----------------------------------
    digitalWrite(LED0, !HIGH);
    Serial.println("!-- Client Device Connected --!");

    // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strenght   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print  ("Server IP Address : ");
    Serial.println(TKDServer);
    Serial.print  ("Device IP Address : ");
    Serial.println(WiFi.localIP());

    // Conecting The Device As A Client -------------------------------------
    TKDRequest();
  }
//====================================================================================
  
  void loop()
  {
     // Read the digital interface
      int digitalVal = digitalRead(DIGITAL_PIN) ; 
      Serial.println(digitalVal); // print analog value
      if (digitalVal == LOW) // When magnetic field is present, Arduino LED is on
      {
        Alarm += 10;
      }
      CheckStatus();
      
      // Read the analog interface
      int analogVal = analogRead(ANALOG_PIN);
      Serial.println(analogVal); // print analog value
    
      delay(50);
  }
  
//====================================================================================

  void CheckStatus() {
    if (++TimerPos == 10) {
      Serial.println(Alarm);
    
      if (Alarm > 80) {
        SendStatus(STATUS_ALARM); 
      } else {
        SendStatus(STATUS_NORMAL);
      } 
  
      Alarm = 0;
      TimerPos = 0;
    }
  }

  void SendStatus(char* status)
  {
    if(TKDClient && TKDClient.connected()) {
      TKDClient.println(status);
      TKDClient.flush();
    } else {
      SetupWifiClient();
    }
  }

//====================================================================================

  void CheckConnectivity()
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      for(int i=0; i < 10; i++)
      {
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
  }

//====================================================================================

  void TKDRequest()
  {
    // First Make Sure You Got Disconnected
    TKDClient.stop();
    int success = 0;

    // If Sucessfully Connected Send Connection Message
    if(TKDClient.connect(TKDServer, 9001))
    {
      Serial.println ("Sending ACK"); 
      TKDClient.println    ("<ACK>");
      TKDClient.flush();
      // Wait For Few Seconds
      delay(500);
      if(TKDClient.available())
      {
        // If Any Data Was Available We Read IT
        while(TKDClient.available()) 
        {
           String line = TKDClient.readStringUntil('\n');
           Serial.println (line);           
           if (line.indexOf("<OK>")> -1) {
              digitalWrite(LED1, HIGH);
              success = 1;
           }
        }
      }


      if(!success) {
        Serial.println ("<ACK> could not sent, connection closing");   
        TKDClient.stop();
        delay(5000);
        TKDRequest();
      }
    }  
  }

//====================================================================================
