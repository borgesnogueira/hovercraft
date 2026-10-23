# Sistema de Controle via MQTT para Hovercraft

Este diretório contém o script de teleoperação em Python ([controle.py](file:///c:/Users/isaca/OneDrive/Desktop/Meus%20Projetos/hovercraft_particular/controller/controle.py)) que lê entradas de um joystick USB/Bluetooth e as transmite via **MQTT** para o firmware do ESP32 ([helices.ino](file:///c:/Users/isaca/OneDrive/Desktop/Meus%20Projetos/hovercraft_particular/firmware/lib/controller_mqtt/helices.ino)).

---

## 🏗️ Arquitetura do Sistema

```
┌──────────────────────┐          MQTT (WiFi)           ┌──────────────────────┐
│  Controle Python     │  ───────────────────────────>  │  ESP32 Hovercraft    │
│  (Pygame + paho-mqtt)│     Broker: test.mosquitto.org │  (helices.ino)       │
└──────────────────────┘                                └──────────────────────┘
        │                                                           │
   Joystick USB                                             Servos / ESCs
 (Analógico + Botões)                                     (Pinos 12, 13 e 14)
```

---

## 📡 Tópicos MQTT e Protocolo de Dados

Todos os tópicos utilizam o prefixo configurável `hovercraft/ect/` para evitar colisões com outros dispositivos no broker público.

| Tópico Completo | Faixa de Valores | Função no Hovercraft |
| :--- | :--- | :--- |
| `hovercraft/ect/helice_esquerda_x` | `-1.00` a `1.00` | Eixo horizontal do stick analógico esquerdo |
| `hovercraft/ect/helice_esquerda_y` | `-1.00` a `1.00` | Eixo vertical do stick analógico esquerdo (inversão aplicada: frente = +1.0) |
| `hovercraft/ect/helice_direita_x` | `-1.00` a `1.00` | Botões direcionais / D-pad (Esquerda = -1.0, Direita = +1.0) |
| `hovercraft/ect/helice_direita_y` | `-1.00` a `1.00` | Botões direcionais / D-pad (Baixo = -1.0, Cima = +1.0) |
| `hovercraft/ect/helice_centro_y` | `-1.00` ou `1.00` | **Hélice Central (Sustentação):** Liga no Máximo (`1.0`) ou Desliga (`-1.0`) |

### Conversão de Ângulo no ESP32 (`helices.ino`)
Os valores flutuantes de `-1.0` a `+1.0` são mapeados na placa para ângulos PWM de servo / ESC (de 0° a 180°):
- `-1.0` $\rightarrow$ **0°** (1000 µs - Desligado / Mínimo)
- `0.0` $\rightarrow$ **90°** (1500 µs - Centro / Neutro)
- `+1.0` $\rightarrow$ **180°** (2000 µs - Máximo / 100%)

---

## 🎮 Mapeamento do Joystick

| Comando | Entrada no Controle | Descrição |
| :--- | :--- | :--- |
| **Direção / Propulsão Esquerda** | **Stick Analógico Esquerdo** | Eixo 0 (X) e Eixo 1 (Y com inversão de sinal) |
| **Comandos Direita** | **Botões Frontais (0, 1, 2, 3)** | Botão 0 (Esq), 2 (Dir), 3 (Cima), 1 (Baixo) |
| **Sustentação Máxima (ON)** | **Botão 5 (R1 / RB)** | Coloca a hélice central em 100% (`1.0`) e mantém o estado |
| **Sustentação Desligada (OFF)**| **Botão 4 (L1 / LB)** | Desliga totalmente a hélice central (`-1.0`) |

> *Para alterar os botões da hélice central, basta modificar as constantes `BUTTON_LIFT_ON` e `BUTTON_LIFT_OFF` no topo de [controle.py](file:///c:/Users/isaca/OneDrive/Desktop/Meus%20Projetos/hovercraft_particular/controller/controle.py).*

---

## ⚡ Recursos e Otimizações Implementadas

1. **Anti-Flood por *Threshold*:**
   - O código não envia 300 mensagens por segundo na rede. Ele compara a leitura atual com a última enviada e **só publica se a variação for $\ge 0.05$** (5%), poupando a banda da internet e o buffer do ESP32.
2. **Deadzone Analógica:**
   - Leituras dentro da faixa neutra ($-0.15$ a $+0.15$) são automaticamente tratadas como `0.0`, impedindo que ruído mecânico do analógico mova o robô sozinho.
3. **Hotplug de Controle:**
   - Utiliza os eventos `JOYDEVICEADDED` e `JOYDEVICEREMOVED` do Pygame 2. Você pode conectar ou desconectar o controle USB/Bluetooth com o script rodando sem que o programa trave.
4. **Compatibilidade Paho-MQTT 1.x e 2.x:**
   - Implementa detecção automática da API Version para não gerar avisos ou quebras com as versões mais recentes da biblioteca.

---

## 🚀 Como Executar o Controle Python

### 1. Pré-requisitos
Certifique-se de que o Python 3.8+ está instalado em seu computador.

### 2. Instalação das Dependências
Na pasta raiz do projeto ou dentro da pasta `controller`, execute no terminal:

```bash
pip install -r controller/requirements.txt
```

### 3. Execução
Conecte o seu joystick no computador e execute:

```bash
python controller/controle.py
```

Uma janela do Pygame se abrirá para manter o foco do teclado (pressione **ESC** ou feche a janela para encerrar com segurança).

---

## 🔌 Configuração do ESP32 ([helices.ino](file:///c:/Users/isaca/OneDrive/Desktop/Meus%20Projetos/hovercraft_particular/firmware/lib/controller_mqtt/helices.ino))

### Pinagem:
- **Pino 13 (`HEL_CENT`):** Servo / ESC da Hélice Central (Sustentação)
- **Pino 12 (`HEL_ESQ`):** Servo / ESC da Hélice Esquerda
- **Pino 14 (`HEL_DIR`):** Servo / ESC da Hélice Direita

> [!CAUTION]
> **Alimentação dos Motores:** Nunca alimente servos ou ESCs diretamente pelos pinos de 3.3V ou 5V do ESP32 via cabo USB. O pico de corrente ao acelerar ou mover os servos desarmará o regulador interno do ESP32 por subtensão (*Brownout Detector*). Utilize uma fonte externa ou bateria dedicada com os **GNDs (terras) interligados**.