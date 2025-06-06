#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27, 16, 2);

#define DHT_pino 19
#define Verde 4
#define Vermelho 17

DHTesp sensor;

// WiFi/MQTT configs
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient WOKWI_CLIENT;
PubSubClient client(WOKWI_CLIENT);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Converte o payload para string
  String mensagem = "";
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  // Exemplo: se o tópico for temperatura ou umidade, exibe o valor recebido
  if (String(topic) == "sensor/temperatura") {
    Serial.print("Temperatura recebida: ");
    Serial.print(mensagem);
    Serial.println(" °C - Mensagem entregue com sucesso.");
  } else if (String(topic) == "sensor/umidade") {
    Serial.print("Umidade recebida: ");
    Serial.print(mensagem);
    Serial.println(" % - Mensagem entregue com sucesso.");
  } else {
    Serial.print("Mensagem recebida no tópico [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.print(mensagem);
    Serial.println(" - Mensagem entregue com sucesso.");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // String clientId = "ESP8266Client-";
    // clientId += String(random(0xffff), HEX);
    if (client.connect("WOKWI_CLIENT")) {
      Serial.println("connected");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  lcd.backlight();
  lcd.init();
  Serial.begin(115200);
  sensor.setup(DHT_pino, DHTesp::DHT22);
  pinMode(Verde, OUTPUT);
  pinMode(Vermelho, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperatura = sensor.getTemperature();
  float umidade = sensor.getHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     ");
  lcd.print(temperatura);
  lcd.print(" c");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(umidade);
  lcd.print(" %UR");
  ok();
  delay(2000);

  // Publica temperatura e umidade no MQTT
  snprintf(msg, MSG_BUFFER_SIZE, "%.2f", temperatura);
  client.publish("sensor/temperatura", msg);
  snprintf(msg, MSG_BUFFER_SIZE, "%.2f", umidade);
  client.publish("sensor/umidade", msg);

  if(temperatura >= 37){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("TEMPERAT ELEVEDA");
    Alerta();
    delay(1000);
    client.publish("sensor/alerta", "Temperatura elevada");
  }

  if(umidade <= 30){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("UMIDADE BAIXA");
    Alerta();
    delay(1000);
    client.publish("sensor/alerta", "Umidade baixa");
  }
}

void ok(){
  digitalWrite(Verde, HIGH);
  digitalWrite(Vermelho, LOW);
}

void Alerta(){
  digitalWrite(Vermelho, HIGH);
}
