# Módulo GPS NEO-6M para Hovercraft (ESP32)

Este módulo é responsável por fazer a interface com o sensor GPS **u-blox NEO-6M**, calcular a **velocidade real** e projetar a **posição relativa $(X, Y)$ em metros** a partir do ponto de partida do robô.

---

## 📌 Funcionalidades

- **Posicionamento Relativo Plano:** Converte coordenadas geográficas (Latitude e Longitude) em coordenadas cartesianas locais $(X, Y)$ em metros usando a projeção equirretangular plana.
  - $X$: Deslocamento Leste (+) / Oeste (-)
  - $Y$: Deslocamento Norte (+) / Sul (-)
- **Origem Automática:** Salva automaticamente o ponto zero $(0,0)$ assim que o GPS adquire sinal estável com no mínimo 4 satélites.
- **Velocidade Confiável:** Obtém a velocidade via efeito Doppler calculada internamente pelo hardware do u-blox NEO-6M (com filtro de repouso para eliminar ruído estático).
- **Leitura 100% Não-Bloqueante:** Não utiliza loops `while(millis())` ou `delay()`, permitindo operação simultânea com controle de motores e MQTT sem travar a CPU do ESP32.
- **Exibição em Display OLED SSD1306 (I2C):** Mostra em tempo real número de satélites, distância da origem, velocidade e coordenadas.

---

## 🔌 Pinagem e Conexão de Hardware (ESP32)

### 1. GPS NEO-6M
| Pino GPS | Pino ESP32 | Função |
| :--- | :--- | :--- |
| **VCC** | **3.3V** ou **5V** | Alimentação (o módulo possui regulador embutido) |
| **GND** | **GND** | Terra Comum |
| **TX** | **GPIO 16 (RX2)** | Envio de sentenças NMEA do GPS para a Serial 2 do ESP32 |
| **RX** | **GPIO 17 (TX2)** | Linha de transmissão do ESP32 (opcional/para comandos) |

### 2. Display OLED SSD1306 (I2C)
| Pino OLED | Pino ESP32 | Função |
| :--- | :--- | :--- |
| **VCC** | **3.3V** | Alimentação |
| **GND** | **GND** | Terra Comum |
| **SDA** | **GPIO 21** | I2C Data |
| **SCL** | **GPIO 22** | I2C Clock |

---

## 📐 Modelo Matemático

Para áreas de operação de robôs e hovercrafts (raios de alguns quilômetros), a curvatura da Terra pode ser aproximada localmente por um plano tangente (Projeção Equirretangular):

$$\Delta \text{lat} = (\text{lat} - \text{lat}_0) \times \frac{\pi}{180}$$
$$\Delta \text{lon} = (\text{lon} - \text{lon}_0) \times \frac{\pi}{180}$$
$$\text{lat}_{\text{média}} = \frac{\text{lat} + \text{lat}_0}{2} \times \frac{\pi}{180}$$

Coordenadas em metros:
$$Y = \Delta \text{lat} \times R_{\text{Terra}}$$
$$X = \Delta \text{lon} \times R_{\text{Terra}} \times \cos(\text{lat}_{\text{média}})$$
$$\text{Distância} = \sqrt{X^2 + Y^2}$$

*Onde $R_{\text{Terra}} = 6.378.137\text{ m}$ (Raio Equatorial WGS-84).*

---

## 💻 Estrutura de Arquivos

```text
📁 GpsModule/
├── 📄 GPSModule.h       # Declaração da classe GPSModule
├── 📄 GPSModule.cpp     # Implementação não-bloqueante e cálculos geodésicos
├── 📄 main_gps.ino      # Sketch de exemplo para uso com OLED e Serial
└── 📄 README.md         # Esta documentação
```

---

## 🚀 Como Executar

### Opção 1: Arduino IDE
1. Copie os arquivos `main_gps.ino`, `GPSModule.h` e `GPSModule.cpp` para uma pasta chamada **`main_gps`**.
2. Abra o arquivo `main_gps.ino` no Arduino IDE. Os arquivos `.h` e `.cpp` aparecerão como abas laterais.
3. No **Gerenciador de Bibliotecas** (`Ctrl + Shift + I`), instale:
   - `TinyGPSPlus` por *Mikal Hart*
   - `ESP8266 and ESP32 OLED driver for SSD1306 displays` por *ThingPulse*
4. Selecione a placa **ESP32 Dev Module**, conecte via USB e clique em **Upload**.
5. Abra o **Serial Monitor** em `115200 baud`.

### Opção 2: PlatformIO
O módulo já reside na pasta `firmware/lib/GpsModule/` do projeto, onde as dependências necessárias já estão configuradas no `platformio.ini`.

---

## 🛠️ Referência da API (`GPSModule`)

```cpp
GPSModule gps;

// Inicializa a Serial de comunicação
gps.begin(HardwareSerial& serial, unsigned long baud = 9600, int rxPin = 16, int txPin = 17);

// Processa novos bytes sem travar a execução. Chame sempre no loop()
bool novoDado = gps.update();

// Redefine o ponto zero (0,0) para as coordenadas atuais
gps.resetOrigin();

// Status de conexão
bool fix = gps.hasFix();            // true se tiver fix com >= 4 satélites
int sat  = gps.getSatellites();     // quantidade de satélites visíveis
double hdop = gps.getHDOP();        // precisão geométrica (menor = melhor)

// Posição Relativa em metros
double x = gps.getX();              // Leste (+) / Oeste (-) em metros
double y = gps.getY();              // Norte (+) / Sul (-) em metros
double d = gps.getDistanceFromOrigin(); // Distância radial em metros

// Cinemática
double velKmh = gps.getSpeedKmh();  // Velocidade em km/h
double velMs  = gps.getSpeedMs();   // Velocidade em m/s
double rumo   = gps.getHeading();   // Direção em graus (0° a 360°)
```

---

## ⚠️ Dicas e Solução de Problemas

1. **Primeiro Fix (Cold Start):**
   - O primeiro sincronismo do NEO-6M pode levar de **1 a 5 minutos**.
   - O módulo **não pega sinal em ambientes fechados** (dentro de casa ou salas sem janela). Sempre teste próximo a uma janela aberta ou em área externa.
2. **LED Indicador do NEO-6M:**
   - **LED apagado/fixo:** Módulo ligado, mas ainda buscando satélites.
   - **LED piscando a 1 Hz (1x por segundo):** Sincronismo fixo e dados de alta precisão disponíveis.

