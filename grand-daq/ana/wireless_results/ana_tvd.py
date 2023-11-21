#!/usr/bin/env python3
#Created by duanbh, 2022/8/20

import os
import re
import matplotlib.pyplot as plt
import numpy as np
from sys import argv
from matplotlib.ticker import FuncFormatter
from datetime import datetime
import matplotlib.mlab as mlab

n = 0
y_after_min = 0
y_after_max = 0
event_n =[]
t_before_Dvalue = []
t_after_Dvalue = []
t_before_Dvalue.append(0)
# t_after_Dvalue.append(0)
event_n.append(0)

filename = argv[1]
now = datetime.now()
t_mkdir = now.strftime("%Y-%m-%d-%H-%M-%S")

# path to save figures.
pathToSaveFig = '/home/grand/workarea/grand-daq/ana/wireless_results/pngs/' + t_mkdir
if not os.path.exists(pathToSaveFig):
    os.makedirs(pathToSaveFig)

filename = argv[1]
with open(filename) as f:
    l = f.readline()
    while l:
        m = re.match(r"Before_Dval is (.*), After_Dvalue is (.*)", l)
        if m:
            t_before = int(m.group(1))
            t_after = int(m.group(2))
            if n==1:
               y_after_min = int(m.group(2)) - 2000
               y_after_max = int(m.group(2)) + 2000
            t_before_Dvalue.append(t_before)
            t_after_Dvalue.append(t_after)
            n+=1
            event_n.append(n)
        l = f.readline()
plt.figure(figsize=(16,9))
plt.title('Time Dvalue')
#plt.plot(event_n, t_before_Dvalue, label = 'BEFORE_time_difference')
#plt.plot(event_n, t_after_Dvalue, label = 'AFTER_time_difference')
#plt.scatter(event_n, t_after_Dvalue, s=5,label = 'AFTER_time_difference')
t_after_Dvalue_sz = len(t_after_Dvalue)
weights = np.ones_like(t_after_Dvalue)/float(t_after_Dvalue_sz)
n, bins, patches = plt.hist(t_after_Dvalue, bins=100, weights=weights, density = False)
plt.plot(label = 'BEFORE_time_difference')
plt.xlim(min(t_after_Dvalue), max(t_after_Dvalue))
#plt.ylim( y_after_min, y_after_max)
plt.xlabel('Event number')
plt.ylabel('Time difference distribution between two successful events')
plt.grid(True)
plt.xticks(bins,bins,fontsize=10,rotation=270)
plt.legend(fontsize=10, markerscale=2, scatterpoints=1)
#plt.legend()
figname = filename
plt.savefig(pathToSaveFig + '/' + figname + '.png')
plt.show()
