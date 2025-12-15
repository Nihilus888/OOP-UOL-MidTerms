#pragma once

#include "User.h"
#include <string>

class UserService
{
public:
    static User registerUser();
    static User loginUser();

private:
    static bool userExists(const std::string& email, const std::string& fullName);
    static std::string generateUsername();
    static size_t hashPassword(const std::string& password);
};
