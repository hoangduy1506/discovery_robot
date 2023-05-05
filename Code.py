import serial
import math
import time
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import mplot3d

arduino = serial.Serial(
    port='COM17',
    baudrate=115200
)
time.sleep(1)

num = input("Enter a number: ") # Taking input from user
arduino.write(bytes(num, 'utf-8'))
time.sleep(0.05)

plt.ion()
fig = plt.figure(dpi=200, figsize=(10,10))
ax = fig.add_subplot(111, projection='3d')
ax.scatter(0, 0, 0, s=10, c = 'red')

ax.set_xlim(-50, 50)
ax.set_ylim(-50, 50)
ax.set_zlim(0, 50)

xlist=[]
ylist=[]
zlist=[]
j=0

limit_of_data = True

while limit_of_data:
    while arduino.in_waiting == 0:
        pass
    dataPacket = arduino.readline()
    dataPacket  = str(dataPacket, 'utf-8')    
    dataPacket  = dataPacket.strip('\r\n')
    print(dataPacket)
    if (dataPacket == 'End Data'):
        limit_of_data = False
    else:
        splitData = dataPacket.split(',')
        dist    = float(splitData[2])
        pitch   = float(splitData[1]) *3.14/180
        yaw     = float(splitData[0]) *3.14/180
        x = dist*(np.cos(pitch))*(np.cos(yaw))
        y = dist*(np.cos(pitch))*(np.sin(yaw))
        z = dist*(np.sin(pitch))

        xlist.append(x)
        ylist.append(y)
        zlist.append(z)
        
        ax.scatter(xlist[j],ylist[j],zlist[j],s=0.01,c='green')     
        j += 1
        plt.show()
        plt.pause(0.0001)

ax.view_init(elev=0, azim=-90)   
print("Done")
