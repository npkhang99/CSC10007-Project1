#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>

#include "utils.h"

class command {
private:
    std::vector<std::string> _cmd;
    std::vector<std::vector<std::string>> _args;
    int _size;

    int _execute(int i) const {
        return execvp(_cmd[i].c_str(), to_char_arr(_args[i]));
    }

public:
    command(const std::string& raw) {
        _size = 0;
        _cmd.clear();
        _args.clear();

        if (raw.find("!!") != std::string::npos && trim(raw).size() != 2) {
            throw "History command must be a stand-alone command";
        }

        std::vector<std::string> parts = split(raw, '|');
        for (std::string cmd : parts) {
            std::vector<std::string> args = split(cmd);
            _cmd.push_back(args[0]);
            _args.push_back(args);
            _size++;
        }
    }

    command() {
        _size = 0;
        _cmd.clear();
        _args.clear();
    }

    command& operator=(const command& o) {
        clear();
        _cmd = o._cmd;
        _args = o._args;
        _size = o._size;
        return *this;
    }

    void clear() {
        _size = 0;
        _cmd.clear();
        _args.clear();
    }

    int size() const {
        return _size;
    }

    std::string to_string() const {
        std::string cmd_str = "";
        for (int i = 0; i < _size; i++) {
            cmd_str = join(_args[i]);
            if (i < _size - 1) {
                cmd_str += " | ";
            }
        }
        return cmd_str;
    }

    std::string to_string(const int& i) const {
        assert(0 <= i && i < _size);
        return join(_args[i]);
    }

    bool is_history_command() const {
        return _cmd[0] == "!!";
    }

    int execute() const {
        if (_size == 1) {
            return _execute(0);
        }
        else {
            return _execute(0);
        }
    }
};

#endif // __COMMAND_H__
