#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <ctime>
#include <netdb.h>
#include <map>
#include <thread>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <vector>
#include <openssl/sha.h>

#define BUFFER_SIZE 1024
#define MAX_CONN 50
std::vector<std::thread> threads;
std::vector<int> Num_clients;
std::vector<std::string> all_clients;
//std::vector<std::string> client_time;
//std::vector<std::string> client_Nmsgs;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
std::map<std::string, int> client_map;
std::map<int, std::chrono::time_point<std::chrono::system_clock>> client_time;
std::map<int, int> client_Nmsgs;
int udp_socket;
void client_handler(int client_socket);
int IPL = 0;
int count = 0;
using namespace std;
void join_client(int sock_tcp) {
    while (true) {
    if(count<MAX_CONN){
        int client_socket, client_addr_len;
        struct sockaddr_in client_address;
        client_addr_len = sizeof(client_address);
        if ((client_socket = accept(sock_tcp, (struct sockaddr*)&client_address, (socklen_t*)&client_addr_len)) < 0) {
            continue;
        }
        std::thread client_thread(client_handler, client_socket);
        client_thread.detach();
        while(true){
        std::string response = "Please choose an option: login or signup\n";
    	send(client_socket, response.c_str(), response.length(), 0);
    	char buffer[1024];
    	int bytes_received = recv(client_socket, buffer, 1024, 0);    
    	std::string request(buffer, bytes_received);
        std::string selection(buffer, bytes_received);
    	if (selection == "login") {
    	    bool is_loggedin = false;
    	    std::string line;
            response = "Username:\n";
            send(client_socket, response.c_str(), response.length(), 0);
            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string username(buffer, bytes_received);
            response = "Password:\n";
            send(client_socket, response.c_str(), response.length(), 0);
            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string password(buffer, bytes_received);
            unsigned char hash[SHA256_DIGEST_LENGTH];
    	    SHA256((const unsigned char*)password.c_str(), password.length(), hash);
    	    std::cout << hash << " ";
            std::ifstream file("users.csv");
            while (std::getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                    std::string fusername = line.substr(0, pos);
                    std::string fpassword = line.substr(pos+1);
                    unsigned char hash2[SHA256_DIGEST_LENGTH];
    	            SHA256((const unsigned char*)fpassword.c_str(), fpassword.length(), hash2);
    	            std::cout << hash2 << " ";
                    if (fusername == username && fpassword == password) {
                        is_loggedin = true;
                        break;
                    }
                }
            }
            file.close();
            if (is_loggedin) {
                response = "Login successful!\n";
                send(client_socket, response.c_str(), response.length(), 0);
                IPL++;
                auto join_time = std::chrono::system_clock::now();
                client_time.insert(std::make_pair(client_socket,join_time));
                client_Nmsgs.insert(std::make_pair(client_socket,1));    
		response = "Please enter anything and press enter to continue : \n";
		send(client_socket, response.c_str(), response.length(), 0);
		uint16_t port = ntohs(client_address.sin_port);
		std::string Iport = std::to_string(port);
		all_clients.push_back("Client "+ std::to_string(IPL+1) + " : " + inet_ntoa(client_address.sin_addr) + ":" +Iport );
		std::string Ip = inet_ntoa(client_address.sin_addr);
		std::string final_address = Ip + ":" + Iport;
		client_map.insert(std::make_pair(final_address, client_socket));
		//for (int ppl = 0; ppl < all_clients.size(); ppl++) {
		  //  std::cout << all_clients[ppl] << " ";
		//}
		Num_clients.push_back(client_socket);
		count=count+1;
		break;

}else{
      response = "invalid credentials. \n";
      send(client_socket, response.c_str(), response.length(), 0);
}}
            else if (selection == "signup") {
            response = "Please enter a new username:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string new_username(buffer, bytes_received);
            std::ifstream file("users.csv");
            std::string line;
            bool username_exists = false;
            while (std::getline(file, line)) {
                size_t pos = line.find(',');
                if (pos != std::string::npos) {
                std::string username_from_file = line.substr(0, pos);
                if (new_username == username_from_file) {
                username_exists = true;
                    break;
                    }
                    }
                        }
                file.close();

                 if (username_exists) {
            response = "username not available.Enter different username.\n";
            send(client_socket, response.c_str(), response.length(), 0);
        } else {
            response = "Please enter a new password:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string new_password(buffer, bytes_received);

            response = "Re-enter your password:\n";
            send(client_socket, response.c_str(), response.length(), 0);

            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string fpassword(buffer, bytes_received);

            if (new_password != fpassword) {
                response = "Passwords do not match.\n";
                send(client_socket, response.c_str(), response.length(), 0);
            } else {
            	//unsigned char hash3[SHA256_DIGEST_LENGTH];
    	    	//SHA256((const unsigned char*)confirmed_password.c_str(), confirmed_password.length(), hash3);
                std::ofstream outfile;
                outfile.open("users.csv", std::ios_base::app);
                outfile << new_username << "," << fpassword << std::endl;
                outfile.close();

                response = "Successful.Please login\n";
                send(client_socket, response.c_str(), response.length(), 0);
            }
        }
        } else {
            response = "Invalid.Enter login or signup\n";
            send(client_socket, response.c_str(), response.length(), 0);
        }    
}}else{
  perror("MAX Limit is reached");
}}}
int main() {
    int sock_tcp;
    struct sockaddr_in tcp_address;
    if ((sock_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error in creating TCP socket." << std::endl;
        exit(EXIT_FAILURE);
    }
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_address.sin_port = htons(4010); // use port 8080
    if (bind(sock_tcp, (struct sockaddr*)&tcp_address, sizeof(tcp_address)) < 0) {
        std::cerr << "TCP Socket already in Use, try changing port number" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(sock_tcp, MAX_CONN) < 0) {
        std::cerr << "Server Busy, Please try after some time" << std::endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CONN; i++) {
        threads.push_back(std::thread(join_client, sock_tcp));
    }
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
void client_handler(int client_socket) {
char buffer[1024];
    int bytes_received = recv(client_socket, buffer, 1024, 0);
    while (true) {
    auto current_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(current_time - client_time[client_socket]);
    int minutes = duration.count();
    std::cout << " duration is : " <<  minutes << "\n";
    if(minutes>=30){
    client_time[client_socket] = current_time;
    if(client_Nmsgs[client_socket]>1){
    continue;
    }else{
    std::string response = "InActive for too long. Closing Connection\n";
    send(client_socket, response.c_str(), response.length(), 0);
    close(client_socket);
    }
    }
        std::string response = "Please Choose an option:\n1. List of active clients\n2. Broadcast a message\n3.To establish peer connection enter msg_yourMessage_address";
        send(client_socket, response.c_str(), response.length(), 0);
        bytes_received = recv(client_socket, buffer, 1024, 0);
        client_Nmsgs[client_socket] = client_Nmsgs[client_socket]+1;
        if (bytes_received < 0) {
            return;
        }
        std::string selection(buffer, bytes_received);
        size_t delimiter_pos = selection.find("_");
        std::string first_segment = selection.substr(0, delimiter_pos);
	std::string second_segment = selection.substr(delimiter_pos + 1);
	size_t delimiter_pos2 = second_segment.find("_");
	std::string first_segment2 = second_segment.substr(0, delimiter_pos2);
	std::string second_segment2 = second_segment.substr(delimiter_pos2 + 1);
        if (selection == "1") {
            std::string active_clients_list = "Active Clients:\n";
            for (const auto& client : all_clients) {
                active_clients_list +=   client +"\n";
            }
            send(client_socket, active_clients_list.c_str(), active_clients_list.length(), 0);
        }
         else if (selection == "2") {
            std::string response = "Enter the message to broadcast:\n";
            send(client_socket, response.c_str(), response.length(), 0);
            bytes_received = recv(client_socket, buffer, 1024, 0);
            if (bytes_received < 0) {
                return;
            }
            std::string msg(buffer, bytes_received);
            std::string broadcast_message = "received a broadcasted by " + std::to_string(client_socket-2) + "\nMessage ::\n"+msg; 
            for (int client_socket : Num_clients) {
                    std::cout << " client number is : " <<  client_socket << "\n";
            send(client_socket, broadcast_message.c_str(), broadcast_message.size(), 0);
                }
            response = "Message sent to all clients successfully!\n";
            send(client_socket,response.c_str(), response.length(), 0);
} 
else if (first_segment == "msg") {
            std::cerr << "Entered 3rd segment." << std::endl;
            response = first_segment2;
            std::string response2 = "peers_" + second_segment2;
            std::string serialized_map;
    	    for (const auto& pair : client_map) {
        	serialized_map += pair.first + "," + std::to_string(pair.second) + "\n";
            }
            int target_client_index =  client_map[second_segment2];
            //std::cout << " client is : " <<  second_segment2 << "\n";
            //std::cout << " Message is : " <<  first_segment2 << "\n";
            send(target_client_index,response.c_str(), response.length(), 0);
            //send(client_socket,response2.c_str(), response2.length(), 0);
            }else{
std::string response = "Invalid option.Try Again\n";
send(client_socket, response.c_str(), response.length(), 0);
//close(client_socket);
return;
}            
}}


