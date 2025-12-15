#include "UserService.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <functional>

static const std::string USER_FILE = "users.csv";

/* ---------- helpers ---------- */

std::string UserService::generateUsername()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 9);

    std::string username;
    for (int i = 0; i < 10; ++i)
        username += std::to_string(dist(gen));

    return username;
}

size_t UserService::hashPassword(const std::string& password)
{
    return std::hash<std::string>{}(password);
}

bool UserService::userExists(const std::string& email, const std::string& fullName)
{
    std::ifstream file(USER_FILE);
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string uname, name, mail, hash;

        std::getline(ss, uname, ',');
        std::getline(ss, name, ',');
        std::getline(ss, mail, ',');
        std::getline(ss, hash, ',');

        if (name == fullName && mail == email)
            return true;
    }
    return false;
}

/* ---------- registration ---------- */

User UserService::registerUser()
{
    std::string fullName, email, password;

    std::cout << "Full name: ";
    std::getline(std::cin, fullName);

    std::cout << "Email: ";
    std::getline(std::cin, email);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (userExists(email, fullName))
    {
        std::cout << "User already exists.\n";
        return User();
    }

    std::string username = generateUsername();
    size_t pwHash = hashPassword(password);

    std::ofstream file(USER_FILE, std::ios::app);
    file << username << ","
         << fullName << ","
         << email << ","
         << pwHash << "\n";

    std::cout << "Registered successfully!\n";
    std::cout << "Your username is: " << username << "\n";

    return User(username, fullName, email, pwHash);
}

/* ---------- login ---------- */

User UserService::loginUser()
{
    std::string username, password;

    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    size_t inputHash = hashPassword(password);

    std::ifstream file(USER_FILE);
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string uname, name, email, hash;

        std::getline(ss, uname, ',');
        std::getline(ss, name, ',');
        std::getline(ss, email, ',');
        std::getline(ss, hash, ',');

        if (uname == username && std::stoull(hash) == inputHash)
        {
            std::cout << "Login successful!\n";
            return User(uname, name, email, inputHash);
        }
    }

    std::cout << "Invalid login.\n";
    std::cout << "If you forgot your username or password, please re-register.\n";
    return User();
}
