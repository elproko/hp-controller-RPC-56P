#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "time.h"

// Please set proper WIFI settings first !
#include "wifi_settings.h"

//needed for SNTP & MQTT
static const char* ntpServer = "pool.ntp.org";
static const long  gmtOffset_sec = 3600;
static const int   daylightOffset_sec = 3600;


/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
//static const char* connectionString = ""; // conection string

static bool hasIoTHub = false;

#include "Esp32MQTTClient.h"

// Warning:
// C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\AzureIoT\src\az_iot\c-utility\pal\lwip\sntp_lwip.c 
// The file from path above needs to be patched according to info from:
// https://github.com/VSChina/ESP32_AzureIoT_Arduino/issues/18



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
int lastSecWater=13000;  //13000 is the longest time than longest interval we have
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


const String WERSJA = "Pompka w Zielistkach v0.591";

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

String wy=""; 
 wy=wy+getSeries("s1","TOP_src",TOPmeasures,tr,"");
 wy=wy+getSeries("s2","BOTTOM_src",BOTTOMmeasures,br,"");
 wy=wy+getSeries("s3","ROOM_temp",ROOMmeasures,rr,"");
 wy=wy+getSeries("s4","OUTSIDE_temp",OUTmeasures,orr,"");
 wy=wy+getSeries("s5","WATER_temp",WATERmeasures,wr,"");
 wy=wy+getSeries("s6","ALARMS",ALMlist,ar,",line: {shape: 'hv'}");
 wy=wy+getSeries("s7","RELAYS",RELlist,relr,",line: {shape: 'hv'}");
 wy=wy+"var layout = { width: 1800,  height: 800 };\n"
       "var data = [s6,s7,s1,s2,s3,s4,s5];\n"
       "Plotly.newPlot('plotly1div', data, layout);\n";
return(wy);
}


String plotlyDataJs(int number,int tr){

String wy;   
if (number==1) wy=getSeries("s1","TOP_src",TOPmeasures,tr,"");
if (number==2) wy=getSeries("s2","BOTTOM_src",BOTTOMmeasures,tr,"");
if (number==3) wy=getSeries("s3","ROOM_temp",ROOMmeasures,tr,"");
if (number==4) wy=getSeries("s4","OUTSIDE_temp",OUTmeasures,tr,"");
if (number==5) wy=getSeries("s5","WATER_temp",WATERmeasures,tr,"");
if (number==6) wy=getSeries("s6","ALARMS",ALMlist,tr,",line: {shape: 'hv'}");
if (number==7) wy=getSeries("s7","RELAYS",RELlist,tr,",line: {shape: 'hv'}");
return(wy);
}

String mainIndexStart(String aWersja,String aPumpStatus){  
 String wy="<html lang='pl' dir='ltr'>"
 "<head><meta charset='utf-8'>"
 +JSINCLUDE+
 "</head>"
 "<body>"
    "<table width='90%' bgcolor='A09F9F' align='center' style='font-size:2em;'>"
        "<tr>"
            "<td colspan=4>"
                "<center><b>"+aWersja+" "+getStrLocalTime()+"</b></center>"              
                "<center>"+aPumpStatus+"</center>"                               
                "<center><a href='/login'>login</a></center>"                
            "</td>"                    
        "</tr>"
    "</table>"
"<div id='plotly1div'><!-- Plotly chart will be drawn inside this DIV --></div>\n<script>\n";
 return(wy);
}

/*
 * Login page
 */
String loginIndex(String aWersja){ 
 return("<html lang='pl' dir='ltr'>"
 "<head><meta charset='utf-8'>"
 +JSINCLUDE+
 "</head>"
 "<body>"
 "<form name='loginForm'>"
    "<table width='50%' bgcolor='A09F9F' align='center' style='font-size:2em;'>"
        "<tr>"
            "<td colspan=2>"
                "<center><b>"+aWersja+" Logowanie "+getStrLocalTime()+"</b></center>"
                "<br>"
            "</td>"          
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
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
 "</html>");
} 
 
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

void ghp(String co){
Serial.println(co);
Serial.println(ESP.getFreeHeap());  
}

/*
 * setup function
 */
void setup(void) {
  Serial.begin(19200);

ghp("after com1");
  
  Serial2.begin(19200);  

ghp("after com2");

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

ghp("after wifi begin");

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

ghp("after IP");

  // use mdns for host name resolution
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

ghp("after mDNS");

 //init and get the time
//  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

ghp("after time config");

   delay(1500);

  printLocalTime();

ghp("after print local time");

  // get data few times because controller sends requests in different frequency per message type
  // we need here to ensure we have all types stored already
  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  getSerialData();
  
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN); //to force "chunked"
    //server.sendHeader("Connection", "close");
    server.send(200, "text/html", mainIndexStart(WERSJA,pumpStatus));

        server.sendContent(plotlyDataJs(1,wrptrTOP));
        server.sendContent(plotlyDataJs(2,wrptrBOTTOM));
        server.sendContent(plotlyDataJs(3,wrptrROOM));
        server.sendContent(plotlyDataJs(4,wrptrOUT));
        server.sendContent(plotlyDataJs(5,wrptrWATER));
        server.sendContent(plotlyDataJs(6,wrptrALM));
        server.sendContent(plotlyDataJs(7,wrptrREL));      
  
 // rest of page
 server.sendContent(" var layout = { width: 1800,  height: 800 };\n"
 " var data = [s6,s7,s1,s2,s3,s4,s5];\n"
 "Plotly.newPlot('plotly1div', data, layout);\n</script>\n"
 "</script></body>"
 "</html>");
  
  });

  server.on("/test.svg", drawGraph);

  server.on("/login", HTTP_GET, []() {
    //server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex(WERSJA));
  });

  server.on("/serverIndex", HTTP_GET, []() {
    //server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex(WERSJA,pumpStatus));
  });

