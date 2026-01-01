#pragma once

#include <string>

class User
{
public:
    User();
    User(const std::string& username,
         const std::string& fullName,
         const std::string& email,
         size_t passwordHash);

    std::string username;   
    std::string fullName;
    std::string email;
    size_t passwordHash;    
};
