#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHT_pino 19
#define Verde 4
#define Vermelho 17

DHTesp sensor;

// WiFi/MQTT configs
const char* ssid = "Wokwi-GUEST"; // SUBSTITUA PELO SEU SSID
const char* password = ""; // SUBSTITUA PELA SUA SENHA DO WIFI
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient WOKWI_CLIENT;
PubSubClient client(WOKWI_CLIENT);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

float lastTemperature = 0.0;
float lastHumidity = 0.0;
const float THRESHOLD = 0.5; // Limite para considerar uma alteração significativa (ex: 0.5 unidades)

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

  Serial.print("Mensagem recebida no tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  float currentTemperature = sensor.getTemperature();
  float currentHumidity = sensor.getHumidity();

  // Ao receber uma mensagem, responde com a temperatura ou umidade atual
  if (String(topic) == "smartirrigation/temperaturaAmbiente") {
    snprintf(msg, MSG_BUFFER_SIZE, "Temperatura atual: %.2f C", currentTemperature);
    client.publish("smartirrigation/respostaTemperatura", msg);
    Serial.print("Respondendo a temperatura: ");
    Serial.println(msg);
  } else if (String(topic) == "smartirrigation/umidadeAmbiente") {
    snprintf(msg, MSG_BUFFER_SIZE, "Umidade atual: %.2f %%", currentHumidity);
    client.publish("smartirrigation/respostaUmidade", msg);
    Serial.print("Respondendo a umidade: ");
    Serial.println(msg);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("WOKWI_CLIENT")) { // Pode usar um clientId mais dinâmico se preferir
      Serial.println("connected");
      // Se inscreve nos tópicos de temperatura e umidade
      client.subscribe("smartirrigation/temperaturaAmbiente");
      Serial.println("Subscribed to smartirrigation/temperaturaAmbiente");
      client.subscribe("smartirrigation/umidadeAmbiente");
      Serial.println("Subscribed to smartirrigation/umidadeAmbiente");
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

  // Inicializa as últimas leituras para evitar publicação na primeira leitura
  lastTemperature = sensor.getTemperature();
  lastHumidity = sensor.getHumidity();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float currentTemperature = sensor.getTemperature();
  float currentHumidity = sensor.getHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    ");
  lcd.print(currentTemperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(currentHumidity);
  lcd.print(" %UR");
  ok();
  delay(2000);

  // Verifica se a temperatura ou umidade mudou significativamente
  if (abs(currentTemperature - lastTemperature) >= THRESHOLD) {
    snprintf(msg, MSG_BUFFER_SIZE, "%.2f", currentTemperature);
    client.publish("smartirrigation/temperaturaAmbiente", msg);
    Serial.print("Publicando nova temperatura: ");
    Serial.println(msg);
    lastTemperature = currentTemperature;
  }

  if (abs(currentHumidity - lastHumidity) >= THRESHOLD) {
    snprintf(msg, MSG_BUFFER_SIZE, "%.2f", currentHumidity);
    client.publish("smartirrigation/umidadeAmbiente", msg);
    Serial.print("Publicando nova umidade: ");
    Serial.println(msg);
    lastHumidity = currentHumidity;
  }

  if (currentTemperature >= 37) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("TEMPERAT ELEVADA");
    Alerta();
    delay(1000);
    client.publish("smartirrigation/alerta", "Temperatura elevada");
  }

  if (currentHumidity <= 30) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("UMIDADE BAIXA");
    Alerta();
    delay(1000);
    client.publish("smartirrigation/alerta", "Umidade baixa");
  }
}

void ok() {
  digitalWrite(Verde, HIGH);
  digitalWrite(Vermelho, LOW);
}

void Alerta() {
  digitalWrite(Vermelho, HIGH);
  digitalWrite(Verde, LOW); // Garante que o LED verde esteja desligado durante o alerta
}
