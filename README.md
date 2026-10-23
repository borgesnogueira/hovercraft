# Hovercraft Autônomo para Monitoramento Ambiental em Manguezais

Projeto de desenvolvimento de um veículo anfíbio (hovercraft) autônomo subatuado para navegação, mapeamento e inspeção ambiental em ecossistemas costeiros e estuarinos de difícil acesso.

---

## 📌 Sobre o Projeto

Os manguezais representam ecossistemas tropicais de altíssima produtividade e relevância ambiental, cobrindo cerca de 10.000 km² do litoral brasileiro. Entretanto, a presença de lama densa, vegetação fechada e superfícies alagadas com pouca profundidade inviabiliza o uso de embarcações convencionais ou veículos terrestres com rodas.

Este projeto desenvolve um **hovercraft autônomo** composto por três hélices operadas de forma independente:
* **Sustentação:** 1 hélice inferior maior responsável por gerar o colchão de ar sob a base do veículo, garantindo a sustentação e flutuação sobre terra, água ou lama sem contato direto com o solo.
* **Movimentação e Direção:** 2 hélices traseiras acionadas de forma diferencial para controle de velocidade vetorial e esterço (yaw).

O robô embarca microcontroladores da linha ESP32 e sensores de localização para navegação e envio de telemetria em tempo real.

---

## 🛠️ Arquitetura de Hardware e Software

### Hardware & Microcontroladores
* **Processamento Embarcado:** ESP32 e ESP32 com display OLED integrado.
* **Módulo de Localização:** GPS u-blox **NEO-6M** (comunicação Serial UART).
* **Propulsão e Sustentação:** 3 motores brushless acionados via ESCs (*Electronic Speed Controllers*):
  * 1 ESC para o motor de sustentação/elevação inferior.
  * 2 ESCs para acionamento diferencial dos motores de propulsão traseiros.
* **Comunicação Sem Fio:** Protocolo MQTT via Wi-Fi/LoRa para envio de comandos de atuação e recepção de telemetria.

### Estrutura do Software
* **Controle de Motores e Sustentação:** Firmware em C++ (.ino) rodando no ESP32 para recepção de tópicos MQTT e geração dos sinais PWM/Servo para os 3 ESCs.
* **Módulo GPS:** Classe orientada a objetos (`GPSModule.h` / `GPSModule.cpp`) para encapsulamento e parsing dos dados de navegação.
* **Interface de Controle Externa:** Script Python (`controle_mqtt.py`) para envio de comandos de movimentação/flutuação e gerenciamento da comunicação MQTT.

---

## 📂 Estrutura do Repositório

```text
hovercraft/
├── .vscode/                       # Configurações de ambiente do VS Code
├── controle-helices/              # Firmware dos motores (sustentação e acionamento diferencial)
│   ├── helices_mqtt.ino           # Controle dos ESCs via MQTT
│   ├── teste_motor.ino            # Script de validação e teste de bancada dos ESCs
│   └── README.md
├── controle-python-mqtt/          # Scripts client-side para envio de comandos
│   ├── controle_mqtt.py           # Interface Python para controle do hovercraft
│   └── infromacoes_controle.txt   # Notas e configurações de tópicos/mensagens
├── main_gps/                      # Módulo de telemetria e navegação GPS
│   ├── GPSModule.cpp              # Implementação da classe de leitura do NEO-6M
│   ├── GPSModule.h                # Cabeçalho da classe GPSModule
│   ├── main_gps.ino               # Sketch principal de captura e envio de coordenadas
│   └── README.md
├── .gitignore
└── README.md
```

---

## ⚙️ Configuração do PlatformIO (`platformio.ini`)

Para compilar e carregar o firmware utilizando o **VS Code + PlatformIO**, você pode criar um arquivo `platformio.ini` na raiz do projeto ou nas pastas dos firmwares. 

Abaixo está o modelo de configuração sugerido com suporte aos dois ambientes utilizados no projeto:

