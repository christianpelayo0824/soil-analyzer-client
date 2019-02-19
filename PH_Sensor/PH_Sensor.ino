#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset -2.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
#include <LiquidCrystal_I2C.h>

ESP8266WebServer server(80);

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex = 0;

int stationA = A0;
char* ssid     = "Jackarovs";
char* password = "uityhg423567";
char* host = "192.168.254.102";

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("Welcome");
  lcd.setCursor(2, 1);
  lcd.print("Soil Analyzer");
  delay(2000);
  lcd.clear();
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(2000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  lcd.setCursor(0, 0);
  lcd.print("Connecting");
  delay(2000);
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  lcd.setCursor(0, 0);
  lcd.print("Connected to ");
  lcd.setCursor(0, 1);
  lcd.println(ssid);
  delay(5000);
  lcd.clear();
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  lcd.print("IP address:  ");
  lcd.setCursor(0, 1);
  lcd.println(WiFi.localIP());
  delay(5000);
  lcd.clear();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  lcd.setCursor(0, 0);
  lcd.print("Wait For 2mins");
  lcd.setCursor(0, 1);
  lcd.print("for analysis");
  delay(3000);
  lcd.clear();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  configRoute();
  server.begin();
}

void configRoute() {
  server.on("/", []() {
    server.send(200, "text/plain", "Hello World!");
  });
}



double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}

void getPHValue() {

  Serial.println("Connecting to host");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection Failed");
    return;
  }

  String ADCData, station, getData, Link;
  int adcvalue = analogRead(A0); //read analog value from LM35

  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  if (millis() - samplingTime > samplingInterval)
  {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth)pHArrayIndex = 0;
    voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
    pHValue = 3.5 * voltage + Offset;
    samplingTime = millis();

  }


  if (millis() - printTime > printInterval)  //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    lcd.setCursor(0, 0);
    lcd.print("Now Analyzing");


    lcd.setCursor(0, 1);
    lcd.print("................");
    delay(3000);

    lcd.setCursor(0, 1);
    lcd.print("Voltage: ");
    lcd.println(voltage, 2);

    lcd.setCursor(0, 0);
    lcd.print("pH Value: ");
    lcd.println(pHValue, 2);
    delay(3000); //3000 3s

    lcd.setCursor(0, 0);
    lcd.print("One Minute Left");

    lcd.setCursor(0, 1);
    lcd.print("................");
    delay(3000);

    lcd.setCursor(0, 1);
    lcd.print("Voltage: ");
    lcd.println(voltage, 2);

    lcd.setCursor(0, 0);
    lcd.print("pH Value: ");
    lcd.println(pHValue, 2);
    delay(3000); //3000 50 sec

    lcd.setCursor(0, 0);
    lcd.print("Finalizing");

    lcd.setCursor(5, 1);
    lcd.print("result");
    delay(3000);

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Voltage: ");
    lcd.println(voltage, 2);
    lcd.setCursor(0, 0);
    lcd.print("pH Value: ");
    lcd.println(pHValue, 2);
    delay(3000);

    digitalWrite(LED, digitalRead(LED) ^ 1);
    printTime = millis();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonObj = jsonBuffer.createObject();
    char JSONmessageBuffer[200];

    jsonObj["phpSensor"] = pHValue;
    jsonObj["voltage"] = voltage;

    jsonObj.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);


    HTTPClient http;
    http.begin("http://192.168.254.102:8080/api/sensor/savePHPSensorData");
    //    http.addHeader("Access-Control-Allow-Origin: *");
    http.addHeader("Content-Type", "application/json");
    //    http.addHeader("Access-Control-Allow-Methods: POST");

    int httpCode = http.POST(JSONmessageBuffer);

    if (httpCode > 0 ) {
      String payload = http.getString();
      Serial.println("Status");
      Serial.println(httpCode);
      Serial.println("Payload: ");
      Serial.println(payload);
    } else {
      Serial.println("Error POST request");
      Serial.println(httpCode);
    }

    http.end();
    delay(3000);
  }
}

void loop()
{
  getPHValue();
  server.handleClient();
}







