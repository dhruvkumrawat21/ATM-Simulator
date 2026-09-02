#ifndef PERSON_H
#define PERSON_H

#include <string>

// Base class representing any person in the system
class Person {
protected:
    std::string name;

public:
    Person(const std::string& n) : name(n) {}
    virtual ~Person() = default;

    std::string getName() const { return name; }
};

#endif // PERSON_H
