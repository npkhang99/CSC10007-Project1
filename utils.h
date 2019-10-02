#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

std::string fmt_to_string(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return std::string(buffer);
}

std::string trim(std::string st) {
    while (st.length() > 0 && isspace(st[0])) {
        st.erase(st.begin());
    }

    while (st.length() > 0 && isspace(st.back())) {
        st.pop_back();
    }

    return st;
}

std::vector<std::string> split(const std::string& st, char delimiter = ' ') {
    std::vector<std::string> splts;
    std::string tmp = "";
    for (int i = 0; i < st.length(); i++) {
        if (st[i] != delimiter) {
            tmp += st[i];
        }
        else {
            splts.push_back(trim(tmp));
            tmp = "";
        }
    }
    splts.push_back(trim(tmp));
    return splts;
}

char** to_char_arr(std::vector<std::string> v) {
    char** a;
    a = (char**) malloc(v.size() * sizeof(char*));
    for (int i = 0; i < v.size(); i++) {
        a[i] = strdup(v[i].c_str());
    }
    return a;
}

std::string join(const std::vector<std::string>& v, const std::string& seperator = " ") {
    std::string st = "";
    for (int i = 0; i < (int) v.size(); i++) {
        st += v[i];
        if (i < (int) v.size() - 1) {
            st += seperator;
        }
    }
    return trim(st);
}

bool is_exit(string command) {
    return trim(command) == "exit";
}

#endif // __UTILS_H__
