make shm_lib

echo "Shm Results" > shm_lib_output.txt
echo "====" >> shm_lib_output.txt
for i in {1..10}
do
    echo "Run $i" >> shm_lib_output.txt
    ./shm_lib_server &
    ./shm_lib_client >> shm_lib_output.txt &
    wait
    echo >> shm_lib_output.txt
done

echo "Shm Non-serialization Results" >> shm_lib_output.txt
echo "====" >> shm_lib_output.txt
for i in {1..10}
do
    echo "Run $i" >> shm_lib_output.txt
    ./shm_lib_server --no-serialization &
    ./shm_lib_client --no-serialization >> shm_lib_output.txt &
    wait
    echo >> shm_lib_output.txt
done
