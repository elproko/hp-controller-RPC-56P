#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "time.h"

// Please set proper WIFI settings first !
#include "wifi_settings.h"

#define LED_PWR 22

#define LED_PIN1 17
#define LED_PIN2 18
#define LED_PIN3 19
#define LED_PIN4 21
#define LED_PIN5 15
//#define LED_PIN5 22
#define LED_PIN6 23
//#define LED_PIN2 15


#define  MAXTAB 701


#define INOUT_INTERVAL 600
#define TOPBOTTOM_INTERVAL 300
#define WATER_INTERVAL 300
#define HIGHSPEED_INTERVAL 60;

/*
#define INOUT_INTERVAL 1
#define TOPBOTTOM_INTERVAL 1
#define WATER_INTERVAL 1
#define HIGHSPEED_INTERVAL 1;
*/
struct HPmeasure{
  short int temp;
  byte h;
  byte m;
  byte s;
  byte day;
  byte month;
  byte year;
};

struct HPstateRecord{     //alarms & relays
  byte state;
  byte h;
  byte m;
  byte s;
  byte day;
  byte month;
  byte year;
};

unsigned long tdiff=0;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

IPAddress local_IP(192, 168, 1, 45);
IPAddress gateway(192, 168, 1, 3);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 3);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

String curTime;

bool migaj=false;
int licznik = 0;
int licznikpwr = 0;
int cmdlen = 0;
int pwrblinked=0;
int blinked=0;

String pumpStatus = "";

String inputString = "";         // a String to hold incoming data

String lastTime="";
int lastSecWater=13000;
int lastSecInOut=13000;
int lastSecTopBottom=13000;

byte lastAlm=255;
byte lastRel=255;

byte statRelays=0;
int statInTemp = 145;
int statOutTemp = 145;
int statGzTemp = 157;
int statCwuTemp = 158;
int statDzTemp = 159;
byte statAlarms = 0;


HPmeasure hpmCurrentROOM;
HPmeasure hpmCurrentOUT;
HPmeasure hpmCurrentTOP;
HPmeasure hpmCurrentBOTTOM;
HPmeasure hpmCurrentWATER;
HPstateRecord hpmCurrentALM;
HPstateRecord hpmCurrentREL;

 HPmeasure      BOTTOMmeasures[MAXTAB]; //bottom source
 HPmeasure      TOPmeasures[MAXTAB];    //top source
 HPmeasure      OUTmeasures[MAXTAB];    //outside temp
 HPmeasure      ROOMmeasures[MAXTAB];   //inside/room
 HPmeasure      WATERmeasures[MAXTAB];  // user's hot water
 HPstateRecord RELlist[MAXTAB];        //relays states list
 HPstateRecord ALMlist[MAXTAB];        //alarms states list

 int wrptrBOTTOM=0;
 int wrptrTOP=0;
 int wrptrOUT=0;
 int wrptrROOM=0;
 int wrptrWATER=0;
 int wrptrREL=0;
 int wrptrALM=0;
 

int wrptr;


String unk="";
String tmp;

//globalne do przechowywania czasu
struct tm TT;

WebServer server(80);


const String WERSJA = "Pompka w Zielistkach v0.57";

const String JSINCLUDE =
"\n  <!-- Plotly.js -->"
"\n  <script src='https://cdn.plot.ly/plotly-latest.min.js'></script>"    // Java script for defining chart functions
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<script>"
    "function check(form){"
    "if(form.userid.value=='admin' && form.pwd.value=='admin'){ window.open('/serverIndex')"
    "}else{ alert('Error Password or Username')/*displays error message*/ }"
    "}"
"</script>";

void printLocalTime()
{
//  struct tm timeinfo;
  if(!getLocalTime(&TT)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&TT, "%A, %B %d %Y %H:%M:%S");
}

String getTm(byte co){
  String wy=String(co);
  if (wy.length()<2)  wy="0"+wy; 
  return(wy);
}

String getStrLocalTime()
{
  if(!getLocalTime(&TT)){
    return("Failed to obtain time"); 
  }

int _hour = TT.tm_hour;
int _min  = TT.tm_min;
int _sec  = TT.tm_sec;

int _day = TT.tm_mday;
int _month = TT.tm_mon + 1;
int _year = TT.tm_year + 1900;
 
  return(String(_year) + "-"+getTm(_month)+"-"+getTm(_day)+" "+getTm(_hour)+":"+getTm(_min)+":"+getTm(_sec));
 
}


