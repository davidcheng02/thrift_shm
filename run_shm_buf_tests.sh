make

echo "Shm Results" > output.txt
echo "====" >> output.txt
for i in {1..10}
do
    echo "Run $i" >> output.txt
    ./server &
    ./client >> output.txt &
    wait
    echo >> output.txt
done

echo "Shm No Serialization Results" >> output.txt
echo "====" >> output.txt
for i in {1..10}
do
    echo "Run $i" >> output.txt
    ./server --no-serialization &
    ./client --no-serialization >> output.txt &
    wait
    echo >> output.txt
done

echo "HTTP Results" >> output.txt
echo "====" >> output.txt
for i in {1..10}
do
    echo "Run $i" >> output.txt
    ./http_server &
    ./http_client >> output.txt
    # server will listen indefinitely, so need to manually kill it
    ps -ef | grep "./http_server" | grep -v grep | awk '{print $2}' | xargs kill
    wait
    echo >> output.txt
done
