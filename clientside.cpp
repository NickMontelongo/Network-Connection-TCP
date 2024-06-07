/****************Client program********************************/

/* client_tcp.c is on zeus.cs.txstate.edu
   open a window on zeus.
   use a port number between 10,000-15,000.
   compile:
   $gcc -o c client_tcp.c
   $./c eros.cs.txstate.edu 12000
*/
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <limits>
#include "person.h"
using namespace std;


string getStringWithSizeRange(const string& prompt, int minSize, int maxSize) {
    string input;
    do {
        cout << prompt;
        cin >> input;
        if (input.length() < minSize || input.length() > maxSize) {
            cout << "Invalid input size. Please enter a string of length between "
                 << minSize << " and " << maxSize << "." << endl;
            // Clear input buffer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    } while (true);
    return input;
}


int getIntInRange(const string& prompt, int minVal, int maxVal) {
    int input;
    do {
        cout << prompt;
        cin >> input;
        if (cin.fail() || input < minVal || input > maxVal) {
            cout << "Invalid input. Please enter an integer between "
                 << minVal << " and " << maxVal << "." << endl;
            // Clear input buffer
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    } while (true);
    return input;
}

int main(int argc, char **argv){

  int clientSocket;
  char buffer[1024] = {0};
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  unsigned short port;
  struct hostent *hostnm;

  uint32_t num, cnum;
  int option = 0;




//xxxxxxxxxxxMENUxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  do {


      /*---- Create the socket. The three arguments are: ----*/
      /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
      clientSocket = socket(PF_INET, SOCK_STREAM, 0);

      /*---- Configure settings of the server address struct ----*/
      /* Address family = Internet */
      serverAddr.sin_family = AF_INET;

      /* set port number*/
      port = (unsigned short)atoi(argv[2]);

      /* htons() stands for "host to network short" and is used to convert the port number stored in the "port" variable from host byte order to network byte order, ensuring that data is not misinterpreted when sent over the network. */
      serverAddr.sin_port = htons(port);

      /* Set IP address to localhost */
      hostnm = gethostbyname(argv[1]);

    /* This sets the serverAddr structure's sin_addr member to the host address provided by the hostnm->h_addr variable. The s_addr field contains the IP address of the host in network byte order, and the *((unsigned long *)hostnm->h_addr) expression casts the data to an unsigned long type. This ensures that the IP address taken from hostnm is formatted correctly for network communication. */
      serverAddr.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /* Set all bits of the padding field to 0. It is used to ensure that the serverAddr structure is correctly zero initialized before use, which is necessary for certain network operations. */
      memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

      /*---- Connect the socket to the server using the address struct ----*/
      addr_size = sizeof serverAddr;

      connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);








        std::cout << "\nMenu:\n";
        std::cout << "1: Add Student\n";
        std::cout << "2: Display Student by ID\n";
        std::cout << "3: Display Student by Score\n";
        std::cout << "4: Display All Students\n";
        std::cout << "5: Delete Student by ID\n";
        std::cout << "6: Exit\n";
        std::cout << "Enter your choice (1-6): ";
        std::cin >> option;

        std::string msg;

        // Send the selected option to the server
        cnum = htonl(option);

        switch (option) {
            case 1: // Add Student to Database
                {
                string studentID, firstName, lastName, scoreStr;

                //Setting user Variables by getting user Input within a valid range
                studentID = getStringWithSizeRange("Enter student ID (6 characters): ",6,6);
                firstName = getStringWithSizeRange("Enter first name (max 9 characters): ",1,9);
                lastName = getStringWithSizeRange("Enter last name (max 9 characters): ",1,9);
                scoreStr= getIntInRange("Enter student score from (0-100): ",0,100);

                //Creation of Person Instance
                Person personToAdd;
                personToAdd.id = studentID;
                personToAdd.firstName = firstName;
                personToAdd.lastName = lastName;
                personToAdd.score = scoreStr[0];

                //Packaging User information


                msg += 0x01;
                msg += to_packet(personToAdd);

                }
                break;
            case 2: // Display Student by ID
                {
                string studentIDToSearch = getStringWithSizeRange("Enter a student ID to Search(6 characters): ",6,6);

                msg += 0x02;
                msg += studentIDToSearch;
                }
                break;
            case 3: // Display Student with Score greater than supplied score
                // Implement code to display a student by score
                {
                string prompt = "Please enter a score between (0-100), to show students who have a score above said value: ";
                int scoreToCompare = getIntInRange(prompt,0,100);

                msg += 0x03;
                msg += scoreToCompare;

                }
                break;
            case 4: // Display All Students
                {
                msg += 0x04;
                }
                break;
            case 5: // Delete Student by ID
                {
                 string studentIDToSearch = getStringWithSizeRange("Enter a student ID for deletion(6 characters): ",6,6);


                 msg += 0x05;
                 msg += studentIDToSearch;
                }
                break;
            case 6: // Exit
                std::cout << "Exiting Program\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }



        //sending information
        if(send(clientSocket, msg.c_str(), msg.size(), 0) < 0)
        {
            throw std::invalid_argument("failed to send");
        }

        char buff[65535] = {0};

        // Receive response from the server
        ssize_t len = recv(clientSocket, buff, sizeof(buff), 0);

        for(int i = 0; i < len / 25; i++)
        {
            Person p = from_packet(buff + i * 25);
            std::cout << p.firstName << " " << p.lastName << " " << p.id << " " << p.score << endl;
        }


        // Close the client socket
        close(clientSocket);


    } while (option != 6);


    return 0;
}
