import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import time
import serial

# Open the serial communication
serialCom = serial.Serial('COM3', 9600)
serialCom.setDTR(False)
time.sleep(1)
serialCom.flushInput()
serialCom.setDTR(True)

# Initialize DataFrame
columns = ['LeftFootName', 'InsideLeft','OutsideLeft','HeelLeft', 'RightFootName','InsideRight','OutsideRight','HeelRight', 'DistanceName','Distance']
df = pd.DataFrame(columns=columns)

# Initialize figure and axis
fig, ax = plt.subplots()
plt.ion()  # Turn on interactive mode

def draw_feet(df):
    """Draws feet visualization with circles based on the latest dataframe values."""
    if df.empty:
        return
    
    # Get the latest row
    latest_row = df.iloc[-1]
    
    
    # Clear the figure
    ax.clear()
    ax.set_xlim(-2, 2)
    ax.set_ylim(-3, 1)
    ax.set_aspect('equal')
    ax.axis('off')
    
    # Draw feet outline
    foot_left = plt.Circle((-1, -1), 1, edgecolor='black', facecolor='none', linewidth=2)
    foot_right = plt.Circle((1, -1), 1, edgecolor='black', facecolor='none', linewidth=2)
    ax.add_patch(foot_left)
    ax.add_patch(foot_right)
    
    
    # Calculate circle RGB values
    colors = [
        (1 - float(latest_row['InsideLeft']), float(latest_row['InsideLeft']), 0),
        (1 - float(latest_row['OutsideLeft']), float(latest_row['OutsideLeft']), 0),
        (1 - float(latest_row['HeelLeft']), float(latest_row['HeelLeft']), 0),
        (1 - float(latest_row['InsideRight']), float(latest_row['InsideRight']), 0),
        (1 - float(latest_row['OutsideRight']), float(latest_row['OutsideRight']), 0),
        (1 - float(latest_row['HeelRight']), float(latest_row['HeelRight']), 0)
    ]
    
    # Circle positions relative to feet
    positions = [(-0.8, -0.4), (-1.5, -0.6), (-1, -1.5), (0.8, -0.4), (1.5, -0.6), (1, -1.5)]
    
    # Draw circles
    for i, (x, y) in enumerate(positions):
        circle = plt.Circle((x, y), 0.2, edgecolor='black', facecolor=colors[i])
        ax.add_patch(circle)
    
    plt.draw()
    plt.pause(0.1)  # Pause for animation effect

# Live update loop
while True:
    try:
        # Read the line
        s_bytes = serialCom.readline()
        decoded_bytes = s_bytes.decode("utf-8").strip('\r\n')
        values = decoded_bytes.split(' ')
        if len(values) == len(columns):
            new_row = pd.DataFrame([values], columns=columns)
            df = pd.concat([df, new_row], ignore_index=True)
            print(df['HeelRight'])
            
            # Normalize data columns
            for column in ['InsideRight','OutsideRight', 'HeelRight','InsideLeft','OutsideLeft', 'HeelLeft']:
                df[column] = pd.to_numeric(df[column], errors='coerce').fillna(0)  # Replace NaNs with 0
                min_val = df[column].min()
                max_val = df[column].max()
                if min_val != max_val:  # Avoid division by zero
                    df[column] = (df[column] - min_val) / (max_val - min_val)
                else:
                    df[column] = 0  # If all values are the same, set to 0
                    
            draw_feet(df)
    except Exception as e:
        print(f"Error: {e}")
        continue
