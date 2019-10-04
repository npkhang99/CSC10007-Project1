#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

#include "utils.h"
#include "config.h"
#include "command.h"

int main() {
    bool loop = true;
    command history;

    while (loop) {
        cout << "khang@osh> ";
        cout.flush();
        string raw_cmd;
        getline(cin, raw_cmd);
        if (is_exit(raw_cmd)) {
            break;
        }

        pid_t pid = fork();
        pid_t wait_pid;
        if (pid == 0) {
            command cmd(raw_cmd);
            bool ok_to_execute = true;
            if (cmd.is_history_command()) {
                // history rỗng đồng nghĩa với size của history bằng 0
                if (history.size() == 0) {
                    cout << "History is empty!" << endl;
                    ok_to_execute = false;
                    break;
                }
                else {
                    cmd = history;
                    cout << cmd.to_string().c_str() << endl;
                }
            }

            if (ok_to_execute) {
                exit(cmd.execute());
            }
        }
        else {
            if (!command(raw_cmd).is_background_process()) {
                wait_pid = wait(NULL);
            }

            if (!command(raw_cmd).is_history_command()) {
                history = command(raw_cmd);
            }
        }
    }
    return 0;
}
