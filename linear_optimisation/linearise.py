import numpy as np
import matplotlib.pyplot as plt

# Example data
x = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8])
y_original = np.array([6.8, 
7.9,
8.8,
9.7,
10.4,
11.1,
11.7,
12.2,
12.8,
13.3])

# Apply logarithmic transformation to y-values
y_linearized = np.log(y_original)

# Calculate gain and offset for each data point
gains = np.diff(y_linearized) / np.diff(x)
# offsets = y_linearized - gains * x

# Calculate linearized y-values using gain and offset for each data point
# y_linearized_predicted = gains * x + offsets
print(y_linearized)

# # Perform linear regression on the transformed data
# slope, intercept = np.polyfit(x, y_linearized, 1)

# # Calculate the linearized y-values using the linear regression equation
# y_linearized_predicted = slope * x + intercept

# Plot the original and linearized data
plt.figure()
plt.plot(x, y_original, 'o', label='Original Data')
# plt.plot(x, y_linearized, 'x', label='Linearized Data')
plt.xlabel('x')
plt.ylabel('y')

gradients_original = np.diff(y_linearized) / np.diff(x)
print(gradients_original)

desired_gradient = 1
gain = desired_gradient / np.mean(gradients_original)
offset = np.mean(y_linearized) - np.mean(gradients_original) * np.mean(x)

y_corrected = gain * x + offset
# print(y_corrected)

# plt.plot(x, y_corrected, 'x', label='Corrected Data')
plt.axhline(y=desired_gradient * x[0] + offset, color='r', linestyle='-', label='Desired Line')
plt.title('Linearising & Linear Data Correction')
plt.legend()
plt.show()

print(f"Gain: {gain}")
print(f"Offset: {offset}")
