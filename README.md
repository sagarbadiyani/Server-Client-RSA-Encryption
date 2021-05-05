#Compilation
- Server (Same command for both Mac and Ubuntu): `gcc -o server server.c`
- Client (Mac): `gcc client.c -o client -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/`
- Client (Ubuntu): `gcc client.c -o client -lssl -lcrypto`

#Run
- Server: `./server 8080`
- Client1: `./client private1.pem public2.pem 127.0.0.1 8080`
- Client2: `./client private2.pem public1.pem 127.0.0.1 8080`

#Sample Run
- As soon as you connect both the clients to the server, the server will display the message that both the clients have connected.
- In client 1 terminal, enter "Hello, Client 1 here".
- You will see its cipher and its decrypted text in terminal 2.
- Similarly, do this for client 2 "Hello, Client 2 here".
- You will see its cipher and its decrypted text in terminal 1.
- Write "exit" and hit enter in any one of the two terminals, you will see both the clients exit.
- The server will now be ready for accepting two more clients.