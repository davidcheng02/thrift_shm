import matplotlib as mpl
import matplotlib.pyplot as plt
import statistics
import subprocess

from pdf2image import convert_from_path

mpl.rcParams['font.family'] = 'serif'
mpl.rcParams['font.serif'] = ['Computer Modern']
mpl.rcParams['text.usetex'] = True

#subprocess.call(['sh', './run_shm_buf_tests.sh'])
NUM_RUNS = 10

shm_throughput = []
shm_latency = []
shm_time = []

shm_ns_throughput = []
shm_ns_latency = []
shm_ns_time = []

http_throughput = []
http_latency = []
http_time = []

with open("output.txt", "r") as file:
    lines = file.readlines()

    data_processed = 0
    for line in lines:
        line_split = line.split()

        if (len(line_split) > 0):
            if line_split[0] == "Throughput:":
                throughput = float(line_split[1])

                if data_processed < NUM_RUNS:
                    shm_throughput.append(throughput)
                elif data_processed < 2*NUM_RUNS:
                    shm_ns_throughput.append(throughput)
                else:
                    http_throughput.append(throughput)
            elif line_split[0] == "Latency:":
                latency = float(line_split[1])

                if data_processed < NUM_RUNS:
                    shm_latency.append(latency)
                elif data_processed < 2*NUM_RUNS:
                    shm_ns_latency.append(latency)
                else:
                    http_latency.append(latency)
            elif line_split[0] == "Time:":
                time = float(line_split[1])

                if data_processed < NUM_RUNS:
                    shm_time.append(time)
                elif data_processed < 2*NUM_RUNS:
                    shm_ns_time.append(time)
                else:
                    http_time.append(time)

                data_processed += 1

print(f"Http throughput avg: {statistics.fmean(http_throughput)} req/s")
print(f"Shm throughput avg: {statistics.fmean(shm_throughput)} req/s")
print(f"Shm no-serialization throughput avg: {statistics.fmean(shm_ns_throughput)} req/s")
print()

print(f"Http throughput std: {statistics.stdev(http_throughput)}")
print(f"Shm throughput std: {statistics.stdev(shm_throughput)}")
print(f"Shm no-serialization throughput std: {statistics.stdev(shm_ns_throughput)}")
print()

print(f"Http latency avg: {statistics.fmean(http_latency)} us")
print(f"Shm latency avg: {statistics.fmean(shm_latency)} us")
print(f"Shm no-serialization latency avg: {statistics.fmean(shm_ns_latency)} us")
print()

print(f"Http time avg: {statistics.fmean(http_time)} s")
print(f"Shm time avg: {statistics.fmean(shm_time)} s")
print(f"Shm no-serialization time avg: {statistics.fmean(shm_ns_time)} s")
print()

# UNCOMMENT FOR GRAPH GENERATION. REQUIRES LATEX TO BE INSTALLED
#fig, ax = plt.subplots()
#types = ["HTTP/2", "shm", "shm (no serialization)"]
#counts = [statistics.fmean(http_throughput),
#          statistics.fmean(shm_throughput),
#          statistics.fmean(shm_ns_throughput)]
#stds = [statistics.stdev(http_throughput),
#        statistics.stdev(shm_throughput),
#        statistics.stdev(shm_ns_throughput)]
#bar_colors = ['tab:red', 'tab:blue', 'tab:orange']
#
#ax.bar(types,
#       counts,
#       yerr=stds,
#       color=bar_colors,
#       align="center",
#       alpha=0.5,
#       ecolor="black",
#       capsize=10)
#
#ax.set_xlabel("Transport Protocol")
#ax.set_ylabel("Throughput (req/s)")
#ax.set_title("Shared Memory Buffer Throughput Comparison")
#
#plt.savefig("throughputs.pdf")
#convert_from_path("throughputs.pdf",
#                  dpi=1200,
#                  output_folder=".",
#                  fmt="png",
#                  output_file="throughputs")
#plt.show()
