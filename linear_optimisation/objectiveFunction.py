import numpy as np
from scipy.optimize import minimize
import matplotlib.pyplot as plt

# Sample data
x = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
y_original = np.array([0, 2.31, 11, 18.46, 24.4, 30.395, 37.635, 43.08, 48.455, 54.585, 61.655])  # Original data points

# Desired gradient (you can modify this as needed)
desired_gradient = 1

# Define the objective function to minimize
def objective_function(params):
    gain, offset = params
    adjusted_gradients = (np.diff(y_original) / np.diff(x)) * gain + offset
    error = np.sum((adjusted_gradients - desired_gradient)**2)
    return error

# Initial guess for gain and offset
initial_guess = [1.0, 0.0]

# Minimize the objective function
result = minimize(objective_function, initial_guess, method='Nelder-Mead')

# Obtain the optimized gain and offset
optimized_gain, optimized_offset = result.x

# Calculate adjusted data using optimized gain and offset
y_adjusted = (np.diff(y_original) / np.diff(x)) * optimized_gain + optimized_offset

# Plot the original and adjusted data
plt.figure()
plt.plot(x[:-1], y_original[:-1], 'o', label='Original Data')
plt.plot(x[:-1], y_adjusted, 'x', label='Adjusted Data')
plt.axhline(y=desired_gradient, color='r', linestyle='-', label='Desired Gradient')
plt.legend()
plt.xlabel('x')
plt.ylabel('y')
plt.title('Data Adjustment to Desired Gradient')
plt.show()

# Print the optimized gain and offset
print("Optimized Gain:", optimized_gain)
print("Optimized Offset:", optimized_offset)
