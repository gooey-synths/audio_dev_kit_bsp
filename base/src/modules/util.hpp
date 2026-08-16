#pragma once

#include <string>
#include <unordered_map>

///
/// Get the value of a key from a map.
/// @tparam T Type of value.
/// @param map Map to find key in.
/// @param key Key to find.
/// @param val Value to return to.
/// @param throwNotFound Set to true in order to throw and exception on key not found.
/// @return True if key was found, false if not.
///
template<typename T>
bool getKeyValue(std::unordered_map<std::string, std::string>& map, std::string key, T& val, bool throwNotFound) {
    auto it = map.find("idx");
    if (it != map.end()) {
        try {
            std::string second = it->second;
            if constexpr (std::is_same_v<T, std::string>) {
                val = second;
            } else if constexpr (std::is_same_v<T, int>) {
                val = std::stoi(second);
            } else if constexpr (std::is_same_v<T, float>) {
                val = std::stof(second);
            }
        } catch(...) {
            throw "Invalid value";
        }
        return true;
    }

    if(throwNotFound) {
        throw "Key not found";
    }

    return false;
}

