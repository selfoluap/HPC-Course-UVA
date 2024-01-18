import matplotlib.pyplot as plt
import re

thread_pattern = r"CPUS:\s+(\d+)"
nitter_pattern = r"DONE with (\d+)"
time_pattern = r"obtained in (\d+\.\d+) seconds"

# Read the content from pi.out
with open('pi.out', 'r') as file:
    text = file.read()
    threads = re.findall(thread_pattern, text)
    times = re.findall(time_pattern, text)
    times = [float(time) for time in times]

print(threads)
print(times)

fig, ax = plt.subplots()
ax.plot(threads, times)
ax.set_xlabel('Number of Threads')
ax.set_ylabel('Elapsed Time to Estimate Pi')
ax.set_title('Graph of Elapsed Time vs Number of Threads')
ax.grid(True)
fig.savefig("pi.png")

with open('pi-2.out', 'r') as file:
    text = file.read()
    nitters = re.findall(nitter_pattern, text)
    times = re.findall(time_pattern, text)
    times = [float(time) for time in times]

fig, ax = plt.subplots()
ax.plot(nitters, times)
plt.xticks(fontsize=8)
ax.set_xlabel('Number of Iterations')
ax.set_ylabel('Elapsed Time to Estimate Pi')
ax.set_title('Graph of Elapsed Time vs Number of Iterations')
ax.grid(True)
fig.savefig("pi-2.png")
