import matplotlib.pyplot as plt
import os

currentDirPath = os.getcwd()
parentDirPath = parent_directory = os.path.join(currentDirPath, '..')
logDirPath = os.path.join(parent_directory, 'arduino-fdc1004/logs')

files = os.listdir(logDirPath)

if len(files) == 0:
    print("There are no files in the target folder!")
    exit(0)

file_paths = [os.path.join(logDirPath, file) for file in files if os.path.isfile(os.path.join(logDirPath, file))]

# Sort the file paths by modification time in descending order
file_paths.sort(key=lambda x: os.path.getmtime(x), reverse=True)

most_recent_log_file = file_paths[0]
print(most_recent_log_file)

file = open(most_recent_log_file, "r")

r_values = []
l_values = []
e_values = []

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
        case _:
            continue

plt.figure()  # Create a new figure

x = [i for i in range(len(r_values))]

plt.plot(x, r_values, label='Reference')  # Plot data set 1
plt.plot(x, l_values, label='Level')  # Plot data set 2
plt.plot(x, e_values, label='Environment')  # Plot data set 3

plt.xlabel('X')  # Set X-axis label
plt.ylabel('Y')  # Set Y-axis label
plt.title('Capacitive Level Output')  # Set plot title

plt.legend()  # Show legend

plt.grid(True)  # Show grid lines

plt.show()  # Show the plot
