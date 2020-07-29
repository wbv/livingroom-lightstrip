# Author: Kali Regenold
# Date: July 2020

# Requests a 5-color palette from colormind.io and graphs it.

import requests
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle as rect
import numpy as np

r = requests.post('http://colormind.io/api/', data='{"model":"default"}')
color_list = r.json()['result']

fig = plt.figure()
ax = fig.add_subplot(111)

for i in range (0,5):
    color = tuple([x/255 for x in color_list[i]])
    rectangle = rect((i,0),1,5,color=color)
    ax.add_patch(rectangle)

plt.xlim([0,5])
plt.ylim([0,5])
plt.show()
