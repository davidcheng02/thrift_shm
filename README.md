# thrift_shm
## Initialization
Run `thrift -r --gen cpp echo.thrift` to generate the corresponding Thrift
files before building any of the programs.
## Benchmarks
### Shared memory buffer with semaphores
Run `sh run_shm_buf_tests.sh` to generate
`output.txt`. Then run `python3 run_shm_buf_tests.py` to parse `output.txt` and
obtain averages of throughput, latency, and total time, along with graphs.

### Shared memory RPC library
Run `sh run_shm_lib_tests.sh` to generate `shm_lib_output.txt`. Then run
`python3 run_shm_lib_tests.py` to parse `shm_lib_output.txt` and obtain
averages of throughput, latency, and total time, along with graphs.
