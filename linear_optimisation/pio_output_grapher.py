import matplotlib.pyplot as plt
import os
import numpy as np
import math

currentDirPath = os.getcwd()
parentDirPath = parent_directory = os.path.join(currentDirPath, '..')
logDirPath = os.path.join(parent_directory, 'arduino-fdc1004/logs')

files = os.listdir(logDirPath)

if len(files) == 0:
    print("There are no files in the target folder!")
    exit(0)

def map_range(value, from_min, from_max, to_min, to_max):
    # Ensure that the value is within the original range
    value = min(max(value, from_min), from_max)
    
    # Perform linear interpolation and mapping
    from_range = from_max - from_min
    to_range = to_max - to_min
    mapped_value = (value - from_min) * (to_range / from_range) + to_min
    
    return mapped_value



file_paths = [os.path.join(logDirPath, file) for file in files if os.path.isfile(os.path.join(logDirPath, file))]

# Sort the file paths by modification time in descending order
file_paths.sort(key=lambda x: os.path.getmtime(x), reverse=True)

most_recent_log_file = file_paths[0]
print(most_recent_log_file)

file = open(most_recent_log_file, "r")

r_values = []
l_values = []
e_values = []
c_values = []

while True:
    line = file.readline()
    if not line:
        break
    match line[0]:
        case "R":
            value = float(line[3:-1])
            r_values.append(value)
        case "L":
            value = float(line[3:-1])
            l_values.append(value)
        case "E":
            value = float(line[3:-1])
            e_values.append(value)
        case "O":
            value = float(line[3:-1])
            c_values.append(value)
        case _:
            continue

fig, (ax1, ax2) = plt.subplots(1, 2)  # Create new figures

# Cropping
# r_values = r_values[60:-90]
# l_values = l_values[60:-90]
# e_values = e_values[60:-90]
# c_values = c_values[60:-90]

x = np.array([i for i in range(len(r_values))])

# ax1.plot(x, r_values, label='Reference')  # Plot data set 1
ax1.plot(x, l_values, label='Level')  # Plot data set 2
ax2.plot(x, c_values, label='Calculated Level')  # Plot data set 2
# ax1.plot(x, e_values, label='Environment')  # Plot data set 3

ax1.set_xlabel('X')  # Set X-axis label
ax1.set_ylabel('Y')  # Set Y-axis label
ax2.set_title('Capacitive Level Output')  # Set plot title


# Line of Best Fit
m1, b = np.polyfit(x, l_values, 1)
trend_values = m1 * x + b
ax1.plot(x, trend_values, label="L TrendLine")

m2, b2 = np.polyfit(x, c_values, 1)
trend_values = m2 * x + b2
ax2.plot(x, trend_values, label="C Trendline")

print(f"C Trend Values: y = {m2}x + {b}")

# mapped_level = [map_range(i, min_trend, max_trend, 1, 10) for i in trend_values]
# ax2.plot(x, mapped_level, label="Mapped Level", color="green")

# ax2.plot(l_values, )

ax1.grid(True)  # Show grid lines
ax2.grid(True)  # Show grid lines
plt.tight_layout()
ax1.legend()  # Show legend
ax2.legend()  # Show legend
# plt.show()  # Show the plot

# Back Calculation Test
x = 8.7
m = 17.574
b = -135.149
y = m * x + b
print(f"y({x}) = {y}")
print(f"Level: {(round(y / 5) * 5)}")