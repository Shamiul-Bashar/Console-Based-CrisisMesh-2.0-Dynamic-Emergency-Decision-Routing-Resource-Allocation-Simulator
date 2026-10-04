#pragma once

#include "dsa/Array.hpp"
#include "models/Models.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace crisismesh {

class UserStorage {
public:
    static std::string defaultPath() {
        return "data/users.txt";
    }

    template <std::size_t Capacity>
    static bool load(StaticArray<User, Capacity>& users,
                     int& nextUserId,
                     const std::string& path = defaultPath()) {
        users.clear();
        nextUserId = 1;

        std::ifstream input(path);
        if (!input.is_open()) {
            return true; // First run: no storage file yet.
        }

        int maxId = 0;
        std::string line;

        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream row(line);
            std::string tag;
            row >> tag;

            if (tag == "NEXT_ID") {
                int storedNext = 1;
                if (row >> storedNext && storedNext > 0)
                    nextUserId = storedNext;
                continue;
            }

            if (tag != "USER") continue;

            User user;
            if (!(row >> user.id
                      >> std::quoted(user.name)
                      >> std::quoted(user.email)
                      >> std::quoted(user.phone)
                      >> std::quoted(user.username)
                      >> std::quoted(user.password))) {
                continue; // Ignore malformed rows instead of breaking the application.
            }

            if (user.id <= 0 || user.username.empty() || users.full())
                continue;

            bool duplicate = false;
            for (std::size_t i = 0; i < users.size(); ++i) {
                if (users[i].id == user.id || users[i].username == user.username) {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) continue;

            users.pushBack(user);
            if (user.id > maxId) maxId = user.id;
        }

        if (nextUserId <= maxId)
            nextUserId = maxId + 1;

        return true;
    }

    template <std::size_t Capacity>
    static bool save(const StaticArray<User, Capacity>& users,
                     int nextUserId,
                     const std::string& path = defaultPath()) {
        try {
            const std::filesystem::path target(path);
            if (target.has_parent_path())
                std::filesystem::create_directories(target.parent_path());

            const std::filesystem::path temp = target.string() + ".tmp";
            std::ofstream output(temp, std::ios::trunc);
            if (!output.is_open()) return false;

            output << "# CrisisMesh 2.0 persistent user data\n";
            output << "# Format: USER <id> <name> <email> <phone> <username> <password>\n";
            output << "NEXT_ID " << nextUserId << "\n";

            for (std::size_t i = 0; i < users.size(); ++i) {
                const User& user = users[i];
                output << "USER "
                       << user.id << ' '
                       << std::quoted(user.name) << ' '
                       << std::quoted(user.email) << ' '
                       << std::quoted(user.phone) << ' '
                       << std::quoted(user.username) << ' '
                       << std::quoted(user.password) << "\n";
            }

            output.flush();
            if (!output.good()) {
                output.close();
                std::error_code ignore;
                std::filesystem::remove(temp, ignore);
                return false;
            }
            output.close();

            std::error_code error;
            const std::filesystem::path backup = target.string() + ".bak";

            std::filesystem::remove(backup, error);
            error.clear();

            const bool hadExistingFile = std::filesystem::exists(target);
            if (hadExistingFile) {
                std::filesystem::rename(target, backup, error);
                if (error) {
                    std::filesystem::remove(temp, error);
                    return false;
                }
            }

            error.clear();
            std::filesystem::rename(temp, target, error);
            if (error) {
                std::error_code restoreError;
                if (hadExistingFile && std::filesystem::exists(backup))
                    std::filesystem::rename(backup, target, restoreError);
                std::filesystem::remove(temp, restoreError);
                return false;
            }

            std::filesystem::remove(backup, error);
            return true;
        } catch (...) {
            return false;
        }
    }
};

} // namespace crisismesh
