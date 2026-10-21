import paho.mqtt.client as mqtt
import sys
import pygame

# constantes
FPS = 60
DEADZONE = 0.15  # no analogico, qualquer valor <0.15 sera =0
SPEED = 1.0

# Botões da Hélice Central
BUTTON_LIFT_ON = 5   # Liga no máximo
BUTTON_LIFT_OFF = 4  # Desliga totalmente

# dados do mqtt
BROKER = "test.mosquitto.org"
PORT = 1883
TOPIC_PREFIX = "hovercraft/ect/"  # Prefixo para evitar conflito no broker público


# lugar para guardar o último valor enviado
last_sent_values = {
    "helice_esquerda_x": None,
    "helice_esquerda_y": None,
    "helice_direita_x": None,
    "helice_direita_y": None,
    "helice_centro_y": None,
}


# conexão mqtt
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("--> Conectado com sucesso ao Broker MQTT!")
    else:
        print(f"--> Falha na conexão MQTT, código: {rc}")


def setup_mqtt() -> mqtt.Client:
    # compatível tanto com paho-mqtt 1.x quanto 2.x
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    except AttributeError:
        client = mqtt.Client()

    client.on_connect = on_connect
    print(f"Conectando a {BROKER}:{PORT}...")
    client.connect(BROKER, PORT, 60)
    client.loop_start()
    return client


def publish_if_changed(client: mqtt.Client, topic_suffix: str, value: float, threshold: float = 0.05):
    # envia apenas se houver mudanças
    topic = TOPIC_PREFIX + topic_suffix
    last_val = last_sent_values.get(topic_suffix)

    if last_val is None or abs(value - last_val) >= threshold or (value == 0.0 and last_val != 0.0):
        client.publish(topic, str(round(value, 2)))
        last_sent_values[topic_suffix] = value


def main() -> None:
    # inicializacao do pygame
    pygame.init()
    pygame.joystick.init()

    # Cria uma pequena janela para permitir foco no teclado (ESC) e fechar (X)
    screen = pygame.display.set_mode((400, 200))
    pygame.display.set_caption("Hovercraft Controller")
    clock = pygame.time.Clock()

    client = setup_mqtt()

    # inicializacao e conexao com os joysticks
    joystick = None
    if pygame.joystick.get_count() > 0:
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f"Controle conectado: {joystick.get_name()}")
    else:
        print("Nenhum joystick detectado!\n")

    # Sustentação central: 1.0 (Máximo / 180° no ESP32) ou -1.0 (Desligado / 0° no ESP32)
    lift_power = -1.0  # Inicia desligado

    # loop principal
    running = True
    while running:
        # Tratamento de eventos
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False

        if joystick:
            # JOYSTICK ESQUERDO (Eixos 0 e 1)
            raw_lx = joystick.get_axis(0)
            raw_ly = -joystick.get_axis(1)

            left_motion_x = raw_lx * SPEED if abs(raw_lx) > DEADZONE else 0.0
            left_motion_y = raw_ly * SPEED if abs(raw_ly) > DEADZONE else 0.0

            publish_if_changed(client, "helice_esquerda_x", left_motion_x)
            publish_if_changed(client, "helice_esquerda_y", left_motion_y)

            # JOYSTICK DIREITO (Botões 0, 1, 2, 3)
            right_motion_x = 0.0
            right_motion_y = 0.0

            if joystick.get_button(2):  # Direita
                right_motion_x -= SPEED
            if joystick.get_button(0):  # Esquerda
                right_motion_x += SPEED
            if joystick.get_button(3):  # Cima
                right_motion_y -= SPEED
            if joystick.get_button(1):  # Baixo
                right_motion_y += SPEED

            publish_if_changed(client, "helice_direita_x", right_motion_x)
            publish_if_changed(client, "helice_direita_y", right_motion_y)

            # 3. HÉLICE CENTRAL
            # Um botão liga no máximo (1.0) e outro desliga totalmente (-1.0)
            if joystick.get_button(BUTTON_LIFT_ON):
                lift_power = 1.0
            elif joystick.get_button(BUTTON_LIFT_OFF):
                lift_power = -1.0

            publish_if_changed(client, "helice_centro_y", lift_power)

        # Atualiza a interface
        screen.fill((30, 30, 30))
        pygame.display.flip()
        clock.tick(FPS)

    print("Finalizando conexões e encerrando")
    client.loop_stop()
    client.disconnect()
    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