```ini
[platformio]
default_envs = esp32dev, esp32display

[env]
platform = espressif32
framework = arduino
monitor_speed = 115200
lib_deps =
    knolleary/PubSubClient@^2.8
    mikalhart/TinyGPSPlus@^1.0.3

; Ambiente para o ESP32 responsável pelo controle dos motores
[env:esp32dev]
board = esp32dev
build_flags = 
    -D MOTOR_CONTROL_NODE

; Ambiente para o ESP32-display responsável pelo GPS e telemetria
[env:esp32display]
board = heltec_wifi_kit_32_v2  ; ou ttgo-lora32-v1 / esp32-OLED dependendo do modelo do seu display
lib_deps = 
    ${env.lib_deps}
    thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays@^4.3.0
build_flags = 
    -D TELEMETRY_DISPLAY_NODE
```

### Comandos Úteis do PlatformIO CLI

* **Compilar todos os ambientes:**
  ```bash
  pio run
  ```
* **Upload para a placa de controle de motores (ESP32):**
  ```bash
  pio run -e esp32dev -t upload
  ```
* **Upload para a placa de telemetria/GPS (ESP32 Display):**
  ```bash
  pio run -e esp32display -t upload
  ```
* **Abrir o Monitor Serial:**
  ```bash
  pio device monitor
  ```

---

## 🚀 Como Executar o Projeto

### Ambiente de Desenvolvimento
O código do firmware pode ser compilado e gravado no ESP32 utilizando:
* **VS Code** com a extensão **PlatformIO** *(Recomendado para gerenciamento de dependências e ambientes)*.
* **VS Code** com a extensão **Arduino** ou a **Arduino IDE** configurada para placas ESP32.

### Pré-requisitos e Bibliotecas
1. **Python 3.x** instalado no computador de controle.
2. Bibliotecas do ecossistema Arduino/ESP32:
   * `TinyGPS++` (para parsing de sentenças NMEA do NEO-6M).
   * `MQTT.h` (para cliente MQTT no ESP32).
3. Dependência Python para o script de controle:
   ```bash
   pip install paho-mqtt
   ```

### 1. Configuração do Firmware (ESP32)
1. Configure as credenciais da rede Wi-Fi e os parâmetros do Broker MQTT nos arquivos dos firmwares (`controle-helices/` e `main_gps/`).
2. Compile e faça o upload de `controle-helices/helices_mqtt.ino` no ESP32 dedicado ao controle das 3 hélices (sustentação + 2 propulsoras).
3. Compile e faça o upload de `main_gps/main_gps.ino` no ESP32/ESP32-display conectado ao módulo **NEO-6M**.

### 2. Execução do Controle Python
Para enviar comandos de flutuação, direcionamento e monitorar o veículo:
```bash
cd controle-python-mqtt
python controle_mqtt.py
```

---

## 🔬 Próximos Passos e Linhas de Pesquisa

- [ ] **Integração de Filtro de Kalman (EKF):** Fusão sensorial entre dados de IMU e do GPS NEO-6M para estimativa de pose precisa em dinâmica sem rodas.
- [ ] **Sondas de Monitoramento Ambiental:** Embarque de sensores para análise de água em manguezais (pH, salinidade, turbidez e oxigênio dissolvido).
- [ ] **Modelagem e Controle Subatuado:** Desenvolvimento de controladores para compensar o efeito de *drift* (deriva) provocado pela sustentação por colchão de ar.
- [ ] **Detecção de Obstáculos:** Embarque de sensores ultrassônicos/LiDAR para prevenção de colisões.

---

## 📚 Referências Científicas

* **BAHRI et al. (2022)** — *Small-scale hovercraft prototype for river exploration*.
* **ICMBio (2018)** — *Atlas dos Manguezais do Brasil*.
* **QUINCOZES et al. (2019)** — *Análise de Desempenho do Protocolo MQTT em Sistemas Embarcados*.