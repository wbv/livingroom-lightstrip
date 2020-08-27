#!/usr/bin/python3

# Author: Kali Regenold
# Date: July 2020

# Color grabber for my led strip.
# Requests colors from colormind.io API.
# Sends them to an Arduino over a Serial USB cable.
# TODO: Cull the colors, some won't show up nicely.

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
    ser.write(bytes([color[0]]))
    ser.write(bytes([color[1]]))
    ser.write(bytes([color[2]]))

# Send the palette size and palette colors to Arduino
def send_palette(colors):
    palette_size = len(colors)
    ser.write(bytes([palette_size]))
    for c in colors:
        send_color(c)

# Send a single color to Arduino
def run_colors():
    palette = get_colors()
    for c in palette:
        print(c)
    send_palette(palette)
    palette_test.plot_colors(palette)

if __name__ == "__main__":
    # Sleep to let serial connection settle
    time.sleep(3)
    run_colors()
