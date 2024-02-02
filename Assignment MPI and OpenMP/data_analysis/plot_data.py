import os
import re
import matplotlib.pyplot as plt
import pandas as pd


# Define the directory path here
directory_path = "data"

# Regex pattern to match the files
file_pattern = r"gol_(beehive|grower|glider)_(\d+)\.out"

# Data storage
data = {
    "beehive": [],
    "grower": [],
    "glider": []
}

# Iterate through the files in the directory
for filename in os.listdir(directory_path):
    if re.match(file_pattern, filename):
        with open(os.path.join(directory_path, filename), 'r') as file:
            content = file.read()
            
            # Extract the time taken
            time_taken = re.search(r"It took (\d+\.\d+) seconds", content)

            if time_taken:
                group = re.match(file_pattern, filename).group(1)
                time = float(time_taken.group(1))
                cores = int(re.match(file_pattern, filename).group(2))

                # Append data
                data[group].append((cores, time))


# Convert the lists into DataFrames
df_beehive = pd.DataFrame(data['beehive'], columns=['Cores', 'Time'])
df_grower = pd.DataFrame(data['grower'], columns=['Cores', 'Time'])
df_glider = pd.DataFrame(data['glider'], columns=['Cores', 'Time'])

# Sort data by 'Cores'
df_beehive.sort_values(by='Cores', inplace=True)
df_grower.sort_values(by='Cores', inplace=True)
df_glider.sort_values(by='Cores', inplace=True)

# Function to calculate speedup
def calculate_speedup(df):
    # Find the time for single-core processing (Cores = 1)
    single_core_time = df[df['Cores'] == 1]['Time'].values[0]

    # Calculate speedup: single_core_time / multi-core_time
    df['Speedup'] = single_core_time / df['Time']
    return df

# Apply the speedup calculation to each DataFrame
df_beehive = calculate_speedup(df_beehive)
df_grower = calculate_speedup(df_grower)
df_glider = calculate_speedup(df_glider)

# Updated plotting function for speedup, with saving feature
def plot_and_save_speedup(df, title, filename):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Cores'], df['Speedup'], marker='o')
    plt.title(title)
    plt.xlabel('Cores of Cores')
    plt.ylabel('Speedup')
    plt.grid(True)
    plt.savefig(f"{filename}_speedup.png")
    plt.show()

# Updated plotting function for normal plot, with saving feature
def plot_and_save_normal(df, title, filename):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Cores'], df['Time'], marker='o')
    plt.title(title)
    plt.xlabel('Cores from Filename')
    plt.ylabel('Time Taken (seconds)')
    plt.grid(True)
    plt.savefig(f"{filename}_time.png")
    plt.show()

# Plot and save speedup and normal plots for each group
plot_and_save_speedup(df_beehive, 'Beehive Speedup Curve', 'beehive')
plot_and_save_normal(df_beehive, 'Beehive Time Analysis', 'beehive')
plot_and_save_speedup(df_grower, 'Grower Speedup Curve', 'grower')
plot_and_save_normal(df_grower, 'Grower Time Analysis', 'grower')
plot_and_save_speedup(df_glider, 'Glider Speedup Curve', 'glider')
plot_and_save_normal(df_glider, 'Glider Time Analysis', 'glider')
