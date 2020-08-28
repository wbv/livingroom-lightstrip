#!/usr/bin/python3

# Author: Kali Regenold
# Date: July 2020

# Requests a 5-color palette from colormind.io and saves graph to image.

import requests
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle as rect
import numpy as np


def plot_colors(color_list):
    list_sz = len(color_list)
    fig = plt.figure()
    ax = fig.add_subplot(111)

    for i in range (0, list_sz):
        color = tuple([x/255 for x in color_list[i]])
        rectangle = rect((i,0),1,list_sz,color=color)
        ax.add_patch(rectangle)

    plt.xlim([0,list_sz])
    plt.ylim([0,list_sz])
    # plt.show()
    plt.savefig('palette.png', bbox_inches='tight')

if __name__ == "__main__":
    r = requests.post('http://colormind.io/api/', data='{"model":"default"}')
    color_list = r.json()['result']
    plot_colors(color_list)
