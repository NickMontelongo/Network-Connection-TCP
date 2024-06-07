#include "person.h"

#include <fstream>

void addPerson(const Person &p)
{
    std::string score = std::to_string(p.score);
    std::ofstream out("people.txt", std::ios_base::app);
    out.write(p.id.c_str(), p.id.size());
    out.write("\n", 1);
    out.write(p.firstName.c_str(), p.firstName.size());
    out.write("\n", 1);
    out.write(p.lastName.c_str(), p.lastName.size());
    out.write("\n", 1);
    out.write(score.c_str(), score.size());
    out.write("\n", 1);
    out.close();
}

bool getPerson(const std::string &id, Person &out)
{
    bool ret = false;
    std::ifstream in("people.txt");
    std::string line1, line2, line3, line4;

    while(std::getline(in, line1) &&
          std::getline(in, line2) &&
          std::getline(in, line3) &&
          std::getline(in, line4))
    {
        if(line1 == id)
        {
            out.id = line1;
            out.firstName = line2;
            out.lastName = line3;
            out.score = std::stoi(line4);
            ret = true;
            break;
        }
    }

    in.close();
    return ret;
}

std::vector<Person> getPeople()
{
    std::vector<Person> people;

    std::ifstream in("people.txt");
    std::string line1, line2, line3, line4;

    while(std::getline(in, line1) &&
          std::getline(in, line2) &&
          std::getline(in, line3) &&
          std::getline(in, line4))
    {
        Person p;
        p.id = line1;
        p.firstName = line2;
        p.lastName = line3;
        p.score = std::stoi(line4);
        people.push_back(p);
    }

    in.close();
    return people;
}

std::vector<Person> getPeopleWithScore(int score)
{
    std::vector<Person> people;

    std::ifstream in("people.txt");
    std::string line1, line2, line3, line4;

    while(std::getline(in, line1) &&
          std::getline(in, line2) &&
          std::getline(in, line3) &&
          std::getline(in, line4))
    {
        int currentScore = std::stoi(line4);
        if(currentScore > score)
        {
            Person p;
            p.id = line1;
            p.firstName = line2;
            p.lastName = line3;
            p.score = currentScore;
            people.push_back(p);
        }
    }

    in.close();
    return people;
}

bool removePerson(const std::string &id)
{
    bool ret = false;
    std::ifstream in("people.txt");
    std::ofstream out("people_temp.txt");
    std::string line1, line2, line3, line4;

    while(std::getline(in, line1) &&
          std::getline(in, line2) &&
          std::getline(in, line3) &&
          std::getline(in, line4))
    {
        if(line1 == id)
        {
            ret = true;
        }
        else
        {
            out.write(line1.c_str(), line1.size());
            out.write("\n", 1);
            out.write(line2.c_str(), line2.size());
            out.write("\n", 1);
            out.write(line3.c_str(), line3.size());
            out.write("\n", 1);
            out.write(line4.c_str(), line4.size());
            out.write("\n", 1);
        }
    }

    in.close();
    out.close();

    rename("people_temp.txt", "people.txt");

    return ret;
}

std::string to_packet(const Person &p)
{
    std::string reply;
    char first[9] = {0};
    char last[9] = {0};

    for(size_t i = 0; i < p.firstName.size(); i++)
    {
        first[i] = p.firstName[i];
    }
    for(size_t i = 0; i < p.lastName.size(); i++)
    {
        last[i] = p.lastName[i];
    }

    for(int i = 0; i < 6; i++)
    {
        reply += p.id[i];
    }
    for(int i = 0; i < 9; i++)
    {
        reply += first[i];
    }
    for(int i = 0; i < 9; i++)
    {
        reply += last[i];
    }
    reply += p.score;

    return reply;
}

Person from_packet(char *buff)
{
    std::string id, first, last;
    char score;

    for(int i =0 ; i < 6; i++)
    {
        if(buff[i] != '\0')
            id += buff[i];
    }
    for(int i = 6; i < 15; i++)
    {
        if(buff[i] != '\0')
            first += buff[i];
    }
    for(int i = 15; i < 24; i++)
    {
        if(buff[i] != '\0')
            last += buff[i];
    }
    score = buff[24];

    Person p{id, first, last, score};

    return p;
}

