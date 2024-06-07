#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>

#define MAX_ID_LENGTH 7
#define MAX_NAME_LENGTH 10
#define MAX_PERSONS 100

struct Person {
    std::string id;
    std::string firstName;
    std::string lastName;
    int score;
};

void addPerson(const Person &p);
bool getPerson(const std::string &id, Person &out);
std::vector<Person> getPeople();
std::vector<Person> getPeopleWithScore(int score);
bool removePerson(const std::string &id);

std::string to_packet(const Person &p);
Person from_packet(char *buff);

#endif // PERSON_H
