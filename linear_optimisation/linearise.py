import numpy as np
import matplotlib.pyplot as plt
import json

# 5mm length reference pad

# Change this stuff
ref_baseline = 1.81

ref_full = (2.4, 2.55) # 2.4
lev_baseline = (6.29, 6.38)

# Extracting data from json
file = open("esp_data.json")
esp_data = json.load(file)

level_values = []
for key in esp_data.keys():
    level_values.append(esp_data[key])

# level_values = [(6.29, 6.38), (6.6, 6.62), (7.30, 7.32), (7.89, 7.91), (8.48, 8.49), (8.94, 8.95), (9.64, 9.66), (10.13, 10.14), (10.70, 10.71), (11.17, 11.18), (11.75, 11.76), (12.45, 12.47), (12.92, 12.93), (13.51, 13.52), (14.10, 14.11), (14.67, 14.68)]
y = [(level_values[i][0] + level_values[i][1]) / 2 for i in range(len(level_values))]
y_delta = [round(abs(y[i] - y[i + 1]), 2) for i in range(len(y) - 1)]
print(y_delta)
print(f"Average delta: {sum(y_delta) / len(y_delta)}")

# X values
x = [(i * 5 + 5) for i in range(16)]

plt.plot(x, y)

 # Trendline
z = np.polyfit(x, y, 1)
p = np.poly1d(z)

#add trendline to plot
line, = plt.plot(x, p(x))
line.set_label('Label via method')
plt.title("y=%.2fx+%.2f"%(z[0],z[1]))
plt.show()

def predict_level(x):
    return ((x - 5.63) / 0.11)

def round5(x):
    return round(x / 5) * 5

for level in y:
    prediction = predict_level(level)
    rounded = round5(prediction)
    print(f"Predicted level for {level}: {rounded}")

# Average delta = 0.556
# Trendline function: y = 0.11x + 5.63
# Inverse function: y = (x - 5.63) / 0.11

file.close()

# Tolerance for nominal level +- 0.278

# Predicted Level for 