String getSeries(String sVar,String sName, HPmeasure arr[],int ile,String sOpt){
  String prolog,xs,ys,epilog;
prolog="var "+sVar+" = {\n";
 xs="x: [";
 ys="y: [";
 for(int x=0;x<ile;x++){
  xs=xs+"'"+(1900+arr[x].year)+"-"+getTm(arr[x].month)+"-"+getTm(arr[x].day)+" "+getTm(arr[x].h)+":"+getTm(arr[x].m)+":"+getTm(arr[x].s)+"',";
  ys=ys+ (double)arr[x].temp/10+",";
 }
 xs=xs+"],\n";
 ys=ys+"],\n";
epilog="  type: 'scatter',\n  name: '"+sName+"'"+"\n"+sOpt+"};\n";
return(prolog+xs+ys+epilog);
}

String getSeries(String sVar,String sName, HPstateRecord arr[],int ile,String sOpt){
  String prolog,xs,ys,epilog;
prolog="var "+sVar+" = {\n";
 xs="x: [";
 ys="y: [";
 for(int x=0;x<ile;x++){
  xs=xs+"'"+(1900+arr[x].year)+"-"+getTm(arr[x].month)+"-"+getTm(arr[x].day)+" "+getTm(arr[x].h)+":"+getTm(arr[x].m)+":"+getTm(arr[x].s)+"',";
  ys=ys+arr[x].state+",";
 }
 xs=xs+"],\n";
 ys=ys+"],\n";
epilog="  type: 'scatter',\n  name: '"+sName+"'"+"\n "+sOpt+"};\n";
return(prolog+xs+ys+epilog);
}

String plotlyData(int tr,int br,int rr,int orr,int wr,int ar,int relr){

String wy="\n<script>\n";   
 wy=wy+getSeries("s1","TOP_src",TOPmeasures,tr,"");
 wy=wy+getSeries("s2","BOTTOM_src",BOTTOMmeasures,br,"");
 wy=wy+getSeries("s3","ROOM_temp",ROOMmeasures,rr,"");
 wy=wy+getSeries("s4","OUTSIDE_temp",OUTmeasures,orr,"");
 wy=wy+getSeries("s5","WATER_temp",WATERmeasures,wr,"");
 wy=wy+getSeries("s6","ALARMS",ALMlist,ar,",line: {shape: 'hv'}");
 wy=wy+getSeries("s7","RELAYS",RELlist,relr,",line: {shape: 'hv'}");
 wy=wy+"var layout = { width: 1800,  height: 800 };\n"
       "var data = [s6,s7,s1,s2,s3,s4,s5];\n"
       "Plotly.newPlot('plotly1div', data, layout);\n</script>\n";
return(wy);
}


String plotlyDataJs(int number,int tr){

String wy="";   
 wy=wy+getSeries("s1","TOP_src",TOPmeasures,tr,"");
 wy=wy+getSeries("s2","BOTTOM_src",BOTTOMmeasures,tr,"");
 wy=wy+getSeries("s3","ROOM_temp",ROOMmeasures,tr,"");
 wy=wy+getSeries("s4","OUTSIDE_temp",OUTmeasures,tr,"");
 wy=wy+getSeries("s5","WATER_temp",WATERmeasures,tr,"");
 wy=wy+getSeries("s6","ALARMS",ALMlist,tr,",line: {shape: 'hv'}");
 wy=wy+getSeries("s7","RELAYS",RELlist,tr,",line: {shape: 'hv'}");
 wy=wy+"\n<script>\n var layout = { width: 1800,  height: 800 };\n"
       " var data = [s6,s7,s1,s2,s3,s4,s5];\n"
       "Plotly.newPlot('plotly1div', data, layout);\n</script>\n";
return(wy);
}

