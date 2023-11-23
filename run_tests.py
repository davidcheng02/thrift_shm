import statistics
import subprocess

#subprocess.call(['sh', './run_tests.sh'])
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

#print(shm_throughput)
#print(http_throughput)

print(f"Shm throughput avg: {statistics.fmean(shm_throughput)} req/s")
print(f"Shm no-serialization throughput avg: {statistics.fmean(shm_ns_throughput)} req/s")
print(f"Http throughput avg: {statistics.fmean(http_throughput)} req/s")
print()

print(f"Shm throughput std: {statistics.stdev(shm_throughput)}")
print(f"Shm no-serialization throughput std: {statistics.stdev(shm_ns_throughput)}")
print(f"Http throughput std: {statistics.stdev(http_throughput)}")
print()

#print(shm_latency)
#print(http_latency)

print(f"Shm latency avg: {statistics.fmean(shm_latency)} us")
print(f"Shm no-serialization latency avg: {statistics.fmean(shm_ns_latency)} us")
print(f"Http latency avg: {statistics.fmean(http_latency)} us")
print()

#print(shm_time)
#print(http_time)

print(f"Shm time avg: {statistics.fmean(shm_time)} s")
print(f"Shm no-serialization time avg: {statistics.fmean(shm_ns_time)} req/s")
print(f"Http time avg: {statistics.fmean(http_time)} s")
print()
