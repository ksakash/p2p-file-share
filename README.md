# p2p-file-share
contains code for a peer-to-peer file sharing system

`broadcast.c` : A beacon which broadcasts a UDP datagram on particular intervals.
`listener.c` : Listens to the beacon and sends a message in reply.
`tcp_client.c` : Sends a request to connect to a destination address.
`tcp_server.c` : Accepts the connection request from the client.

