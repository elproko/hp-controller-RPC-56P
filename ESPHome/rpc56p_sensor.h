#include "esphome.h"

// esp12
#define LED_PWR 16

#define DIR_PIN 4

#define max_line_length 80

class RPC56pSensor : public PollingComponent, public UARTDevice {

 private:
 	 float in_temp = 1.0;
	 float out_temp = 1.0;
	 float top_temp = 1.0;
	 float bottom_temp = 1.0;
	 float water_temp = 1.0;
	 int relays = 1;
	 int alarms = 1;
     int read_ok = 0;
 public:

  Sensor *in_temp_sensor = new Sensor();
  Sensor *out_temp_sensor = new Sensor();
  Sensor *top_temp_sensor = new Sensor();
  Sensor *bottom_temp_sensor = new Sensor();
  Sensor *water_temp_sensor = new Sensor();
  
  Sensor *alarms_sensor = new Sensor();
  Sensor *relays_sensor = new Sensor();
  
  // constructor
  RPC56pSensor(UARTComponent *parent) : PollingComponent(30000),  UARTDevice(parent) {}

  float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; }

  void setup() override {
    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(DIR_PIN, LOW);// read direction for 74sn176
  }

  int readline(int readch, char *buffer, int len)  {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
        case ':': // reset pointer
          pos = 0;
          buffer[pos++] = readch;
          buffer[pos] = 0;
          break;
        case '\n': // Ignore new-lines
          break;
        case '\r': // Return on CR
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  
unsigned long Str2Hex(String we){
  unsigned long ret=0;
  int slen=we.length();
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
  
 void loop() override {

 static char buffer[max_line_length];

  int reads_done = 0;
  
    while (available() && reads_done<20) {
     int n = readline(read(), buffer, max_line_length);
     if( n > 5) {
        String inputString = String(buffer);
        int C = inputString.substring(3,5).toInt();  // uwaga na parametry: od - do
        int wart = inputString.substring(5).toInt(); 
        switch (C) {          
            case 1:    // relays:    0b=spr+cwu+dz, 0a=cwu+dz, 08=cwu
                relays = Str2Hex(inputString.substring(5));
                reads_done++;
                break;
            case 10:  // cmdgettemppok  ":0110"    - czujnik pokojowy                
                in_temp = (float)wart/10; //(double)wart/10;
                reads_done++;
                break;
            case 11:  // cmdgettemppog  ":0111"    - pogodowka
                out_temp = (float)wart/10; //(double)wart/10;
                reads_done++;
                break;
            case 12: //cmdgettempdz   ":0112"    - Dolne ¿ród³o
                bottom_temp = (float)wart/10;//(double)wart/10;
                reads_done++;
                break;
            case 13: //cmdgettempcwu  ":0113"    - CWU
                water_temp = (float)wart/10; //(double)wart/10;
                reads_done++;
                break;
            case 14:  //  cmdgettempco   ":0114"    - CO
                top_temp = (float)wart/10;//(double)wart/10;
                reads_done++;
                break;
            case 22: //cmdreadinput   ":0122"    - wejœcia zezwoleñ i alarmów
                alarms = 7-Str2Hex(inputString.substring(5));
                reads_done++;
                read_ok = 1;
                break;               
        }
	 }
     reads_done++;
    }
 }
 

 void update() override {
    // This will be called every "update_interval" milliseconds.

  if (read_ok==1) {
	 in_temp_sensor->publish_state(in_temp);
	 out_temp_sensor->publish_state(out_temp);
	 top_temp_sensor->publish_state(top_temp);
	 bottom_temp_sensor->publish_state(bottom_temp);
	 water_temp_sensor->publish_state(water_temp);
	 alarms_sensor->publish_state(alarms);
	 relays_sensor->publish_state(relays);
  }	 
 }
};