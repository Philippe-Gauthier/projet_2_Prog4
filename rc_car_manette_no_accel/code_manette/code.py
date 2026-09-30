# file code.py
# author Samuel Faucher
# date septembre 2026
# brief Code generique, d envoie des boutons de la manette en JSON sur espnow
# Compatibilite: PCB V3

import time
import busio
import board
import digitalio
import analogio
import wifi
import espnow
import json
from adafruit_simplemath import map_range

DELAY = 0.25     # sec
POT_MIN = -100.0
POT_MAX = 100.0
ANALOG_MIN = 0.0
ANALOG_MAX = 65535.0
# ESPnow ------------------------------------------
#https://docs.circuitpython.org/en/8.2.x/shared-bindings/espnow/
e = espnow.ESPNow()
peer = espnow.Peer(mac=b'\xdc\x54\x75\xda\x73\xf4')     # A modifier avec l adresse du vehicule desire
e.peers.append(peer)

print(f"My MAC address: {[hex(i) for i in wifi.radio.mac_address]}")

# Buttons -----------------------------------------
# Note: pull_up are external
button_pins = {
    "A": board.A5,
    "B": board.A4,
    "X": board.A2,
    "Y": board.A3,
    "RB": board.A0,
    "LB": board.D13,
    "RT": board.SCK,
    "LT": board.MOSI,
    "Se": board.D12,
    "St": board.A1,
    "LJB": board.RX,
    "RJB": board.TX,
    "U": board.D11,
    "D": board.D9,
    "L": board.MISO,
    "R": board.D10
}
buttons = {}
for button_name in button_pins:
    button_temp = digitalio.DigitalInOut(button_pins[button_name])
    button = digitalio.Direction.INPUT
    buttons.update({button_name:button_temp})


# Joystick through native ESP32-S3 ADC -----------------
pot_pins = {
    "jLX": board.SDA,
    "jLY": board.SCL,
    "jRX": board.D5,
    "jRY": board.D6
}
pots = {}
for pot_name in pot_pins:
    pot_temp = analogio.AnalogIn(pot_pins[pot_name])
    pots.update({pot_name:pot_temp})


# main --------------------------------------------------------------------------
while True:

    # send all inputs --------------------------------------
    msg = {}
    for button_name in buttons:
        msg.update({button_name:buttons[button_name].value})

    for pot_name in pots:
        msg.update({pot_name:int(map_range(pots[pot_name].value, ANALOG_MIN, ANALOG_MAX, POT_MIN, POT_MAX))})

    #print(json.dumps(msg))
    e.send(json.dumps(msg))     # max 250 bytes (https://docs.espressif.com/projects/esp-faq/en/latest/application-solution/esp-now.html#:~:text=The%20maximum%20length%20does%20not,is%20limited%20to%20250%20bytes.)

    time.sleep(DELAY)
