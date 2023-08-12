import numpy as np
import matplotlib.pyplot as plt

x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
# y_original = np.array([0, 2.31, 11, 18.46, 24.4, 30.395, 37.635, 43.08, 48.455, 54.585, 61.655])  # Original data points

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

gradients_original = np.diff(y_original) / np.diff(x)
print(gradients_original)

for i in range(len(gradients_original)):
    gain = 1 / gradients_original[i]
    offset = - y_original[i] + i + 1
    section_params = (gain, offset)
    print(section_params)

y_corrected = gain * x + offset
# print(y_corrected)

# plt.figure()
# plt.plot(x, y_original, 'o', label='Original Data')
# # plt.plot(x, y_corrected, 'x', label='Corrected Data')
# plt.axhline(y=desired_gradient * x[0] + offset, color='r', linestyle='-', label='Desired Line')
# plt.legend()
# plt.xlabel('x')
# plt.ylabel('y')
# plt.title('Raw Input')
# plt.show()

# print(f"Gain: {gain}")
# print(f"Offset: {offset}")
