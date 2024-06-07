

/***************Server program**************************/

/* server_tcp.c is on eros.cs.txstate.edu
   open a window on eros.
   use a port number between 10,000-15,000.
   compile and run the server program first:
   $gcc -o s server_tcp.c
   $./s 12000
*/

#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <vector>
#include "person.h"




int main(int argc, char **argv){
  int welcomeSocket, newSocket;

/* sockaddr_in is an IPv4-specific address structure used for storing internet addresses. */
  struct sockaddr_in serverAddr;
/* sockaddr_storage is a generic address structure used for storing addresses of various types, such as IPv4 and IPv6. */
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  unsigned short port; /* port server binds to */


  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;

  /* Set port number, using htons function to use proper byte order */
  port = (unsigned short) atoi(argv[1]);
  serverAddr.sin_port = htons(port);

  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    std::cout << "Listening" << std::endl;
  else
      std::cout << "Error" << std::endl;

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;

  while(1)
  {
      char buff[64] = {0};
      std::string reply;
      newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

      if(newSocket < 0)
      {
          throw std::invalid_argument("accept failed!");
      }

      ssize_t len = recv(newSocket, &buff, 64, 0);
      //Option 1: Add Student to database text file
      if(buff[0] == 0x01) {

        if(len != 26)
        {
            std::cout << "corrupted add packet: bad size" << std::endl;
        }
        else
        {
            Person p = from_packet(buff + 1);
            addPerson(p);
            std::cout << "added person with id:" << p.id << std::endl;
        }
      }
      //Option 2: Get student based off Student ID
      else if(buff[0] == 0x02)
      {
          if(len != 7)
          {
              std::cout << "corrupted get by ID packet: bad size" << std::endl;
          }
          else
          {
              std::string targetID;
              for(int i = 1; i < 7; i++)
              {
                  targetID += buff[i];
              }

              Person p;
              if(getPerson(targetID, p))
              {
                  reply = to_packet(p);
              }
          }
      }
      // Option 3: Display All students whose Score is > client supplied score
      else if(buff[0] == 0x03)
      {
            if(len != 2)
        {
            std::cout << "corrupted get all by grade packet: bad size" << std::endl;
        }
        else
        {
            int targetScore = buff[1];
            std::vector<Person> studentsWithHigherGrades = getPeopleWithScore(targetScore);
            for(int i = 0; i < studentsWithHigherGrades.size(); i++){
                reply += to_packet(studentsWithHigherGrades[i]);
                }
        }
      }
      // Option 4: Display all Students
      else if(buff[0] == 0x04)
      {
            std::vector<Person> allStudents = getPeople();
            for(int i = 0; i < allStudents.size(); i++){
               reply += to_packet(allStudents[i]);
               }
      }
      // Option 5: Remove individual based on ID
      else if(buff[0] == 0x05)
      {
              if(len != 7)
        {
            std::cout << "corrupted removal packet: bad size" << std::endl;
        }
        else
        {
              std::string targetID;
              for(int i = 1; i < 7; i++)
              {
                  targetID += buff[i];
              }
              Person p;
              if(getPerson(targetID, p))
              {
                  removePerson(targetID);
                  reply = to_packet(p);
              }
        }
      }
      // Option 6: Quit Option
      else if(buff[0] == 0x06)
      {
       //breaks loop to quit ie close socket
       break;
      }

      if(send(newSocket, reply.c_str(), reply.size(), 0) < 0)
      {
          throw std::invalid_argument("failed to send");
      }

      close(newSocket);


  }

  close(welcomeSocket);

  return 0;
}
