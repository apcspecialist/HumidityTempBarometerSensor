//HumidityTempBarometerSensor.ino
/* 
Relay controlled Light switch, Temperature and Humidity Sensor for MySensors.org
Connect button or door/window reed switch between 
digital I/O pin 5 (BUTTON_PIN below) and GND.
By David Schneider - A PC Specialist on August 16, 2015
*/

#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>
#include <Bounce2.h>

#define NODE_ID 10
#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
// #define CHILD_ID_hIndex 2
// #define CHILD_ID_available 3
// #define CHILD_ID_SWITCH 4 //was chil_id 3
// #define CHILD_ID_SOIL 6   // Id of the moisture sensor child

#define BUTTON_PIN  5  // Arduino Digital I/O pin for button/reed switch
#define HUMIDITY_SENSOR_DIGITAL_PIN 3
// #define DIGITAL_INPUT_SOIL_SENSOR 6   // Digital input did you attach your soil sensor.  
// #define INTERRUPT DIGITAL_INPUT_SOIL_SENSOR-2 // Usually the interrupt = pin -2 (on uno/nano anyway)

#define SN "Temp, Humidity and Soil Moisture"
#define SV "2.0E"

#define RELAY_PIN  7  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define RELAY_CHILD 5
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

MySensor gw;
// Bounce debouncer = Bounce(); 
DHT dht;
// Add DHT missing setting here


float lastTemp;
float lastHum;
int pause=30;  // delay of 1000ms * 30, delay of 1 second * 30 = 30 seconds
//int lastSoil = -1;
//bool buggin = false;

//this part of the routine has been modified for Imperial
boolean metric = false; // hard coded for Imperial. 

MyMessage msgHum(CHILD_ID_HUM, V_HUM);  //Define MyMessage msgHum for tx
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP); //Define MyMessage msgTEMP for tx
MyMessage msg(RELAY_CHILD,V_LIGHT); // Defign MyMessage msg for tx

// Change to V_LIGHT if you use S_LIGHT in presentation below
// MyMessage msg(CHILD_ID_SWITCH,V_LIGHT);
// MyMessage msg(CHILD_ID_SOIL, V_TRIPPED);


void setup()  
{ 
//Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, NODE_ID, false);// note: Set nodeid para#2 ID#10, param #1 was NULL
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 

// Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo(SN, SV);

// sets the soil sensor digital pin as input
// pinMode(DIGITAL_INPUT_SOIL_SENSOR, INPUT);

// Sets the light switch to digital pin as output
  pinMode(BUTTON_PIN,OUTPUT);
  
// Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
// gw.present(CHILD_ID_SOIL, S_MOTION);
  gw.present(RELAY_CHILD, S_LIGHT);
  
  metric = gw.getConfig().isMetric; //try rem this
  
// sets the soil sensor digital pin as input
// pinMode(DIGITAL_INPUT_SOIL_SENSOR, INPUT); 
  
// Make sure relays are off when starting up
   digitalWrite(RELAY_PIN, RELAY_OFF);
  
// Then set relay pins in output mode
   pinMode(RELAY_PIN, OUTPUT);    
  
/* You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
gw.present(CHILD_ID_SWITCH, S_LIGHT)  */
}

void loop()      
  {
  delay(dht.getMinimumSamplingPeriod()); // 1 sec - DHT11, 2 sec - DHT22
  float temperature = dht.getTemperature();
  temperature = dht.toFahrenheit(temperature);
  if (isnan(temperature)) 
  {
      Serial.println("Failed reading temperature from DHT");
  }
  else if (temperature != lastTemp) 
  {
    lastTemp = temperature;
    gw.send(msgTemp.set(temperature, 1));
    Serial.print("Temperture: ");
    Serial.println(temperature);
  }
    
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
      Serial.print("Humidity: ");
      Serial.println(humidity);
  }
  
  // Alway process incoming messages whenever possible
  gw.process(); 
  
  {     
  // Read digital soil value
// int soil = digitalRead(DIGITAL_INPUT_SOIL_SENSOR); // 1=(Dry-Not triggered), 0=In soil with water (Triggered) 
//  if (soil != lastSoil) 
  { 
    // FIX ME Below
    // if the soilValue is not equal to lastSoilValue print soilValue to searial
    //Serial.print("Soil Value: ");
    //Serial.println(soil); // print soilValue to serial display for debugging
    //gw.send(msg.set(soil==0?1:0));  // Send the inverse to gw as tripped should be when no water in soil
  // Returns 0 = Dry (Triggered), 1 = In soil with water (Not triggered)   
    //lastSoil = soil;  // Replace lastSoilValue with the new soilValue
  }
  
  // Serial.print("Delay: ");
  // Serial.println(dht.getMinimumSamplingPeriod()*pause);  //delay of 1000*20

  //delay(dht.getMinimumSamplingPeriod()*pause);  //delay of 1 second * 20 = 20 seconds
 }
}

void incomingMessage(const MyMessage &message) 
{
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) 
  {
 
     // Change relay state
     digitalWrite(RELAY_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
 
     // Store state in eeprom
     gw.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}


