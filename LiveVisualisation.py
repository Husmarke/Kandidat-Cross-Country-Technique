"""
# ------------------- setup -------------------
# import the required libraries
import serial
import time
import tkinter as tk

# Open the serial communication
serialCom = serial.Serial('COM10', 115200) # change baudrate and port as needed
serialCom.setDTR(False)
time.sleep(0.5)
serialCom.flushInput()
serialCom.setDTR(True)

# initialize the min, max and loadcell values
max_values = [0.0,0.0,0.0,0.0,0.0,0.0]
min_values = [0.0,0.0,0.0,0.0,0.0,0.0]
loadcell_values = [0.0,0.0,0.0,0.0,0.0,0.0]

# find the ratio of the value to the min and max values
def get_ratio(value, min_val, max_val):
    if max_val == min_val:
        return 0.5  # avoid div by zero
    return (value - min_val) / (max_val - min_val)

# ------------------- visualisation -------------------
# Create a tkinter window to visualize the data
class Visualizer:
    def __init__(self, root):
        self.canvas = tk.Canvas(root, width=700, height=400, bg="white")
        self.canvas.pack()
        self.update_visual()

    def draw_circle_group(self, x, y, radius, group_indices):
        # Draw outer circle
        self.canvas.create_oval(x - radius, y - radius, x + radius, y + radius, outline="black", width=2)

        # Position map: index 1 = top-left, index 2 = top-right, index 3 = bottom
        positions = [
            (x - radius * 0.5, y - radius * 0.5),  # index 1 (top-left)
            (x + radius * 0.5, y - radius * 0.5),  # index 2 (top-right)
            (x, y + radius * 0.4)                  # index 3 (bottom-center)
        ]

        for pos_idx, value_idx in enumerate(group_indices):
            ratio = get_ratio(loadcell_values[int(value_idx)], min_values[int(value_idx)], max_values[int(value_idx)])
            r = int(20 + ratio * 20)
            cx, cy = positions[pos_idx]
            self.canvas.create_oval(cx - r, cy - r, cx + r, cy + r, fill="blue", outline="")

    def update_visual(self):
        self.canvas.delete("all")

        # Divide 6 values into 2 groups of 3
        for i in range(2):
            group_ratios = [
                get_ratio(loadcell_values[j], min_values[j], max_values[j]) for j in range(i*3, (i+1)*3)
            ]
            self.draw_circle_group(170 + i*350, 200, 150, group_ratios)

        # Call this method again after 100 ms (0.1 sec)
        root.after(100, self.update_visual)

# Example from your data-fetching logic
def update_data(new_values):
    global loadcell_values,min_values, max_values
    for i in range(6):
        loadcell_values[i] = new_values[i]
        min_values[i] = min(min_values[i], new_values[i])
        max_values[i] = max(max_values[i], new_values[i])

root = tk.Tk()
app = Visualizer(root)
root.mainloop()

while True:
    s_bytes = serialCom.readline()
    # decode the bytes to string and strip the new line characters
    decoded_bytes = s_bytes.decode("utf-8").strip('\r\n')
    # split the string into a list of values
    all_values = decoded_bytes.split(' ')
    # store the loadcell values in a list
    loadcell_values = [all_values[2],all_values[3],all_values[4],all_values[6],all_values[7],all_values[8]]
    update_data(loadcell_values)

    #print("Loadcell values: ", loadcell_values)
    #print("Max values: ", max_values)
    #print("Min values: ", min_values)
"""

# ------------------- setup -------------------
import serial
import time
import tkinter as tk
import threading

# Open the serial communication
serialCom = serial.Serial('COM10', 115200)  # change port and baudrate as needed
serialCom.setDTR(False)
time.sleep(0.5)
serialCom.flushInput()
serialCom.setDTR(True)

# Initialize the min, max, and loadcell values
max_values = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
min_values = [999999.0, 999999.0, 999999.0, 999999.0, 999999.0, 999999.0]
loadcell_values = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

# Function to get ratio
def get_ratio(value, min_val, max_val):
    if max_val == min_val:
        return 0.5  # Avoid divide-by-zero
    return (value - min_val) / (max_val - min_val)

# ------------------- visualisation -------------------
class Visualizer:
    def __init__(self, root):
        self.canvas = tk.Canvas(root, width=700, height=400, bg="white")
        self.canvas.pack()
        self.update_visual()

    def draw_circle_group(self, x, y, radius, group_indices):
        # Draw outer circle
        self.canvas.create_oval(x - radius, y - radius, x + radius, y + radius, outline="black", width=2)

        # Triangle layout: index 0 = top-left, 1 = top-right, 2 = bottom-center
        positions = [
            (x - radius * 0.5, y - radius * 0.5),  # top-left
            (x + radius * 0.5, y - radius * 0.5),  # top-right
            (x, y + radius * 0.4)                  # bottom-center
        ]

        for pos_idx, value_idx in enumerate(group_indices):
            value = loadcell_values[value_idx]
            min_val = min_values[value_idx]
            max_val = max_values[value_idx]
            ratio = get_ratio(value, min_val, max_val)

            r = int(30)  # Circle size from 20 to 40
            cx, cy = positions[pos_idx]

            # Color from red to green
            red = int(255 * (1 - ratio))
            green = int(255 * ratio)
            color = f'#{red:02x}{green:02x}00'

            self.canvas.create_oval(cx - r, cy - r, cx + r, cy + r, fill=color, outline="")

    def update_visual(self):
        self.canvas.delete("all")

        # Draw two groups (each with 3 values)
        for i in range(2):
            group_indices = [i * 3, i * 3 + 1, i * 3 + 2]
            self.draw_circle_group(170 + i * 350, 200, 150, group_indices)

        root.after(10, self.update_visual)  # Refresh every 10ms

# Update values from new data
def update_data(new_values):
    global loadcell_values, min_values, max_values
    for i in range(6):
        value = new_values[i]
        loadcell_values[i] = value
        min_values[i] = min(min_values[i], value)
        max_values[i] = max(max_values[i], value)

# Background thread to read serial data
def read_serial_loop():
    global loadcell_values
    while True:
        try:
            s_bytes = serialCom.readline()
            decoded = s_bytes.decode("utf-8").strip()
            all_values = decoded.split(' ')

            if len(all_values) >= 9:
                new_vals = [
                    float(all_values[2]), float(all_values[3]), float(all_values[4]),
                    float(all_values[6]), float(all_values[7]), float(all_values[8])
                ]
                update_data(new_vals)
        except Exception as e:
            print("Error reading serial:", e)

# Launch GUI
root = tk.Tk()
root.title("Loadcell Visualizer")
app = Visualizer(root)

# Start serial reading in background
threading.Thread(target=read_serial_loop, daemon=True).start()

# Start GUI loop
root.mainloop()



