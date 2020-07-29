# Author: Kali Regenold
# Date: July 2020

# Color grabber for my led strip.
# Sends RGB Red to Arduino over serial.
# Requests colors from colormind.io API.
# TODO: Send colormind colors to Arduino.
#       Cull the colors, some won't show up nicely.

import requests
import serial
import time
import palette_test

# Serial port to Arduino
ser = serial.Serial('/dev/ttyACM0', 9600)

# Red, Green, Blue testing colors
test_colors = [[255,   0,   0],
               [  0, 255,   0],
               [  0,   0, 255]]

# Get colors from colormind.io's API
def get_colors():
    r = requests.post('http://colormind.io/api/', data='{"model":"default"}')
    return r.json()['result']

# Return a single int value as a bytes object
def convert_to_bytes(num):
    return bytes([num])

# Write an RGB value over serial
def send_color(color):
    print("send_color")
    ser.write(bytes([color[0]]))
    ser.write(bytes([color[1]]))
    ser.write(bytes([color[2]]))

def send_palette(colors):
    packet_size = len(colors)
    ser.write(bytes([packet_size]))
    for c in colors:
        send_color(c)

# Send a single color to the Arduino.
def run_colors():
    # Sleep to let serial connection settle
    time.sleep(3)

    palette = get_colors()
    send_palette(palette)
    print(palette)
    palette_test.plot_colors(palette)

if __name__ == "__main__":
    run_colors()