/*
 *   if (server.hasArg("content") ){
    // Content has been edited. Save.
    File file = SPIFFS.open(pathname.c_str(), "w");
    String content = server.arg("content");
 */


  server.on("/restart", HTTP_GET, []() {
    //server.sendHeader("Connection", "close");
    server.send(200, "text/html", "<html><head/><body>Restart... <a href='/serverIndex'>go back</a></body></html>");
    delay(100);
    ESP.restart();
  });

  server.on("/data.js", HTTP_GET, []() {
     //server.sendHeader("Connection", "close");
     if (server.hasArg("s") ){
        String tmp =server.arg("s");
        int num=tmp.toInt();
        String wy;
        if (num==1) wy=plotlyDataJs(1,wrptrTOP);
        if (num==2) wy=plotlyDataJs(2,wrptrBOTTOM);
        if (num==3) wy=plotlyDataJs(3,wrptrROOM);
        if (num==4) wy=plotlyDataJs(4,wrptrOUT);
        if (num==5) wy=plotlyDataJs(5,wrptrWATER);
        if (num==6) wy=plotlyDataJs(6,wrptrALM);
        if (num==7) wy=plotlyDataJs(7,wrptrREL);      
        server.send(200, "text/html", wy);       
     }   
  });


  server.on("/setptr", HTTP_GET, []() {
     //server.sendHeader("Connection", "close");
     if (server.hasArg("s") ){
        String tmp=server.arg("s");
        int num=tmp.toInt();
        wrptrBOTTOM=num;
        wrptrTOP=num;
        wrptrOUT=num;
        wrptrROOM=num;
        wrptrWATER=num;
        wrptrREL=num;
        wrptrALM=num;
        server.send(200, "text/html", "<html><head/><body>Pointers set to:"+tmp+" <a href='/serverIndex'>go back</a></body></html>");      
     }   
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


  hasIoTHub = false;
  if (Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = true;
    Serial.println("IoT hub Initialized.");
  }  else  {
    Serial.println("Error during IoT hub Init...");  
  }

ghp("after mqtt init");
  
  server.begin();

ghp("after http start");
  
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


EVENT_INSTANCE *Esp32MQTTClient_Message_Generate(const char *eventString )
{
  
    EVENT_TYPE type = MESSAGE;
    if (eventString == NULL)
    {
        return NULL;
    }

    EVENT_INSTANCE *event = (EVENT_INSTANCE *)malloc(sizeof(EVENT_INSTANCE));
    event->type = type;

    if (type == MESSAGE)
    {
        event->messageHandle = IoTHubMessage_CreateFromString(eventString);
                        (void)IoTHubMessage_SetContentTypeSystemProperty(event->messageHandle, "application%2fjson");
                (void)IoTHubMessage_SetContentEncodingSystemProperty(event->messageHandle, "utf-8");
        if (event->messageHandle == NULL)
        {
            LogError("iotHubMessageHandle is NULL!");
            free(event);
            return NULL;
        }
    }
    else if (type == STATE)
    {
        event->stateString = eventString;
    }

    return event;
}



/*
 *  =========== L O O P ==============
 */
 
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

 if (curTime!=lastTime) { // each second
  lastTime=curTime;

 // MQTT nadling
  if (hasIoTHub && TT.tm_sec==0) // every minute
  {
    char buff[128]; //128 is enough for our dataset
    snprintf(buff, 128, "{\"TempTOP\":%.1f,\"TempBOTTOM\":%.1f,\"TempROOM\":%.1f,\"TempOUTSIDE\":%.1f,\"TempWATER\":%.1f,\"TempALARMS\":%i,\"TempRELAYS\":%i}" ,
    (double)statGzTemp/10,(double)statDzTemp/10,(double)statInTemp/10,(double)statOutTemp/10,(double)statCwuTemp/10,statAlarms,statRelays);
    EVENT_INSTANCE* message = Esp32MQTTClient_Message_Generate(buff);      
    if (Esp32MQTTClient_SendEventInstance(message))
    {
      Serial.println("Data sent:");
      Serial.println(buff);
    } else {
      Serial.println("MQTT err...");
    }
  }


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

  if (toinc==0){
      // save saamplepoint also for alarms & relays, but only when the samplepoint was NOT triggered by Alarms nor Relays

       RELlist[wrptrREL]=hpmCurrentREL;        
       wrptrREL=safeInc(wrptrREL,MAXTAB,RELlist);   

       ALMlist[wrptrALM]=hpmCurrentALM;       
       wrptrALM=safeInc(wrptrALM,MAXTAB,ALMlist);         

  }

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
               
        pumpStatus = inputString          //intentional - for any unparsed command to be shown
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
