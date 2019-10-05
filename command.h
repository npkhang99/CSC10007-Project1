#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils.h"

class command {
private:
    std::string raw_command;
    std::vector<std::string> _cmd;
    std::vector<std::vector<std::string>> _args;
    int _size;

    bool _have_io_redirect() const {
        int input_redirect = -1;
        int output_redirect = -1;
        for (int i = 0; i < _args[0].size(); i++) {
            if (_args[0][i] == "<") {
                assert(input_redirect == -1);
                input_redirect = i;
            }
            if (_args[0][i] == ">") {
                assert(output_redirect == -1);
                output_redirect = i;
            }
        }
        return input_redirect != -1 || output_redirect != -1;
    }

    std::vector<std::string> _get_io_redirect_files() const {
        int input_redirect = -1;
        int output_redirect = -1;
        for (int i = 0; i < _args[0].size(); i++) {
            if (_args[0][i] == "<") {
                input_redirect = i;
            }
            if (_args[0][i] == ">") {
                output_redirect = i;
            }
        }

        std::string input_file_name = (input_redirect != -1)? _args[0][input_redirect + 1] : "";
        std::string output_file_name = (output_redirect != -1)? _args[0][output_redirect + 1] : "";
        
        std::vector<std::string> io_files;
        io_files.push_back(input_file_name);
        io_files.push_back(output_file_name);

        return io_files;
    }

    int _execute(int cmd_id) const {
        if (_have_io_redirect()) {
            std::vector<std::string> io_file_names = _get_io_redirect_files();
            int in = -1, out = -1;
            if (io_file_names[0].length() > 0) {
                in = open(io_file_names[0].c_str(), O_RDONLY);
                assert(in != -1);
                dup2(in, STDIN_FILENO);
                close(in);
            }
            if (io_file_names[1].length() > 0) {
                out = open(io_file_names[1].c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                assert(out != -1);
                dup2(out, STDOUT_FILENO);
                close(out);
            }
        }

        std::vector<std::vector<std::string>> args = _args;
        for (int i = 0; i < args[cmd_id].size(); i++) {
            while (i < (int) args[cmd_id].size() && (args[cmd_id][i] == ">" || args[cmd_id][i] == "<")) {
                args[cmd_id].erase(args[cmd_id].begin() + i, args[cmd_id].begin() + i + 2);
            }
        }

        // std::cerr << fmt_to_string("Executing: %s\n", join(args[cmd_id]).c_str());

        if (args[cmd_id].back() == "&") {
            args[cmd_id].pop_back();
            return execvp(args[cmd_id][0].c_str(), to_char_arr(args[cmd_id]));
        }

        return execvp(args[cmd_id][0].c_str(), to_char_arr(args[cmd_id]));
    }

public:
    command(const std::string& raw) {
        _size = 0;
        _cmd.clear();
        _args.clear();

        raw_command = raw;
        if (raw.find("!!") != std::string::npos && trim(raw).size() != 2) {
            throw "History command must be a stand-alone command";
        }

        std::vector<std::string> parts = split(raw, '|');
        for (std::string cmd : parts) {
            std::vector<std::string> args = split(trim(cmd));
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

    bool is_background_process() const {
        return _args[0].back() == "&";
    }

    int execute() const {
        if (_size == 1) {
            return _execute(0);
        }
        else {
            int pipefd[2]; // pipefd[0] = output của lệnh 1 | pipefd[1] = input của lệnh 2
            pipe(pipefd);
            pid_t pid = fork();
            if (pid == 0) {
                dup2(pipefd[0], 0);
                // chỉ cần pipe output nên đóng pipe input
                close(pipefd[1]);
                _execute(1);
            }
            else {
                dup2(pipefd[1], 1);
                // chỉ cần pipe input nên đóng pipe output
                close(pipefd[0]);
                _execute(0);
            }
        }
    }

    bool have_io_redirect() const {
        return _have_io_redirect();
    }
};

#endif // __COMMAND_H__