String onoffIndex(String aWersja,String aPumpStatus){  
String wy="<html lang='pl' dir='ltr'>"
 "<head><meta charset='utf-8'>"
 +JSINCLUDE+
 "</head>"
 "<body>"
    "<table width='90%' bgcolor='A09F9F' align='center' style='font-size:2em;'>"
        "<tr>"
            "<td colspan=4>"
                "<center><b>"+aWersja+" "+getStrLocalTime()+"</b></center>"              
                "<center>Status: "+aPumpStatus+"</center>"                               
                "<center><a href='/login'>login</a></center>"                
            "</td>"                    
        "</tr>"
      /*  "<tr>"
        "<td>Klatka schodowa:</td>"
        "<td><a href='/on1'>włącz</a></td>"
        "<td><a href='/off1'>wyłącz</a></td>"
        "<td><a href='/blink1'>migaj</a><br></td>"
        "</tr>" 
        "<tr><td colspan=4>&nbsp;</td></tr>"    */  
    "</table>"
"<div id='plotly1div'><!-- Plotly chart will be drawn inside this DIV --></div>"
+plotlyData(wrptrTOP,wrptrBOTTOM,wrptrROOM,wrptrOUT,wrptrWATER,wrptrALM,wrptrREL)+
 "</body>"
 "</html>";
 return(wy);
}

/*
 * Login page
 */

const String loginIndex = 
 "<html lang='pl' dir='ltr'>"
 "<head><meta charset='utf-8'>"
 +JSINCLUDE+
 "</head>"
 "<body>"
 "<form name='loginForm'>"
    "<table width='50%' bgcolor='A09F9F' align='center' style='font-size:2em;'>"
        "<tr>"
            "<td colspan=2>"
                "<center><b>"+WERSJA+" Logowanie "+getStrLocalTime()+"</b></center>"
                "<br>"
            "</td>"          
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
 "</body>"
 "</html>";
 
/*
 * Server Index Page
 */
String serverIndex(String aWersja,String aPumpStatus){  
String wy="<html lang='pl' dir='ltr'>"
 "<head><meta charset='utf-8'>"
 +JSINCLUDE+
"</head>"
"<body>"
    "<table width='50%' bgcolor='A09F9F' align='center' style='font-size:2em;'>"
        "<tr>"
            "<td colspan=2>"
                "<center><b>"+WERSJA+" Zaawansowane "+getStrLocalTime()+"</b></center>"
                "<br>"
                "<center>Status: "+aPumpStatus+"</center>"                               
            "</td>"
            "<br>"           
        "</tr>"
        "<tr><td><a href='/restart'>restart</a></td>"
           "<td><a href='/onoff'>włącz/wyłącz</a></td>"
        "</tr>"
        "<tr><td colspan=4>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
        "</td><tr/>"      
    "</table>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>"
 "</body>"
 "</html>";
 return(wy);
}

void mypwrblink(void){
  if (pwrblinked==0) {
    digitalWrite(LED_PWR, HIGH);
    pwrblinked=1;
  } else {
    digitalWrite(LED_PWR, LOW);
    pwrblinked=0;  
  }
}


HPmeasure setMeasure(int aTemp,tm aTT){
 HPmeasure result;
  result.temp=aTemp;
  result.h=aTT.tm_hour;
  result.m=aTT.tm_min;
  result.s=aTT.tm_sec;
  result.day=aTT.tm_mday;
  result.month=aTT.tm_mon+1;
  result.year=aTT.tm_year;
  return(result); 
}

HPstateRecord setStateRecord(int aState,tm aTT){
 HPstateRecord result;
  result.state=aState;
  result.h=aTT.tm_hour;
  result.m=aTT.tm_min;
  result.s=aTT.tm_sec;
  result.day=aTT.tm_mday;
  result.month=aTT.tm_mon+1;
  result.year=aTT.tm_year;
  return(result); 
}

/*
 * setup function
 */
void setup(void) {

 
  Serial.begin(19200);
  Serial2.begin(19200);  

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  licznik=0;

  pinMode(LED_PWR, OUTPUT);
  digitalWrite(LED_PWR, LOW);

    Serial.println("\r\n"+WERSJA+"\r\n");

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure static IP");
  }
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
//  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("trying to connect to Wifi...");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

 //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  printLocalTime();

  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", onoffIndex(WERSJA,pumpStatus));
  });

  server.on("/test.svg", drawGraph);

  server.on("/login", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });

  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex(WERSJA,pumpStatus));
  });

  server.on("/onoff", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", onoffIndex(WERSJA,pumpStatus));
  });


  server.on("/off1", HTTP_GET, []() {
    digitalWrite(LED_PIN1, HIGH);
//    server.sendHeader("Connection", "close");
    int ttmp=licznik;
    String stmp=onoffIndex(WERSJA,pumpStatus+" "+tdiff);
    tdiff=abs(licznik-ttmp);
    server.send(200, "text/html", stmp);
  });

