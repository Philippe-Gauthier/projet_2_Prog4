# file code.py
# author Samuel Faucher
# date mars 2025
# brief Code generique, d envoie des boutons de la manette en JSON sur espnow
# MAJ: nouveau pinout ESP32-S3 feather (CAN interne, plus de MCP3008 externe)
import time
import board
import digitalio
import analogio
import wifi
import espnow
import json

DELAY = 0.25     # sec

# ESPnow ------------------------------------------
#https://docs.circuitpython.org/en/8.2.x/shared-bindings/espnow/
e = espnow.ESPNow()
peer = espnow.Peer(mac=b'\x44\x1B\xF6\x89\x21\x0C')     # A modifier avec l adresse du vehicule desire
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
    #button = digitalio.Direction.INPUT
    buttons.update({button_name:button_temp})

# Joystick, trigger through native ESP32-S3 ADC -----------------
pot_pins = {
    "jLX": board.SDA,
    "jLY": board.SCL,
    "jRX": board.D5,
    "jRY": board.D6,
    #"tL": board.MOSI,
    #"tR": board.SCK
}

pots = {}
for pot_name in pot_pins:
    pot_temp = analogio.AnalogIn(pot_pins[pot_name])
    pots.update({pot_name:pot_temp})

# Helper function to scale analog values
# max theoritical value = 65535, real max value= 61523
def scale_to_percent(analog_value, max_value=61523, centered=True):
    """
    Convert analog input value to percentage
    Native ESP32-S3 ADC via analogio.AnalogIn returns 12-bit values (0-4095)

    If centered=True: Maps to -100 to 100 with center at 0 (for joysticks)
    If centered=False: Maps to 0 to 100 (for triggers)
    """
    if centered:
        # Map to -100 to 100 range with center at 0 (inverted)
        normalized = (analog_value / max_value) * 2 - 1  # Convert to -1 to 1
        percent = int(normalized * -100)  # Multiply by -100 to invert
        return max(-100, min(100, percent))  # Clamp between -100 and 100
    else:
        # Map to 0 to 100 range
        percent = int((analog_value / max_value) * 100)
        return max(0, min(100, percent))  # Clamp between 0 and 100

# main --------------------------------------------------------------------------
while True:
#    print(f"My MAC address: {[hex(i) for i in wifi.radio.mac_address]}")
#     send all inputs --------------------------------------
    msg = {}

    # Add button values
    for button_name in buttons:
        msg.update({button_name:buttons[button_name].value})

    # Add joystick/trigger values with appropriate scaling
    for pot_name in pots:
        raw_value = pots[pot_name].value
        # Joysticks use centered scaling (-100 to 100), triggers use 0-100
        if pot_name.startswith('j'):  # jLX, jLY, jRX, jRY are joysticks
            scaled_value = scale_to_percent(raw_value, centered=True)
            # Invert Y-axis values
            if pot_name.endswith('Y'):  # jLY or jRY
                scaled_value = -scaled_value
        else:  # tL, tR are triggers
            scaled_value = scale_to_percent(raw_value, centered=False)
        msg.update({pot_name:scaled_value})

    print(json.dumps(msg))     # decommenter pour afficher les valeurs (tests fonctionnels)
    e.send(json.dumps(msg))     # max 250 bytes (https://docs.espressif.com/projects/esp-faq/en/latest/application-solution/esp-now.html#:~:text=The%20maximum%20length%20does%20not,is%20limited%20to%20250%20bytes.)

    time.sleep(DELAY)