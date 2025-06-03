# objetos-conectados

# Monitor de Temperatura e Umidade com ESP32

Este projeto utiliza um ESP32, um sensor DHT22, um display LCD 16x2 (I2C) e dois LEDs para monitorar temperatura e umidade do ambiente.

## Componentes Utilizados

- ESP32 DevKit v4
- Sensor DHT22
- Display LCD 16x2 com interface I2C
- LED vermelho (indicador de alerta)
- LED verde (indicador de funcionamento normal)
- Resistores de 330Ω

## Funcionamento

- O ESP32 lê a temperatura e umidade do sensor DHT22.
- Os valores são exibidos no display LCD.
- Se a temperatura for maior ou igual a 37°C, o LED vermelho acende e uma mensagem de alerta é exibida no LCD.
- Se a umidade for menor ou igual a 30%, o LED vermelho acende e uma mensagem de alerta é exibida no LCD.
- Caso contrário, o LED verde permanece aceso indicando funcionamento normal.

## Ligações

- DHT22:
  - VCC → 5V do ESP32
  - GND → GND do ESP32
  - SDA → Pino 19 do ESP32
- LCD I2C:
  - VCC → 5V do ESP32
  - GND → GND do ESP32
  - SDA → Pino 21 do ESP32
  - SCL → Pino 22 do ESP32
- LED Verde: Anodo no pino 4 do ESP32 (com resistor)
- LED Vermelho: Anodo no pino 17 do ESP32 (com resistor)
- Cátodos dos LEDs → GND

## Bibliotecas Necessárias

Veja [libraries.txt](libraries.txt):

- DHT sensor library for ESPx
- LiquidCrystal I2C

## Código

O código principal está em [sensor.ino](sensor.ino).

## Simulação

O projeto pode ser simulado no [Wokwi](https://wokwi.com/).