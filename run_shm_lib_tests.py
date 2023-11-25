import matplotlib as mpl
import matplotlib.pyplot as plt
import statistics
import subprocess

from pdf2image import convert_from_path

mpl.rcParams['font.family'] = 'serif'
mpl.rcParams['font.serif'] = ['Computer Modern']
mpl.rcParams['text.usetex'] = True

#subprocess.call(['sh', './run_shm_lib_tests.sh'])
NUM_RUNS = 10


# shm, shm_ns
throughputs = [[], []]
latencies = [[], []]
times = [[], []]

with open("shm_lib_output.txt", "r") as file:
    lines = file.readlines()

    data_processed = 0
    for line in lines:
        line_split = line.split()

        if (len(line_split) > 0):
            if line_split[0] == "Throughput:":
                throughput = float(line_split[1])

                if data_processed < NUM_RUNS:
                    throughputs[0].append(throughput)
                else:
                    throughputs[1].append(throughput)

            elif line_split[0] == "Latency:":
                latency = float(line_split[1])

                if data_processed < NUM_RUNS:
                    latencies[0].append(latency)
                else:
                    latencies[1].append(latency)
            elif line_split[0] == "Time:":
                time = float(line_split[1])

                if data_processed < NUM_RUNS:
                    times[0].append(time)
                else:
                    times[1].append(time)

                data_processed += 1

throughput_means = [statistics.fmean(tput) for tput in throughputs]
throughput_stds = [statistics.stdev(tput) for tput in throughputs]

latency_means = [statistics.fmean(l) for l in latencies]
time_means = [statistics.fmean(t) for t in times]

print(f"Shm throughput avg: {throughput_means[0]} req/s")
print(f"Shm ns throughput avg: {throughput_means[1]} req/s")
print()

print(f"Shm throughput std: {throughput_stds[0]}")
print(f"Shm ns throughput std: {throughput_stds[1]}")
print()

print(f"Shm latency avg: {latency_means[0]} us")
print(f"Shm ns latency avg: {latency_means[1]} us")
print()

print(f"Shm time avg: {time_means[0]} s")
print(f"Shm ns time avg: {time_means[1]} s")
print()

fig, ax = plt.subplots()
types = ["Shm", "Shm (no serialization)"]
bar_colors = ['tab:red', 'tab:blue']

ax.bar(types,
       throughput_means,
       yerr=throughput_stds,
       color=bar_colors,
       align="center",
       alpha=0.5,
       ecolor="black",
       capsize=10)

ax.set_xlabel("Transport Protocol")
ax.set_ylabel("Throughput (req/s)")
ax.set_title("Shm Library Throughput Comparison")

plt.savefig("shm_lib_throughputs.pdf")
convert_from_path("shm_lib_throughputs.pdf",
                  dpi=1200,
                  output_folder=".",
                  fmt="png",
                  output_file="shm_lib_throughputs")
plt.show()
