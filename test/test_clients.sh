# How many simultaneous clients to test
NUM_CLIENTS=10

echo "Starting $NUM_CLIENTS concurrent clients to test the server..."

for i in $(seq 1 $NUM_CLIENTS); do
  (
    echo "Client $i" | nc localhost 3001
  ) &
done

# Wait for all background jobs to finish
wait

echo "All clients finished."