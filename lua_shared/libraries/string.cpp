#include "CLuaBase.h"
#include <lua.hpp>
#include <regex>

// https://wiki.facepunch.com/gmod/string.EndsWith
int CLuaBase::lua$string_EndsWith()
{
    std::string str = lua_tostring(lua_state, 1);
    std::string end = lua_tostring(lua_state, 2);

    lua_pushboolean(lua_state, str.ends_with(end));
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Explode
int CLuaBase::lua$string_Explode()
{
    // FIXME: Support patterns.
    std::string separator = lua_tostring(lua_state, 1);
    std::string input = lua_tostring(lua_state, 2);

    lua_newtable(lua_state);

    size_t next_start = 0;
    double next_table_index = 1;
    while (true) {
        size_t separator_index = input.find(separator, next_start);

        lua_pushnumber(lua_state, next_table_index);
        lua_pushstring(lua_state, input.substr(next_start, separator_index - next_start).c_str());
        lua_settable(lua_state, -3);

        if (separator_index == std::string::npos)
            break;

        next_start = separator_index + separator.length();
        next_table_index++;
    }

    return 1;
}

int CLuaBase::lua$string_format()
{
    // FIXME: Support options for format specifiers.
    std::string format = lua_tostring(lua_state, 1);
    size_t format_index = 0;
    size_t vararg_index = 2;
    std::string result;

    bool in_format_specifier = false;
    while (format_index < format.length()) {
        char next = format[format_index++];

        if (in_format_specifier) {
            switch (next) {
            case 's': {
                lua_pushcfunction(lua_state, lua$tostring$entry);
                lua_pushvalue(lua_state, vararg_index++);
                lua_call(lua_state, 1, 1);
                std::string string = lua_tostring(lua_state, -1);
                lua_pop(lua_state, 1);
                result += string;
                in_format_specifier = false;
                break;
            }
            case 'c': {
                char character = static_cast<char>(lua_tonumber(lua_state, vararg_index++));
                result += character;
                in_format_specifier = false;
                break;
            }
            case 'e':
            case 'E':
            case 'f':
            case 'G': {
                char small_format[3];
                char buffer[32];
                double number = lua_tonumber(lua_state, vararg_index++);
                small_format[0] = '%';
                small_format[1] = next;
                small_format[2] = 0;
                size_t length = snprintf(buffer, sizeof(buffer), small_format, number);
                result += { buffer, length };
                in_format_specifier = false;
                break;
            }
            case 'd':
            case 'i':
            case 'u':
            case 'o':
            case 'x':
            case 'X': {
                char small_format[3];
                char buffer[32];
                long long number = lua_tonumber(lua_state, vararg_index++);
                small_format[0] = '%';
                small_format[1] = next;
                small_format[2] = 0;
                size_t length = snprintf(buffer, sizeof(buffer), small_format, number);
                result += { buffer, length };
                in_format_specifier = false;
                break;
            }
            case '%': {
                result += '%';
                in_format_specifier = false;
                break;
            }
            default:
                printf("Unhandled format specifier: '%c%s'\n", next, format.substr(format_index).c_str());
                break;
            }
            continue;
        }

        if (next == '%') {
            in_format_specifier = true;
            continue;
        }

        result += next;
    }

    lua_pushstring(lua_state, result.c_str());
    return 1;
}

// https://wiki.facepunch.com/gmod/string.match
int CLuaBase::lua$string_match()
{
    std::string string = lua_tostring(lua_state, 1);
    // FIXME: Implement support for Lua's pattern matching language.
    // https://wiki.facepunch.com/gmod/Patterns
    std::regex pattern = std::regex(lua_tostring(lua_state, 2));
    double start_position = lua_gettop(lua_state) >= 3 ? lua_tonumber(lua_state, 3) : 1;

    if (start_position > 1) {
        string = string.substr(start_position - 1);
    } else if (start_position < 0) {
        string = string.substr(string.length() + start_position);
    }

    std::smatch matches;
    if (!std::regex_search(string, matches, pattern)) {
        lua_pushnil(lua_state);
        return 1;
    }

    if (matches.size() == 1) {
        lua_pushstring(lua_state, matches[0].str().c_str());
        return 1;
    }

    for (size_t i = 1; i < matches.size(); i++) {
        if (matches[i].matched)
            lua_pushstring(lua_state, matches[i].str().c_str());
        else
            lua_pushnil(lua_state);
    }

    return matches.size() - 1;
}

// https://wiki.facepunch.com/gmod/string.rep
int CLuaBase::lua$string_rep()
{
    std::string str = lua_tostring(lua_state, 1);
    size_t repetitions = lua_tonumber(lua_state, 2);
    std::string separator = lua_gettop(lua_state) >= 3 ? lua_tostring(lua_state, 3) : "";

    std::string result;

    while (repetitions > 0) {
        result += str;
        repetitions--;

        if (repetitions > 0)
            result += separator;
    }

    lua_pushstring(lua_state, result.c_str());
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Split
int CLuaBase::lua$string_Split()
{
    lua_pushcfunction(lua_state, lua$string_Explode$entry);
    lua_pushvalue(lua_state, 2);
    lua_pushvalue(lua_state, 1);
    lua_pushboolean(lua_state, false);
    lua_call(lua_state, 3, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/string.StartWith
int CLuaBase::lua$string_StartWith()
{
    std::string str = lua_tostring(lua_state, 1);
    std::string start = lua_tostring(lua_state, 2);

    lua_pushboolean(lua_state, str.starts_with(start));
    return 1;
}
