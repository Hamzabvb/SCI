#include "StaticVar.h";
#include "Motors.h";

#include <TinyGPS++.h>      
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

struct GPS_coordinate {
  float lat;
  float lng;
};
struct GPS_coordinate coordinate ,poin ;

int speedCar = 400;         // 400 - 1023.
int speed_Coeff = 4;
long duration;
float distanceCm;
char side;

const char* ssid = "Rio"; //ssid of your wifi
const char* password = "hamza5620"; //password of your wifi

float starting_lat , starting_lng;
float end_lat=36.704735 , end_lng= 3.173469;
double d;

unsigned long previousMillis = 0; const long interval = 2000;  
int year , month , date, hour , minute , second, t;
String date_str , time_str , lat_str , lng_str, dist;
int pm;

WiFiServer server(80);
TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(Rx, Tx); // The serial connection to the GPS device (RX, TX)
TinyGPSCustom steerDirection(gps, "GPRMB", 3);

//--------------------------------------------------------
struct GPS_coordinate getGpsCoordinate();
float distance_obstacleCm ();
double distance_gpsM(float A_lat, float A_lng ,float B_lat, float B_lng)
{
  return gps.distanceBetween(A_lat,A_lng,B_lat,B_lng);
}

void goAhead();
void goBack();
void goRight();
void goLeft();
void stopRobot();

void angleRotation(int angle ,  char side );
void leftORright();

void WifiConnection();
void PreparehtmlPage();

//--------------------------------------------------------
void setup() {
 
 pinMode(ENA, OUTPUT);
 pinMode(ENB, OUTPUT);  
 pinMode(IN_1, OUTPUT);
 pinMode(IN_2, OUTPUT);
 pinMode(IN_3, OUTPUT);
 pinMode(IN_4, OUTPUT); 
 
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
 pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 
 
 Serial.begin(115200);
 ss.begin(9600);

//--------------------------------------------------------
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password); //connecting to wifi
 while (WiFi.status() != WL_CONNECTED)// while wifi not connected
 {
   delay(500);
   Serial.print("."); //print "...."
 }
 Serial.println("");
 Serial.println("WiFi connected");
 server.begin();
 Serial.println("Server started");
 Serial.println(WiFi.localIP());
//--------------------------------------------------------
 digitalWrite(IN_1, LOW);
 digitalWrite(IN_2, LOW);
 digitalWrite(IN_3, LOW);
 digitalWrite(IN_4, LOW);
//---------------------------------------------------------

 coordinate = getGpsCoordinate();
 poin = getGpsCoordinate();
 starting_lat= coordinate.lat;
 starting_lng= coordinate.lng;

}
void loop() {
   coordinate = getGpsCoordinate();
   
   
   unsigned long currentMillis = millis();
   
   if (currentMillis - previousMillis >= interval)
   {
    previousMillis = currentMillis;
    
   
   Serial.print("lat = ");Serial.print(lat_str);
   Serial.print(" lng = ");Serial.print(lng_str);   
   Serial.print(" distancegps = ");Serial.print(dist);
   Serial.print(" distanc obj = ") ;Serial.println(distance_obstacleCm());
   }
   //Serial.print(" obj <=> ") ;Serial.println(distanceCm);
  if (16 > 6)
  {
   
   angleRotation(90,'l');
   delay(5000);
    
  }
   
  // Prepare the response
  PreparehtmlPage();
  
}

void PreparehtmlPage()
{
  WiFiClient client = server.available(); // Check if a client has connected
  if (!client)
  {
   return;
  }  
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS DATA</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> GPS DATA</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td></tr> <tr> <th>distance</th> <td ALIGN=CENTER >";
  s += dist;
  s += "</td>  </tr> </table> ";
 
  s += "</body> </html>";
  
  client.print(s); // all the values are send to the webpage
  delay(10);

}

void leftORright ()
{
  int dl , dr;
  int speedi = speedCar;
  stopRobot();
  speedCar=300;
  angleRotation(90,'l');
  
  dl =  distance_obstacleCm ();
  if (dl > 60)
  {
    return;
  }
  angleRotation(180,'r');
  dr= distance_obstacleCm ();
  //delay(500);
  if (dl<dr)
  {
    delay(500);
    angleRotation(180,'r');
    //return 'l';
  }else{
    ;//return 'r';
  }
  speedCar = speedi;
}

void angleRotation(int angle ,  char side )
{
  stopRobot();
  int speedi = speedCar;
  speedCar = 200;  
  //delay(800);
  if((side == 'l')||(side=='L'))
  {
    goLeft();
  }
  else
  {
    goRight();
  }
  delay(12.5 * angle);
  stopRobot();
  speedCar = speedi;
   
}

struct GPS_coordinate getGpsCoordinate()
{
  struct GPS_coordinate c = {0,0};
  
   while (ss.available() >0)
   {
    if(gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        
        c.lat = gps.location.lat();
        lat_str = String(c.lat , 6);
        c.lng = gps.location.lng();
        lng_str = String(c.lng , 6);
        //Serial.print(lat_str+lng_str);        
        d = gps.distanceBetween(end_lat,end_lng , c.lat,c.lng);
        dist=String(d,6);
      }
      else
      {
        c.lat = 36.70;
        lat_str = String(c.lat , 6);
        c.lng = 3.15;
        lng_str = String(c.lng , 6);
      }
      //Serial.println();
      
      if (gps.date.isValid()) //check whether gps date is valid
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);// values of date,month and year are stored in a string
        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month); // values of date,month and year are stored in a string
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year); // values of date,month and year are stored in a string
      }
      if (gps.time.isValid())  //check whether gps time is valid
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
               
        if (hour >= 12)  // checking whether AM or PM
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second); //values of hour,minute and time are stored in a string
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
    
    }
   }
   
   
   return c;
}



float distance_obstacleCm (){
  
  digitalWrite(trigPin, LOW); // Clears the trigPin
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  return distanceCm;
}



void goLeft(){

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }
void goRight(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }

void goBack(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
  }
  
void goAhead(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }
void stopRobot(){  

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar);
}
 
void WifiConnection()
{
 
}
