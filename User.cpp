#include "User.h"

User::User()
{
    username = "";
    fullName = "";
    email = "";
    passwordHash = 0;
}

User::User(const std::string& username,
           const std::string& fullName,
           const std::string& email,
           size_t passwordHash)
{
    this->username = username;
    this->fullName = fullName;
    this->email = email;
    this->passwordHash = passwordHash;
}
