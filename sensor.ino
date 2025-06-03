#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27, 16, 2);

#define DHT_pino 19

#define Verde 4
#define Vermelho 17

DHTesp sensor;

void setup() {
  lcd.backlight();
  lcd.init();
  Serial.begin(9600);
  sensor.setup(DHT_pino, DHTesp::DHT22);
  pinMode(Verde, OUTPUT);
  pinMode(Vermelho, OUTPUT);
}

void loop() {
  float temperatura, umidade;
  temperatura = sensor.getTemperature();
  umidade = sensor.getHumidity();

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

  if(temperatura >= 37){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("TEMPERAT ELEVEDA");
    Alerta();
    delay(1000);
  }

  if(umidade <= 30){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("     ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("UMIDADE BAIXA");
    Alerta();
    delay(1000);
  }
}

void ok(){
  digitalWrite(Verde, HIGH);
  digitalWrite(Vermelho, LOW);
}

void Alerta(){
  digitalWrite(Vermelho, HIGH);
}