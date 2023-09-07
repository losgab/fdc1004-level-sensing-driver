import numpy as np
import matplotlib.pyplot as plt
import json

# Formatting
print("------------------------")

# 5mm length reference pad

# Measurements
ref_baseline = 1.81
ref_full = (2.4, 2.55)
ref_average = sum(ref_full) / 2
calculated_delta = ref_average - ref_baseline

# Anything within range of this is between 0 and 5
print(f"Ref Average: {ref_average}")
print(f"Calculated Level Delta from Ref (5mm): {calculated_delta}")

lev_baseline = 6.29

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

FORECAST_DISTANCE = 100

# Forecast line based on ref sensor
x = [(i * 5 + 5) for i in range(int(FORECAST_DISTANCE / 5))]
forecast_y = [round(lev_baseline + i * calculated_delta, 2) for i in range(int(FORECAST_DISTANCE / 5))] # Could include the gain and offset factoring here

# Get linear properties of forecast
predicted_z = np.polyfit(x, forecast_y, 1)

a = len(forecast_y) * (sum([x[i] * forecast_y[i] for i in range(len(forecast_y))]))
b = sum(forecast_y) * sum(x)
c = len(forecast_y) * (sum([value**2 for value in x]))
d = sum(x)**2

m = (a - b) / (c - d)
print(f"Manual Slope 1: {m}")

def slope2(x, y):
    numerator = denominator = 0
    x_mean = np.mean(x)
    y_mean = np.mean(y)

    for i in range(len(y)):
        numerator += (x[i] - x_mean) * (y[i] - y_mean)
        denominator += (x[i] - x_mean) * (x[i] - x_mean)

    return (numerator / denominator)
    
print(f"Manual Slope 2: {slope2(x, forecast_y)}")

forecast_m = round(predicted_z[0], 3)
forecast_b = round(predicted_z[1], 3)
p = np.poly1d(predicted_z)
plt.plot(x, forecast_y, label = f"Forecast: y = {forecast_m}x + {forecast_b}")
# print(f"Predicted Primitives: {forecast_y}")

# Determine appropriate linear correction based on forecast & target
FORECAST_GAIN = average_real_delta / calculated_delta # (Average delta) / (Measured delta for 5mm)
FORECAST_OFFSET = 0
print(f"Forecast Gain: {FORECAST_GAIN}")
print(f"Forecast Offset: {FORECAST_OFFSET}")

# Apply linear correction
corrected_y = [(forecast_m * FORECAST_GAIN) * val + (forecast_b + FORECAST_OFFSET) for val in x]
plt.plot(x, corrected_y, label = f"Corrected")

for i in range(len(corrected_y)):
    actual_level = (i + 1) * 5
    corrected_value = round(corrected_y[i], 2)
    raw_prediction = round((corrected_value - data_b) / (data_m), 2)
    level_prediction = round5(raw_prediction)
    error = (abs(raw_prediction - actual_level) / actual_level) * 100
    print(f" Rounded for {raw_prediction}: {level_prediction} || Corrected Value: {corrected_value} || % Error from Actual ({actual_level}): {round(error, 2)}")

plt.legend()
plt.grid()
plt.show()

# Average delta = 0.556
# Tolerance for nominal level +- 0.278
