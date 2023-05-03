#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <openssl/sha.h>

#define Length_Message 255

int socket_ad;

//define functions
void *receiveMessage(void *arg);
void *sendMessage(void *arg);

void *receiveMessage(void *arg) {
    char packet[Length_Message+1];
    while (1) {
        int r = recv(socket_ad, packet, Length_Message, 0);
        if (r > 0) {
            packet[r] = '\0';
            //std::cout << "Is peer value is :  " << strcmp(packet,"peer") << "\n";
            printf("%s\n", packet);
            std::string peer(packet);
            size_t delimiter_pos = peer.find("_");
            std::string first_segment = peer.substr(0, delimiter_pos);
	    std::string client_address = peer.substr(delimiter_pos + 1);
	    
	    std::string comp = "peer";
	    
            if(strcmp(first_segment.c_str(),"peer")==0){
            size_t delimiter_pos2 = client_address.find(":");
            std::string Ip = client_address.substr(0, delimiter_pos2);
	    std::string port2 = client_address.substr(delimiter_pos2 + 1);
	    uint16_t port = std::stoul(port2);
            std::string remote_ip = Ip ;
	    int remote_port = port;
	    
	    // Create a TCP socket
	    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0) {
		return NULL;
	    }
	    // Define the address of the remote client
	    struct sockaddr_in remote_addr;
	    memset(&remote_addr, 0, sizeof(remote_addr));
	    remote_addr.sin_family = AF_INET;
	    remote_addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
	    remote_addr.sin_port = htons(remote_port);
	    
	    // Connect to the remote client
	    if (connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) {
		std::cerr << "Error connecting to remote client" << std::endl;
		return NULL;
	    }
		//perror();
		}
		} else {
		    break;
		}
        
    }
    return NULL;
}
int main() {
    socket_ad = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ad == -1) {
        return -1;
    }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(4010);
    
    int r = connect(socket_ad, (struct sockaddr *)&addr, sizeof addr);
    if (r < 0) {
        return -1;
    }
    pthread_t readThread, sendThread;
    pthread_create(&readThread, NULL, receiveMessage, NULL);
    pthread_create(&sendThread, NULL, sendMessage, NULL);

    pthread_join(readThread, NULL);
    pthread_join(sendThread, NULL);

    close(socket_ad);
    return 0;
}
void *sendMessage(void *arg) {
    char packet[Length_Message+1];
    while (1) {
        fgets(packet, Length_Message+1, stdin);
        int len = strlen(packet);
        if (len > 0) {
            if (packet[len-1] == '\n') {
                packet[len-1] = '\0';
            }
            if (send(socket_ad, packet, strlen(packet), MSG_NOSIGNAL) == -1) {
                break;
            }
        }
    }
    return NULL;
}


