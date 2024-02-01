import pandas as pd
import matplotlib.pyplot as plt
import re
import os

# Directory containing output files
directory = 'path_to_your_files'  # Replace with your directory path

# Initialize a DataFrame to store data
data = {'Cores': [], 'ElapsedTime': []}

# Regular expression to find the elapsed time
time_pattern = re.compile(r'Total time: (\d+\.\d+)')

# Read each file and extract data
for filename in os.listdir(directory):
    if filename.startswith('output') and filename.endswith('.txt'):
        # Extract the core number from the file name
        core_number = int(filename.replace('output', '').replace('.txt', ''))
        
        # Read the file
        with open(os.path.join(directory, filename), 'r') as file:
            contents = file.read()
            # Search for the elapsed time
            match = time_pattern.search(contents)
            if match:
                elapsed_time = float(match.group(1))
                # Add data to the DataFrame
                data['Cores'].append(core_number)
                data['ElapsedTime'].append(elapsed_time)

# Convert to DataFrame
df = pd.DataFrame(data)

# Sort the DataFrame based on the number of cores
df = df.sort_values(by='Cores')

# This is the first part, we will continue with plotting next.
# Plotting
plt.figure(figsize=(10, 6))
plt.plot(df['Cores'], df['ElapsedTime'], marker='o', linestyle='-')
plt.title('Elapsed Time vs Number of Cores')
plt.xlabel('Number of Cores')
plt.ylabel('Elapsed Time (seconds)')
plt.grid(True)
plt.show()
