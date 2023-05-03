# Peer_Peeer_Communication_Broadcasting
The folder contains two files 
server_final.cpp
client_final.cpp
Open Terminal and run,
<ul>
<li> g++ server_final.cpp -o server_final -pthread -lssl -lcrypto</li>
<li>g++ client_final.cpp -o client_final -pthread</li>
<li>./server_final</li>
<li> ./client_final </li></ul>

<ol><li>Login/signup</li>
<li>Choose from options displayed.</li>
 <ol> <li> The server will send data of all the clients connected</li>
   <li> You can broadcast a message to all clients connected</li>
   <li> To send a message to a peer enter a message as “msg_YOURMESSAGE_ADDRESSOFCLIENT”.(you can see address first by choosing first option)</li></ol></ol>
<h2>Functionalities</h2>
<ol>
<li>Handling Multiple clients using multi threading - working</li>
<li>Get active clients - working</li>
<li>Broadcast a message to all clients - working</li>
<li>SHA-256 Hashing - working</li>
<li>MAX clients that can be connected to server(50) - working</li>
<li>Checking Client Activity status i.e if client is inactive for 30 mins - working</li>
<li>Login - working</li>
<li>Signup - working</li>
<li>Peer-Peer communication.-working</li></ol>
<h1>Over View of the Project </h1>
<ol>
<li>A function has been implemented to accept incoming connections from multiple clients and here is where the base of the work builds.
Once a client has been connected a new thread has been created and detached to handle that particular client. And the client will be entered into a while loop which breaks when the client successfully logged in.</li>
<li>The all clients vector length will be checked every time a new client requests to join the server. If the server reaches the MAX_Clients it will reject the connection.</li>
<ol>
<li>Login and</li>
<li>Signup : -</li></ol> 
<li>Once the thread is detached, a message will be sent to the client from the server asking to choose Login or SignUp.
If the client choses login, there will be a sequence of messages from the server asking for username and password.A 256 char sequence will be generated using SHA256 for both client entered password and password in csv file and verify the password. If we found the match then the client will be authenticated and will access the further features like</li> 
<ol><li> Active clients</li>
<li>Broadcast</li>
<li>Peer-peer communication</li></ol>
<ul><li>If a client chooses to sign up,the client will be asked to enter a new username and password.and redirected back to login/signup prompt.
Vectors and maps created to handle further functionalities.</li>
<ul><li>A Vector all_clients has been created to store Ip and port numbers of all the clients connected</li>
<li>A Map client_map has been created to map each client socket to their respective IP and Port.</li>
<li>A Map client_time to map each client's socket to their joining time.</li>
<li>A Map clients_Nmsg to map client socket to number of messages sent and it will incremented whenever the client reaches the server.</li></ul></ul>
<li>Once The user is logged in, they will have options like </li>
<ol><li>Active clients </li>
<li>Broadcast</li>
<li>Peer-peer communication</li>

<ol><li>Active Clients :-
Once the user requested active clients, the server will send all_clients vectors to the requested client.</li>
<li>Broadcast : - 
If a user wants to broadcast a message, the client will ask them to enter the message to broadcast and creates a UDP Socket and broadcasts the message to all the clients in the all_clients vector.</li>
<li>Peer-peer communication :- 
Once the user selects the peer to peer communication the server will ask the user to enter the message in the format msg_yourMessage_Address. And once the server receives the message it will split the message by ‘_’ and check the first segments, if first segment is msg then it will send the message to client in the format ‘peer_destinationAddress’ to requested client and once a client receive the message starting with segment ‘peer’ it will creates a remote TCP Socket directly to the destination client address and exchange the messages.</li></ol>
<li>While the server is in an infinite loop waiting for messages from clients, we'll check the first condition that duration of each client from their connected time, if it is more than or equal to 30 mins, then we'll check for second condition that the number of messages sent in the last 30mins by that client. If it is less than 2, then the client will be notified that “Inactive for too long, terminating connection”. And for all the users who entered into the first condition, connected time will be changed to current time and so forth….</li></ol>
