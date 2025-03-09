//EECE446, SPRING 2025
//Kai Cokeley, Charles Boyle

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <filesystem>
#include <arpa/inet.h>

using namespace std;
namespace fs = std::filesystem;

void p2p(string registry_location, uint32_t uid, int server);
void p2p_join(int server, uint8_t action, uint32_t uid);
void p2p_publish(int server, uint8_t action);
void p2p_search(int server, uint8_t action);
int lookup_and_connect(const char *host, const char *service);

int main(int argc, char* argv[]) {
    //verify correct number of function arguements
    if(argc != 4) {
        cerr << "CALL FUNCTION WITH: " << argv[0] << ", registry_location, registry_port, uid\n";
        return 1;
    }

    string registry_location = argv[1];
    string registry_port_str = argv[2];
    uint32_t uid = stoul(argv[3]);

    //verify valid port # was selected
    int registry_port_verify = stoi(registry_port_str);
    if(registry_port_verify <= 2000 || registry_port_verify >= 65536) {
        cerr << "INVALID PORT #, PORT MUST BE IN [2001, 65535]\n";
        return 1;
    }

    //lookup IP and connect to server
    int server;
    if((server = lookup_and_connect(registry_location.c_str(), registry_port_str.c_str())) < 0) {
        cerr << "ERROR, FAILED TO CONNECT TO SERVER\n";
        return 1;
    }

    //run p2p
    p2p(registry_location, uid, server);
    close(server);

    return 0;
}

void p2p(string registry_location, uint32_t uid, int server) {
    while(1) {
        cout << "Enter a command: ";

        //read user input from the terminal
        string user_input;
        getline(cin, user_input);

        //parse user input, get user action
        uint8_t action =    (user_input == "JOIN") ? 0 :
                            (user_input == "PUBLISH") ? 1 :
                            (user_input == "SEARCH") ? 2 : 
                            (user_input == "EXIT") ? 3 : 255; //default to 255 for undefined actions

        //interpret user input and call appropriate function
        if(action == 0) { //JOIN
            p2p_join(server, action, uid);
        }
        else if(action == 1) { //PUBLISH
            p2p_publish(server, action);
        }
        else if(action == 2) { //SEARCH
            p2p_search(server, action);
        }
        else if(action == 3) { //EXIT
            break;
        }
        else {
            cout << "Commands are: JOIN, PUBLISH, SEARCH, EXIT\n";
        }
    }
}

void p2p_join(int server, uint8_t action, uint32_t uid) {
    uint32_t uid_prime = htonl(uid);
    
    //prepare to send in order: action, uid
    vector<char> buffer;
    buffer.push_back(action);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&uid_prime), reinterpret_cast<char*>(&uid_prime) + sizeof(uid_prime));

    //send join request to server
    if(send(server, buffer.data(), buffer.size(), 0) < 0) {
        cerr << "ERROR, JOIN SEND\n";
    }
}

void p2p_publish(int server, uint8_t action) {    
    //add all files in SharedFiles to a vector
    vector<string> files;
    for(const auto& file : fs::directory_iterator("SharedFiles")) {
        if (fs::is_regular_file(file.status())) {
            files.push_back(file.path().filename().string());
        }
    }

    //count number of files
    uint32_t count = files.size();
    uint32_t count_prime = htonl(count);

    //prepare to send in order: action, count, files
    vector<char> buffer;
    buffer.push_back(action);
    buffer.insert(buffer.end(), reinterpret_cast<char*>(&count_prime), reinterpret_cast<char*>(&count_prime) + sizeof(count_prime));
    for(const auto& file : files) {
        //enforce maximum file size
        if(file.length() >= 100) {
            cerr << "File \'" << file << "\' in SharedFiles exceeds 100'B";
            continue;
        }

        //add file to buffer
        buffer.insert(buffer.end(), file.begin(), file.end());
        buffer.push_back('\0');
    }

    //send publish to server
    if(send(server, buffer.data(), buffer.size(), 0) < 0) {
        cerr << "ERROR, PUBLISH SEND\n";
        return ;
    }
}

void p2p_search(int server, uint8_t action) {
    //get filename from user
    cout << "Enter a file name: ";
    string filename;
    getline(cin, filename);

    //prepare to send in order: action, filename
    vector<char> buffer;
    buffer.push_back(action);
    buffer.insert(buffer.end(), filename.begin(), filename.end());
    buffer.push_back('\0');

    //send search to server
    if(send(server, buffer.data(), buffer.size(), 0) < 0) {
        cerr << "ERROR, SEARCH SEND\n";
        return ;
    }

    //recieve search response from server
    char response[10];
    if(recv(server, response, sizeof(response), 0) < 0) {
        cerr << "ERROR, SEARCH RECV\n";
        return ;
    }

    //parse server response: peer_id, peer_ip, peer_port
    uint32_t peer_id = ntohl(*reinterpret_cast<uint32_t*>(response));
    uint32_t peer_ip = *reinterpret_cast<uint32_t*>(response + 4);
    uint16_t peer_port = ntohs(*reinterpret_cast<uint16_t*>(response + 8));

    //output server response to terminal
    if(peer_id == 0 && peer_ip == 0 && peer_port == 0) { //search response is all 0's, file not in registry
        cout << "File not indexed by registry\n";
    } 
    else { //file is in registry, print to user
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, reinterpret_cast<in_addr*>(&peer_ip), ip_str, INET_ADDRSTRLEN);
    
        cout << "File found at\n";
        cout << "\tPeer " << peer_id << "\n";
        cout << "\t" << ip_str << ":" << peer_port << "\n";
    }
}

//copied from project1
int lookup_and_connect( const char *host, const char *service ) {
    struct addrinfo hints;
    struct addrinfo *rp, *result;
    int s;

    /* Translate host name into peer's IP address */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
        fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
        return -1;
    }

    /* Iterate through the address list and try to connect */
    for ( rp = result; rp != NULL; rp = rp->ai_next ) {
        if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
            continue;
        }

        if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
            break;
        }

        close( s );
    }
    if ( rp == NULL ) {
        perror( "stream-talk-client: connect" );
        return -1;
    }
    freeaddrinfo( result );

    return s;
}