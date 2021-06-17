/*
  ING. GADIEL ORTIZ GUTIÉRREZ, CE MERIDA CONKAL
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// REMPLACE EN CASO DE QUERER CONECTARSE A UNA RED WIFI
const char* ssid = "CE_CONKAL";
const char* password = "ceconkal.";

// DEFINA EL TOKEN ID DE Telegram BOT
#define BOTtoken "1773652949:AAG2jdHp6C3FJBrQXzAJTCa3p4Nj8MOA1DU"  // EL TOKEN SE OPTIENE BET Botfather

// Use @myidbot para definir el chat ID individual o por grupo
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "-568469093"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// VARIABLES DE MOVIMIENTO
const int motionSensor = 14; //D5 PIR Motion Sensor
bool motionDetected = false;

// Indicates when motion is detected
void ICACHE_RAM_ATTR detectsMovement() {
  //Serial.println("MOVIMIENTO DETECTADO!!!");
  motionDetected = true;
}

//VARIABLES DE RELAY
// Comprueba si hay nuevos mensajes cada 1 segundo.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin  = 12; //D6
const int ledPin2  = 13; //D7
bool ledState = HIGH;
bool ledState2 = HIGH;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

      if (text == "/opcion") {
      String welcome = "BIENVENIDO, " + from_name + ".\n";
      welcome += "USE ALGUNO DE LOS COMANDOS.\n\n";
      welcome += "/idu_on ENCENDER IDU \n";
      welcome += "/idu_off APAGAR IDU \n";
      welcome += "/ext_on ENCENDER RELAY EXTRA \n";
      welcome += "/ext_off APAGAR RELAY EXTRA \n";      
      welcome += "/est_1 ESTADO DEL IDU \n";
      welcome += "/est_2 ESTADO DEL RELAY EXTRA \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    //RELAY_1
    if (text == "/idu_off") {
      bot.sendMessage(chat_id, "El IDU: APAGADO", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/idu_on") {
      bot.sendMessage(chat_id, "El IDU: ENCENDIDO", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/est_1") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "EL IDU: ENCENDIDO", "");
      }
      else{
        bot.sendMessage(chat_id, "El IDU: APAGADO", "");
      }
    }
    //RELAY-2
    if (text == "/ext_off") {
      bot.sendMessage(chat_id, "EL Relay: APAGADO", "");
      ledState2 = LOW;
      digitalWrite(ledPin2, ledState2);
    }
    
    if (text == "/ext_on") {
      bot.sendMessage(chat_id, "El RELAY: ENCENDIDO", "");
      ledState2 = HIGH;
      digitalWrite(ledPin2, ledState2);
    }
    
    if (text == "/est_2") {
      if (digitalRead(ledPin2)){
        bot.sendMessage(chat_id, "EL RELAY: ENCENDIDO", "");
      }
      
      else{
        bot.sendMessage(chat_id, "El RELAY: APAGADO", "");
      }
    }
    
  }
}
void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  //SENSOR DE MOVIMIENTO
  // PIR Motion Sensor mode INPUT_PULLUP
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  //RELAY
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin, ledState);
  digitalWrite(ledPin2, ledState2);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot SIPSE Iniciado", "");
}

void loop() {
  //CODIGO SENSOR DE MOVIMIENTO
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "Movimiento detectado dentro del edificio de SIPSE!!", "");
    Serial.println("Movimiento detectado");
    motionDetected = false;
  }
  //CODIGO RELAY
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }  
}