/*
 *   if (server.hasArg("content") ){
    // Content has been edited. Save.
    File file = SPIFFS.open(pathname.c_str(), "w");
    String content = server.arg("content");
 */


  server.on("/restart", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", "<html><head/><body>Restart... <a href='/serverIndex'>go back</a></body></html>");
    delay(100);
    ESP.restart();
  });

  server.on("/data", HTTP_GET, []() {
     server.sendHeader("Connection", "close");
     sendArrayData();
  });

  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}


void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void sendArrayData() {
  String out = plotlyData(MAXTAB,MAXTAB,MAXTAB,MAXTAB,MAXTAB,MAXTAB,MAXTAB);
  server.send(200, "text/plain", out);
}


int safeInc(int we,int boundary){
  we++;
  if (we<boundary) { return(we); } else { return(0); }
}

int safeInc(int we,int boundary,HPmeasure aTab[]){
  we++;
  if (we<boundary) {
    return(we);
   } else { 
    int x=0;
    int y=boundary-1;
    while (x<y){   //shift left whole array
      aTab[x]=aTab[++x];
    }
    return(y);
   }
}

int safeInc(int we,int boundary,HPstateRecord aTab[]){
  we++;
  if (we<boundary) {
    return(we);
   } else { 
    int x=0;
    int y=boundary-1;
    while (x<y){   //shift left whole array
      aTab[x]=aTab[++x];
    }
    return(y);
   }
}

void loop(void) {
  getSerialData();
  server.handleClient();

 /* 
  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial2.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }
  if (Serial2.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial2.read());   // read it and send it out Serial (USB)
  }  
  */

  
  delay(1);
licznik++;
licznikpwr++;

if (licznikpwr==500){
  mypwrblink();
  licznikpwr=0;


 curTime=getStrLocalTime(); //here TT global struct is filled in

 if (curTime!=lastTime) {
  lastTime=curTime;

   int inoutInterval     = INOUT_INTERVAL;
   int topbottomInterval = TOPBOTTOM_INTERVAL;
   int waterInterval     = WATER_INTERVAL;
   int highspeedInterval = HIGHSPEED_INTERVAL;

   if (statRelays>0) topbottomInterval=highspeedInterval; // lower top/bottom sources if any relay is set
   if ((statRelays & 8) == 8) waterInterval=highspeedInterval; // lower water interval if water relay is st to on

int toinc=0;

   if (lastAlm!=statAlarms){ // zapisz dokladnie kiedy alarm
       lastAlm=statAlarms;
       toinc=1;
   }

   if (lastRel!=statRelays){ // zapisz dokladnie kiedy zmiana REL
       lastRel=statRelays;
       toinc=1;
   }

if (toinc==1){
        lastSecWater=13000;//TT.tm_min*60+TT.tm_sec; //set to old to force ALL intervals
        lastSecInOut=lastSecWater;
        lastSecTopBottom=lastSecWater;
        
       RELlist[wrptrREL]=hpmCurrentREL;        
       wrptrREL=safeInc(wrptrREL,MAXTAB,RELlist);   

       ALMlist[wrptrALM]=hpmCurrentALM;       
       wrptrALM=safeInc(wrptrALM,MAXTAB,ALMlist);         
}



  if (abs((TT.tm_min*60+TT.tm_sec)-lastSecWater)>=waterInterval) { //getStrLocalTime() sets the global TT.tm_sec 
       lastSecWater=TT.tm_min*60+TT.tm_sec;

       WATERmeasures[wrptrWATER]=hpmCurrentWATER;
       wrptrWATER=safeInc(wrptrWATER,MAXTAB,WATERmeasures);   
  }

  if (abs((TT.tm_min*60+TT.tm_sec)-lastSecTopBottom)>=topbottomInterval) { //getStrLocalTime() sets the global TT.tm_sec 
       lastSecTopBottom=TT.tm_min*60+TT.tm_sec;

       TOPmeasures[wrptrTOP]=hpmCurrentTOP;
       wrptrTOP=safeInc(wrptrTOP,MAXTAB,TOPmeasures);   

       BOTTOMmeasures[wrptrBOTTOM]=hpmCurrentBOTTOM;
       wrptrBOTTOM=safeInc(wrptrBOTTOM,MAXTAB,BOTTOMmeasures);   
  }
  

  if (abs((TT.tm_min*60+TT.tm_sec)-lastSecInOut)>=inoutInterval) { //getStrLocalTime() sets the global TT.tm_sec 
       lastSecInOut=TT.tm_min*60+TT.tm_sec;

       ROOMmeasures[wrptrROOM]=hpmCurrentROOM;
       wrptrROOM=safeInc(wrptrROOM,MAXTAB,ROOMmeasures);   

       OUTmeasures[wrptrOUT]=hpmCurrentOUT;
       wrptrOUT=safeInc(wrptrOUT,MAXTAB,OUTmeasures);   

       //check connection...
       if (WiFi.status() != WL_CONNECTED) {
         Serial.println("...reconnecting...");
         WiFi.begin(ssid, password);
       }      
  }
 } 
}

  if (licznik>=50000){
    licznik=0; 
  }
}

