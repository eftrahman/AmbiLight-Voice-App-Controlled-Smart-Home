#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "BlynkAuth";
char ssid[] = "YourSSID";
char pass[] = "YoutPassword";

float temperature=0;

const int dhtPin = D0;
const int lightOnePin = D1;
const int fanOnePin = D3;
const int lightTwoPin = D2;
const int fanTwoPin = D4;
const int dimmer1Pin = D5;
const int dimmer2Pin = D6;

int lightOneState = LOW;
int fanOneState = LOW;
int dimmerValue1 = 0;
int dimmerValue2 = 0;
int lightTwoState = LOW;
int fanTwoState = LOW;

int fanSpeeds[] = {128, 179, 154};  // array of fan speeds in percentage
int currentSpeed = 0;  // current fan speed index
unsigned long previousMillis = 0;  // last time the fan speed was changed
const long interval = 5000;  // interval to change fan speed (5 seconds)

int duration1=0;
int duration2=0;

BLYNK_WRITE(V2) {
  lightOneState = param.asInt();
  digitalWrite(lightOnePin, !lightOneState);
  if (lightOneState == HIGH) {
    Serial.print("\n++++++Light One ON++++++") ;
  }
  else {
    Serial.print("\n------Light One OFF------") ;
  }
}

BLYNK_WRITE(V3) {
  lightTwoState = param.asInt();
  digitalWrite(lightTwoPin, !lightTwoState);
  if (lightTwoState == HIGH) {
    Serial.print("\n++++++Light Two ON++++++") ;
  }
  else {
    Serial.print("\n------Light Two OFF------") ;
  }
}

BLYNK_WRITE(V4) {
  fanOneState = param.asInt();
  digitalWrite(fanOnePin, !fanOneState);
  if (fanOneState == HIGH) {
    Serial.print("\n++++++Fan One ON++++++") ;
  }
  else {
    Serial.print("\n------Fan One OFF------") ;
  }
}

BLYNK_WRITE(V5) {
  fanTwoState = param.asInt();
  digitalWrite(fanTwoPin, !fanTwoState);
  if (fanTwoState == HIGH) {
    Serial.print("\n++++++Fan Two ON++++++") ;
  }
  else {
    Serial.print("\n------Fan Two OFF------") ;
  }
}

BLYNK_WRITE(V6) {
  dimmerValue1 = param.asInt();
  analogWrite(dimmer1Pin, dimmerValue1);
  Serial.print("\nLight One Value : ") ;
  Serial.print(dimmerValue1) ;
}

BLYNK_WRITE(V7) {
  dimmerValue2 = param.asInt();
  analogWrite(dimmer2Pin, dimmerValue2);
  Serial.print("\nFan One Value : ") ;
  Serial.print(dimmerValue2) ;
}

BLYNK_WRITE(V10) {
  int fanSpeed = 0;
  if (temperature > 40) {
    fanSpeed = 255;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  else if(temperature >=35 && temperature <40){
    fanSpeed = 230;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  else if(temperature >=30 && temperature <35){
    fanSpeed = 200;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  else if(temperature >=25 && temperature <30){
    fanSpeed = 180;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  else if(temperature >=20 && temperature <25){
    fanSpeed = 150;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  else {
    fanSpeed = 0;
    Serial.print("\nfanspeed using temperature : ") ;
    Serial.print(fanSpeed) ;
  }
  analogWrite(fanOnePin, fanSpeed);
}

BLYNK_WRITE(V11) {
  int buttonState = param.asInt();
  if (buttonState == HIGH) {
    // button is pressed
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // it's time to change the fan speed
      analogWrite(fanOnePin, fanSpeeds[currentSpeed]);
      Serial.print("\nFan One Speed : ");
      Serial.print(fanSpeeds[currentSpeed]);
      currentSpeed++;
      if (currentSpeed >= sizeof(fanSpeeds) / sizeof(fanSpeeds[0])) {
        // reached the end of the fan speeds array, start from the beginning
        currentSpeed = 0;
      }
      previousMillis = currentMillis;
    }
  }
}

BLYNK_WRITE(V8) {
  int buttonState = param.asInt();
  if (buttonState == HIGH) {
    Serial.print("\n--timer on--") ;
    // button is pressed, start the timer
    //int duration = param.asInt(); // get the duration from the slider
    
    unsigned long endTime = millis() + duration1 * 60000; // convert minutes to milliseconds
    while (millis() < endTime) {
      // wait for the timer to finish
      delay(500);
    }
    // timer finished, turn off light one
    digitalWrite(lightOnePin, LOW);
    Serial.print("\n------Light One OFF by timer------") ;
  }
}
BLYNK_WRITE(V12) {
  duration1 = (param.asInt() * 60 * 1000); // convert minutes to milliseconds
}
BLYNK_WRITE(V9) {
  if (param.asInt() == 1) {
    duration2 = (param.asInt() * 60 * 1000); // convert minutes to milliseconds
    previousMillis = millis(); // store the current time
  } else {
    duration2 = 0;
    digitalWrite(lightTwoPin, LOW); // turn off light one
  }
}

BLYNK_WRITE(V13) {
  duration2 = param.asInt();// * 60 * 1000); // convert minutes to milliseconds
}

DHT dht(dhtPin, DHT11);
void setup() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
}

void loop() {
  Blynk.run();
  delay(150);
  temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  delay(150);

  if (duration1 != 0) {
    unsigned long currentMillis = millis(); // get the current time
    if (currentMillis - previousMillis >= duration1) { // check if the desired duration has elapsed
      previousMillis = currentMillis;
      digitalWrite(lightOnePin, LOW); // turn off light one
      duration1 = 0;
    }
  }
}
