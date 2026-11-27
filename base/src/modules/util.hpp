#pragma once

#include <string>
#include <unordered_map>

namespace modules {

///
/// Get the value of a key from a map.
/// @tparam T Type of value.
/// @param map Map to find key in.
/// @param key Key to find.
/// @param val Value to return to.
/// @param throwNotFound Set to true in order to throw an exception on key not found.
/// @return True if key was found, false if not.
/// @note The only types that are supported are std::string, int, and float.
///
template<typename T>
bool getKeyValue(std::unordered_map<std::string, std::string>& map, std::string key, T& val, bool throwNotFound) {
    auto it = map.find(key);
    if (it != map.end()) {
        try {
            std::string second = it->second;
            if constexpr (std::is_same_v<T, std::string>) {
                val = second;
            } else if constexpr (std::is_same_v<T, int>) {
                val = std::stoi(second);
            } else if constexpr (std::is_same_v<T, float>) {
                val = std::stof(second);
            } else {
                throw "Invalid type";
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

///
/// Check if a value is between min and max inclusive.
/// @tparam Type of value to check.
/// @param val Value to check.
/// @param min Minimum value.
/// @param max Maximum value.
/// @param throwOutOfBound Set to true in order to throw an exception on out of bounds.
///
template<typename T>
bool checkMinMax(T val, T min, T max, bool throwOutOfBound) {
    bool ret = true;
    if(val > max || val < min) {
        ret = false;
    }
    if(!ret && throwOutOfBound) {
        throw "Out of bounds";
    }
    return ret;
}

} // namespace modules
