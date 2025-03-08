#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>

using namespace std;

void p2p(int port, int uid);
void p2p_join();
void p2p_publish();
void p2p_search();
void p2p_exit();

int main(int argc, char* argv[]) {
    //verify correct number of function arguements
    if(argc != 3) {
        cerr << "CALL FUNCTION WITH : executable, port#, uid\n";
        return 1;
    }

    int port = stoi(argv[1]);
    int uid = std::stoi(argv[2]);

    p2p(int port, int uid);

    return 0;
}

void p2p(int port, int uid) {
    while(1) {
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
            p2p_join();
        }
        else if(action == 1) { //PUBLISH
            p2p_publish();
        }
        else if(action == 2) { //SEARCH
            p2p_search();
        }
        else if(action == 3) { //EXIT
            p2p_exit();
        }
    }
}

void p2p_join() {
    return ;
}

void p2p_publish() {
    return ;
}

void p2p_search() {
    return ;
}

void p2p_exit() {
    return ;
}