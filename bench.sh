for i in $(seq 1 1000); do
    curl -X POST 127.0.0.1:8080/time &
    curl -X POST 127.0.0.1:8080/time/stop &
    sleep 0.1
done
