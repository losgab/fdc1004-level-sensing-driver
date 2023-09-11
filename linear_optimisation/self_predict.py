import numpy as np
import matplotlib.pyplot as plt
import json

# Formatting
print("------------------------")

# 5mm length reference pad

# Measurements
ref_baseline = 1.81
ref_full = 2.4
STEP_DELTA = ref_full - ref_baseline
lev_baseline = 6.28

# Anything within range of this is between 0 and 5
print(f"Ref Full: {ref_full}")
print(f"Calculated Level Delta from Ref (5mm): {STEP_DELTA}")


# Extracting data from json
def extract_data():
    file = open("esp_data.json")
    esp_data = json.load(file)

    level_values = []
    for key in esp_data.keys():
        level_values.append(esp_data[key])
    file.close()
    return level_values

def round5(x):
    return round(x / 5) * 5

# True level values
x = [(i * 5 + 5) for i in range(16)]

# Data Values
level_values = extract_data()
y = [(level_values[i][0] + level_values[i][1]) / 2 for i in range(len(level_values))]
y_delta = [round(abs(y[i] - y[i + 1]), 2) for i in range(len(y) - 1)]
average_real_delta = sum(y_delta) / len(y_delta)
print(f"Average real delta (5mm): {average_real_delta}")

# Data & trendline
plt.plot(x, y)
z = np.polyfit(x, y, 1)
data_m = round(z[0], 2)
data_b = round(z[1], 2)
p = np.poly1d(z)
plt.plot(x, p(x), label = f"Data: y = {data_m}x + {data_b}")

FORECAST_DISTANCE = 20

# Forecast line based on ref sensor
x = [(i * 5 + 5) for i in range(FORECAST_DISTANCE)]
forecast_y = [round(STEP_DELTA * i + lev_baseline, 2) for i in range(int(FORECAST_DISTANCE))] # Could include the gain and offset factoring here

# Find forecast gradient and intercept
predicted_z = np.polyfit(x, forecast_y, 1)
forecast_m = round(predicted_z[0], 3)
forecast_b = round(predicted_z[1], 3)
p = np.poly1d(predicted_z)
plt.plot(x, forecast_y, label = f"Forecast: y = {forecast_m}x + {forecast_b}")

# Determine appropriate linear correction based on forecast & target
CORRECTION_GAIN = 1 / forecast_m
CORRECTION_OFFSET = - CORRECTION_GAIN * forecast_b
print(f"Correction Gain: {CORRECTION_GAIN}")
print(f"Correction Offset: {CORRECTION_OFFSET}")

# Apply linear correction
corrected_y = [CORRECTION_GAIN * val + CORRECTION_OFFSET for val in forecast_y]
plt.plot(x, corrected_y, label = f"Correction")

for i in range(len(corrected_y)):
    actual_level = (i + 1) * 5
    forecast_value = round(forecast_y[i], 2)
    corrected_value = round(corrected_y[i], 2)

    level_prediction = round5(corrected_value)
    error = (abs(level_prediction - actual_level) / ((level_prediction + actual_level) / 2)) * 100
    print(f"Raw: {forecast_value} || Corrected Value: {corrected_value} || % Error from Actual ({actual_level}): {round(error, 2)}")

plt.legend()
plt.grid()
plt.show()