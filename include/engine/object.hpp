#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <string>
#include <vector>

class object
{
public:
private:
};

class FailedToParseException : public std::exception
{
public:
    FailedToParseException(std::string what_msg) : what_msg(what_msg) {};

    const char *what() const noexcept
    {
        return what_msg.c_str();
    }

private:
    std::string what_msg;
};

#endif