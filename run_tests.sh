make

echo "Shm Results" >> output.txt
echo "====" >> output.txt
for i in {1..10}
do
    echo "Run $i" >> output.txt
    ./server &
    ./client >> output.txt &
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
    ps -ef | grep "./http_server" | grep -v grep | awk '{print $2}' | xargs kill
    wait
    echo >> output.txt
done
