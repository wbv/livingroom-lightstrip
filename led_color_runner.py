#!/usr/bin/python3

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
    ser.write(bytes([color[0]]))
    ser.write(bytes([color[1]]))
    ser.write(bytes([color[2]]))

def send_palette(colors):
    packet_size = len(colors)
    ser.write(bytes([packet_size]))
    for c in colors:
        send_color(c)

#https://www.rapidtables.com/convert/color/rgb-to-hsv.html
def RGB_to_HSV(color):
    hsv = [0,0,0]

    rgb_p = [x for x in color]
    c_max = max(rgb_p)
    c_min = min(rgb_p)
    delta = c_max - c_min

    # Hue
    if delta == 0:
        hsv[0] = 0
    elif c_max == rgb_p[0]:
        hsv[0] = 60*(((rgb_p[1]-rgb_p[2])/delta)%6)
    elif c_max == rgb_p[1]:
        hsv[0] = 60*(((rgb_p[2]-rgb_p[0])/delta)+2)
    elif c_max == rgb_p[2]:
        hsv[0] = 60*(((rgb_p[0]-rgb_p[1])/delta)+4)

    # Saturation
    if c_max == 0:
        hsv[1] = 0
    else:
        hsv[1] = delta/c_max

    # Value
    hsv[2] = c_max

    # Remap to byte-sized pieces
    hsv[0] = int(map(hsv[0], 0, 360, 0, 255))
    hsv[1] = int(map(hsv[1], 0, 1, 0, 255))

    return hsv

def map(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

# Send a single color to the Arduino.
def run_colors():
    # Sleep to let serial connection settle

    palette = get_colors()
    hsv_palette = [RGB_to_HSV(x) for x in palette]
    for c in hsv_palette:
        print(c)
    send_palette(hsv_palette)
    # palette_test.plot_colors(palette)

if __name__ == "__main__":
    time.sleep(3)
    run_colors()
