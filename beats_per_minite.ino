#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 


const char *ssid = "Heart on Your Wrist";
const char *password = "password";
// int stateLED = LOW;
String page = "";
double data;
    int UpperThreshold = 518;
    int LowerThreshold = 490;
    int reading = 0;
    float BPM = 0.0;
    bool IgnoreReading = false;
    bool FirstPulseDetected = false;
    unsigned long FirstPulseTime = 0;
    unsigned long SecondPulseTime = 0;
    unsigned long PulseInterval = 0;

ESP8266WebServer server(80);

const String highrate = "<b>High Rate</b><br/>\n";
const String lowrate = "<b>Low Rate</b><br/>\n";
String con;

void setup() 
{
     pinMode(A0, INPUT);
  delay(1000);
  Serial.begin(115200);

    WiFi.softAP(ssid, password);

    IPAddress apip = WiFi.softAPIP();
    Serial.print("visit: \n");
    Serial.println(apip);
    server.on("/", []()
    {
    page = "<meta http-equiv=\"refresh\" content=\"01\"><center><h1>Heart on Your wrist </h1><h3>Data:</h3> <h4>"+String(data)+"</h4><h3>BPM:</h3> <h4>"+float(BPM)+"</h4><h3>Condition:</h3> <h4>"+String(con)+"</h4></center>";
    server.send(200, "text/html", page);
  });
  
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(){
  data = analogRead(A0);
  if(data > UpperThreshold && IgnoreReading == false){
        if(FirstPulseDetected == false){
          FirstPulseTime = millis();
          FirstPulseDetected = true;
        }
        else{
          SecondPulseTime = millis();
          PulseInterval = SecondPulseTime - FirstPulseTime;
          FirstPulseTime = SecondPulseTime;
        }
        IgnoreReading = true;
      }

      // Heart beat trailing edge detected.
      if(data< LowerThreshold){
        IgnoreReading = false;
      }  
      BPM = (1.0/PulseInterval) * 60.0 * 1000;
      if(BPM>0&&BPM<60)
      {
        con="Normal Condition";
        }
        if(BPM>60.0&&BPM<100.0)
        {
          con="Good Condition";
          }
          if(BPM>100.0&&BPM<150.0)
        {
          con="Critical Condition";
         send_sms();
          }
  server.handleClient();
  delay(50); 
}
void send_sms()
{
Serial.print("AT");
delay(3000);
Serial.println();
Serial.println("AT+CMGF=1");  // sets the SMS mode to text
delay(3000);
Serial.println();
Serial.print("AT+CMGS=");     // send the SMS number
Serial.print("\"+918511225062\"");
Serial.println();
delay(3000);
Serial.print("Patient condition very critical please contact");             //enter latitude in msg
Serial.println();   
delay(4000);
Serial.println();
Serial.write(26);    
}

