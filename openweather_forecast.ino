   //////////////////////////////////////////////
  //    Art Deco Weather Forecast Display     //
 //                                          //
//           http://www.educ8s.tv           //
/////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "";      // SSID of local network
const char* password = "";   // Password on network
String APIKEY = "";
String CityID = "6356734"; //Cáceres, España
int TimeZone = 2; //GMT +2

WiFiClient client;
char servername[]="api.openweathermap.org";  // remote server we will connect to
String result;

boolean night = false;
int  counter = 360;
String weatherDescription ="";
String weatherLocation = "";
float Temperature;

extern  unsigned char  cloud[];
extern  unsigned char  thunder[];
extern  unsigned char  wind[];

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  }
  Serial.println("Connected");

}

void loop() {

    if(counter == 360) //Get new data every 30 minutes
    {
      counter = 0;
      getWeatherData();
      
    }else
    {
      counter++;
      delay(5000);
      Serial.println(counter); 
    }
}

void getWeatherData() //client function to send/receive GET request data.
{
  Serial.println("Getting Weather Data");
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/forecast?id="+CityID+"&units=metric&cnt=1&lang=es&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
 
    Serial.println("Waiting for data");

  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}

String location = root["city"]["name"];
String temperature = root["list"]["main"]["temp"];
String weather = root["list"]["weather"]["main"];
String description = root["list"]["weather"]["description"];
String idString = root["list"]["weather"]["id"];
String timeS = root["list"]["dt_txt"];

//timeS = convertGMTTimeToLocal(timeS);

int length = temperature.length();
if(length==5)
{
  temperature.remove(length-1);
}

Serial.println(location);
Serial.println(weather);
Serial.println(temperature);
Serial.println(description);
Serial.println(temperature);
Serial.println(timeS);


}


String convertGMTTimeToLocal(String timeS)
{
 int length = timeS.length();
 timeS = timeS.substring(length-8,length-6);
 int time = timeS.toInt();
 time = time+TimeZone;

 if(time > 21 ||  time<7)
 {
  night=true;
 }else
 {
  night = false;
 }
 timeS = String(time)+":00";
 return timeS;
}
