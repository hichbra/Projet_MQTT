#include <PubSubClient.h>

// utilise les ports 4 7 10 11 et 12
#include <WiFi.h>
#include <SPI.h>
#include <string.h>

//--- Wifi Shield
int status = WL_IDLE_STATUS; // Statut du shield
char ssid[] = "IPOD DE MR.FIK" ; // "FREEBOX_MOHAMMED_X3";
char pass[] = "123456789" ; // "F4CAE55AF478";
WiFiClient clientWifi ;
boolean connexionReussi = true ;

unsigned long lastConnectionTime = 0;        // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 3000L; //10L * 1000L; // delay between updates, in milliseconds

// MQTT
char mqtt_server[] = "88.170.244.38";
char mqtt_user[] = "guest";
char mqtt_pass[] = "guest";

char topic[] = "sensor/temperature";

char message_buff[100];
long lastMsg = 0;   // Horodatage du dernier message publiÃ© sur MQTT
long lastRecu = 0;

PubSubClient client(clientWifi);

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);

  setup_wifi(); // Connexion au rÃ©seau Wifi
 
  // Initialisation MQTT
  client.setServer(mqtt_server, 1883);    // Configuration de la connexion au serveur MQTT
  client.setCallback(callback);  // La fonction de callback qui est executÃ©e Ã  chaque rÃ©ception de message
}

void setup_wifi()
{
   // Test Wifi Shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield non connecte");
    while (true); // Ne pas continuer
  }
  else {
    Serial.println("WiFi shield connecte");
  } 
  
  // Initialisation Wifi
  int i = 0 ;
  status = WiFi.begin(ssid, pass);

  while (status != WL_CONNECTED) {
    Serial.print("Tentative de connexion au reseau: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    i++ ;
   
    if ( i == 2 ){ connexionReussi = false ; break ; }
    else { Serial.print("Essai "); Serial.println(i); delay(5000); }
  }
  if ( i != 2 )
    Serial.println("Connecte au wifi");
  
  printWifiStatus();
  Serial.print("Firmware version: ");
  Serial.println(WiFi.firmwareVersion());
}

void loop() {
  
 if (!client.connected()) {
  reconnect();
 }
  
  client.loop();
 
  long now = millis();

  
  //Serial.print("Temps : ");Serial.println((now-lastMsg)/1000);

  // Envoi d'un message par minute
  if (now - lastMsg > 10000)//60000)
  {
    lastMsg = now;

    // Lecture de la température en Celcius
    //float t = 22.5 ;
    float t = analogRead(A0);

    Serial.print("Humidity : "); Serial.println(t);
    //Serial.print("Temperature : "); Serial.println(t);
    
    client.publish(topic, String(t).c_str(), true);   // Publie la tempÃ©rature sur le topic temperature_topic
  }
  if (now - lastRecu > 100 ) {
    lastRecu = now;

    // Serial.println("Subscribe");
    client.subscribe("homeassistant/switch1");
  }
  
  /*if (millis() - lastConnectionTime > postingInterval && connexionReussi) {
     Serial.print("connected ?? "); Serial.println(client.connected());
     // REQUEST
     //sendRequest(etatServo1, etatServo2, etatServo3, etatServo4);
  }*/
 
}

void reconnect()
{
  while(!client.connected()) {
    Serial.print("Connexion au serveur MQTT...");
    if (client.connect("Capteur_1", mqtt_user, mqtt_pass)) {
      Serial.println("OK");
    }
    else {
      Serial.print("KO, erreur :"); Serial.print(client.state());Serial.println(". 5s avant nouvel essai");
      delay(5000);
    }
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

const char* boolstring( boolean b ) {
  if (b) {
   return "true" ;
  } else { 
   return "false" ;
  }
}

// DÃ©clenche les actions Ã  la rÃ©ception d'un message
// D'aprÃ¨s http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {
 
  int i = 0;
  Serial.println("Message recu =>  topic: " + String(topic));
  Serial.print(" | longueur: " + String(length,DEC));

  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);

  Serial.println("Payload: " + msgString);
  
  
  /*if ( msgString == "ON" ) {
    digitalWrite(D2,HIGH);  
  } else {
    digitalWrite(D2,LOW);  
  }*/
}
