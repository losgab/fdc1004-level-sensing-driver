import numpy as np
import matplotlib.pyplot as plt
import json

# 5mm length reference pad

# Change this stuff
ref_baseline = 1.81

ref_full = (2.4, 2.55) # 2.4
lev_baseline = (6.29, 6.38)

file = open("esp_data.json")
esp_data = json.load(file)

level_values = []
for key, value in esp_data:
    

level_values = [(6.29, 6.38), (6.6, 6.62), (7.30, 7.32), (7.89, 7.91), (8.48, 8.49), (8.94, 8.95), (9.64, 9.66), (10.13, 10.14), (10.70, 10.71), (11.17, 11.18), (11.75, 11.76), (12.45, 12.47), (12.92, 12.93), (13.51, 13.52), (14.10, 14.11), (14.67, 14.68)]
y = [(level_values[i][0] + level_values[i][1]) / 2 for i in range(len(level_values))]

x = [(i * 5 + 5) for i in range(16)]

plt.plot(x, y)

 # Trendline
z = np.polyfit(x, y, 1)
p = np.poly1d(z)

#add trendline to plot
plt.plot(x, p(x))
plt.title("y=%.2fx+%.2f"%(z[0],z[1]))
plt.show()

file.close()