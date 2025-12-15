#pragma once
#include <string>

class User {
public:
    std::string username;      // Unique 10-digit number
    std::string fullname;
    std::string email;
    size_t passwordHash;       // hashed using std::hash
};

// Function declarations
std::string generateUsername();
size_t hashPassword(const std::string& password);
bool userExists(const std::string& email, const std::string& fullname);
void registerUser();
User loginUser();
