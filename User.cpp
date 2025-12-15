#include "User.h"
#include <random>
#include <sstream>
#include <functional>
#include <fstream>
#include <vector>
#include <iostream>

// Generate a random 10-digit username
std::string generateUsername() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 9);

    std::stringstream ss;
    for (int i = 0; i < 10; ++i) {
        ss << dist(gen);
    }
    return ss.str();
}

// Hash a password
size_t hashPassword(const std::string& password) {
    return std::hash<std::string>{}(password);
}

// Check if a user exists
bool userExists(const std::string& email, const std::string& fullname) {
    std::ifstream file("users.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string uname, f, e, ph;
        std::getline(ss, uname, ',');
        std::getline(ss, f, ',');
        std::getline(ss, e, ',');
        std::getline(ss, ph, ',');
        if (f == fullname && e == email) return true;
    }
    return false;
}

// Register a new user
void registerUser() {
    std::string fullname, email, password;
    std::cout << "Full name: "; std::getline(std::cin, fullname);
    std::cout << "Email: "; std::getline(std::cin, email);
    std::cout << "Password: "; std::getline(std::cin, password);

    if (userExists(email, fullname)) {
        std::cout << "User already exists.\n";
        return;
    }

    std::string username = generateUsername();
    size_t passwordHash = hashPassword(password);

    std::ofstream file("users.csv", std::ios::app);
    file << username << "," << fullname << "," << email << "," << passwordHash << "\n";

    std::cout << "Registered! Your username is: " << username << "\n";
}

// Login an existing user
User loginUser() {
    std::string username, password;
    std::cout << "Username: "; std::getline(std::cin, username);
    std::cout << "Password: "; std::getline(std::cin, password);
    size_t hashInput = hashPassword(password);

    std::ifstream file("users.csv");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string uname, fullname, email, phash;
        std::getline(ss, uname, ',');
        std::getline(ss, fullname, ',');
        std::getline(ss, email, ',');
        std::getline(ss, phash, ',');
        if (uname == username && std::stoull(phash) == hashInput) {
            User u;
            u.username = uname;
            u.fullname = fullname;
            u.email = email;
            u.passwordHash = hashInput;
            std::cout << "Login successful!\n";
            return u;
        }
    }
    std::cout << "Invalid username or password.\n";
    return User(); // empty user if login fails
}
