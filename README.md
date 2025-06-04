# Mini Network Proxy

A simple TCP proxy which sets up a socket on the port `3001`, listens for client requests and accepts them in a non-blocking way. Creates a separate thread to answer each of these client requests with a personalised greeting of the form

`"Hello from proxy server! (Thread ID: 6094254080)`

Example output:

_Server_
```
/Users/shlokjain/CLionProjects/mini-network-proxy/cmake-build-debug/mini_network_proxy
Starting proxy on port 3001...
Non-blocking TCP server listening on port 3001
Accepted new client: FD 5
Accepted new client: FD 6
Accepted new client: FD 7
Sent greeting from thread 6094254080 to client FD 5
Accepted new client: FD 5
Sent greeting from thread 6094827520 to client FD 6
Accepted new client: FD 6
Accepted new client: FD 8
Sent greeting from thread 6096547840 to client FD 6
Accepted new client: FD 9
Sent greeting from thread 6095400960 to client FD 7
Sent greeting from thread 6095974400 to client FD 5
Sent greeting from thread 6094254080 to client FD 8
Sent greeting from thread 6094827520 to client FD 9
Accepted new client: FD 6
Accepted new client: FD 5
Accepted new client: FD 7
Sent greeting from thread 6094254080 to client FD 6
Sent greeting from thread 6094827520 to client FD 5
Sent greeting from thread 6095400960 to client FD 7
```

_Terminal_
```
shlokjain@Shloks-MacBook-Pro mini-network-proxy % cd test
shlokjain@Shloks-MacBook-Pro test % chmod +x run.sh
shlokjain@Shloks-MacBook-Pro test % ./run.sh
Starting 10 concurrent clients to test the server...
Hello from proxy server! (Thread ID: 6094254080)
Hello from proxy server! (Thread ID: 6094827520)
Hello from proxy server! (Thread ID: 6094254080)
Hello from proxy server! (Thread ID: 6094827520)
Hello from proxy server! (Thread ID: 6096547840)
Hello from proxy server! (Thread ID: 6095974400)
Hello from proxy server! (Thread ID: 6095400960)
Hello from proxy server! (Thread ID: 6095400960)
Hello from proxy server! (Thread ID: 6094254080)
Hello from proxy server! (Thread ID: 6094827520)
All clients finished.
```
