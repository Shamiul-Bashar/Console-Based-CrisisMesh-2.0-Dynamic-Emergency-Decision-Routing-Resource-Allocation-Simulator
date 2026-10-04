#pragma once

#include <random>
#include <string>

namespace crisismesh {

class AuthService {
public:
    static int generateOtp() {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(100000, 999999);
        return dist(rng);
    }

    static bool validPassword(const std::string& password) {
        if (password.size() < 6) return false;
        bool hasLetter = false, hasDigit = false;
        for (char c : password) {
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) hasLetter = true;
            if (c >= '0' && c <= '9') hasDigit = true;
        }
        return hasLetter && hasDigit;
    }

    static bool validEmail(const std::string& email) {
        const auto at = email.find('@');
        const auto dot = email.rfind('.');
        return at != std::string::npos && dot != std::string::npos && at > 0 && dot > at + 1;
    }
};

} // namespace crisismesh
