# eixos(axes): analogicos, floats entre -1.0 e 1.0 (0.0 repouso)
# botoes(buttons): digitais, 0-solto e 1-pressionado
import paho.mqtt.client as mqtt
# import time
import sys
import pygame

# constantes
FPS = 60
DEADZONE = 0.15  # no analogico, qualquer valor <0.15 sera =0
SPEED = 1.0

BROKER = "test.mosquitto.org"
PORT = 1883


# subscribe
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("--> Conectado com sucesso ao Broker MQTT!")
    else:
        print(f"--> Falha na conexão MQTT, código: {rc}")


client = mqtt.Client()
client.on_connect = on_connect
client.connect("test.mosquitto.org", 1883, 60)

client.loop_start()


def main() -> None:
    # inicializacao do pygame
    pygame.init()
    pygame.joystick.init()
    clock = pygame.time.Clock()

    # inicializacao e conexao com os joysticks
    joystick = None
    if pygame.joystick.get_count() > 0:
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f"Controle conectado: {joystick.get_name()}")
    else:
        print("Nenhum joystick detectado! Conecte um controle.")

    # loop principal
    running = True
    while running:
        # eventos de finalização da simulação
        for event in pygame.event.get():
            # finaliza ao fechar aba no computador
            if event.type == pygame.QUIT:
                print("simulação encerrada")
                running = False
            # finaliza ao apertar a tecla esc no teclado
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                print("simulação encerrada")
                running = False
            # IMPRIME QUALQUER BOTÃO PRESSIONADO
            elif event.type == pygame.JOYBUTTONDOWN:
                print(f"--> Botão pressionado: ID {event.button}")
            elif event.type == pygame.JOYAXISMOTION:
                if abs(event.value) > DEADZONE:
                    print(f"--> Eixo {event.axis} moveu: {event.value:.2f}")

        # eventos de açao
        if joystick:
            # JOYSTICK ESQUERDO (eixos 0 e 1)
            left_x = joystick.get_axis(0)
            left_y = joystick.get_axis(1)

            left_motion_x = left_x * SPEED if abs(left_x) > DEADZONE else 0.0
            left_motion_y = left_y * SPEED if abs(left_y) > DEADZONE else 0.0

            # envio do sinal para o mqtt
            client.publish("helice_esquerda_x", str(round(left_motion_x, 2)))
            client.publish("helice_esquerda_y", str(round(left_motion_y, 2)))

            # JOYSTICK DIREITO (CONFIGURADO COMO 4 BOTOES ISOLADOS)
            # Reseta o movimento do stick direito no frame atual
            right_motion_x = 0.0
            right_motion_y = 0.0

            # Mapeamento dos botões para direções (Ajuste os IDs conforme o seu controle)
            if joystick.get_button(2):  # direira
                right_motion_x += SPEED
            if joystick.get_button(0):  # esquerda
                right_motion_x -= SPEED
            if joystick.get_button(3):  # Cima
                right_motion_y += SPEED
            if joystick.get_button(1):  # Baixo
                right_motion_y -= SPEED
            
            client.publish("helice_direita_x", str(round(right_motion_x, 2)))
            client.publish("helice_direita_y", str(round(right_motion_y, 2)))

            # HELICE CENTRO = BOTOES POSTERIORES
            # 4 e 5 devem aumentar
            # 6 e 7 devem diminuir
            post_motion_y = 0.0

            if joystick.get_button(4) or joystick.get_button(5):
                post_motion_y += SPEED  # apenas pra testar funcionamento
            if joystick.get_button(6) or joystick.get_button(7):
                post_motion_y -= SPEED

            client.publish("helice_centro_y", str(round(post_motion_y, 2)))

        clock.tick(FPS)

    client.loop_stop()
    client.disconnect()
    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