unsigned long Str2Hex(String we){

  unsigned long ret=0;
  int slen=we.length();
 // slen--;
  int pot=1;
  int a;
  int w;
  for (int x=slen;x--;x==0){
     a=(int)we[x];
     if (a>=48 && a<58) w=a-48;
     if (a>=65 && a<71) w=a-55;
     if (a>=97 && a<103) w=a-87;
     ret=ret+w*pot;
     pot=pot<<4;
  }
  return(ret);
}

void getSerialData() {
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    if (inChar == ':') {
       inputString = "";
       cmdlen=0;          
    }
    // add it to the inputString:    
    if (inChar != '\r' && inChar !='\n') {
      inputString += inChar;
      cmdlen++;
    }
     if (inChar == '\r') {    
      if (cmdlen>5) {
        int C = inputString.substring(3,5).toInt();  // uwaga na parametry: od - do
        int wart = inputString.substring(5).toInt(); 
        switch (C) {          
            case 1:    // relays:    0b=spr+cwu+dz, 0a=cwu+dz, 08=cwu
                statRelays = Str2Hex(inputString.substring(5));
                hpmCurrentREL =setStateRecord(statRelays,TT);
                inputString="";
                break;
            case 10:  // cmdgettemppok  ":0110"    - czujnik pokojowy                
                statInTemp = wart; //(double)wart/10;
                hpmCurrentROOM =setMeasure(statInTemp,TT);
                inputString="";
                break;
            case 11:  // cmdgettemppog  ":0111"    - pogodowka
                statOutTemp = wart; //(double)wart/10;
                hpmCurrentOUT =setMeasure(statOutTemp,TT);
                inputString="";
                break;
            case 12: //cmdgettempdz   ":0112"    - Dolne żródło
                statDzTemp = wart;//(double)wart/10;
                hpmCurrentBOTTOM =setMeasure(statDzTemp,TT);
                inputString="";
                break;
            case 13: //cmdgettempcwu  ":0113"    - CWU
                statCwuTemp = wart; //(double)wart/10;
                hpmCurrentWATER =setMeasure(statCwuTemp,TT);
                inputString="";
                break;
            case 14:  //  cmdgettempco   ":0114"    - CO
                statGzTemp = wart;//(double)wart/10;
                hpmCurrentTOP =setMeasure(statGzTemp,TT);
                inputString="";
                break;
            case 22: //cmdreadinput   ":0122"    - wejścia zezwoleń i alarmów
                statAlarms = 7-Str2Hex(inputString.substring(5));
                hpmCurrentALM =setStateRecord(statAlarms,TT);
                inputString="";
                break;
                
        }
               
        pumpStatus = inputString
        +" ROOM="+(double)statInTemp/10
        +" OUTSIDE="+(double)statOutTemp/10
        +" BOTTOM="+(double)statDzTemp/10
        +" WATER="+(double)statCwuTemp/10
        +" TOP="+(double)statGzTemp/10
        +" REL="+statRelays
        +" ALM="+statAlarms
        +" Ptr="+"r"+wrptrROOM+"o"+wrptrOUT+"b"+wrptrBOTTOM+"w"+wrptrWATER+"t"+wrptrTOP+"ar"+wrptrREL;
//        +" R="+ROOMmeasures[wrptrROOM].temp+" "+ROOMmeasures[wrptrROOM].h+":"+ROOMmeasures[wrptrROOM].m+":"+ROOMmeasures[wrptrROOM].s;
        
      }
    }

  }
}
