#define NOB_IMPLEMENTATION
#include "nob.h"

#include <string.h>
#include <stdio.h>

int add_compile_command(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd,
        "cc",
        "-Wall",
        "-Wextra"
    );
    nob_cmd_append(cmd,
        "-I:./includes"
    );
    nob_cmd_append(cmd, 
        "-o",
        "main",
        "main.c"
    );

    nob_cmd_append(cmd, "-lGL");
    nob_cmd_append(cmd, "-lglfw");
    nob_cmd_append(cmd, "-lGLEW");
    nob_cmd_append(cmd, "-lm");
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};

    if (argc > 1) {
        nob_shift(argv, argc);
        const char *arg = nob_shift(argv, argc);
        
        size_t len = strlen(arg);
        char command = arg[0];
         
        
        if (command == 'c') {
            add_compile_command(&cmd);
            if (!nob_cmd_run(&cmd)) return 1;
            if (len > 1) {
                command = arg[1];
            }
        }

        if (command == 'r') {
            nob_cmd_append(&cmd, "./main");
            if (!nob_cmd_run(&cmd)) return 1;
        }
    }

    return 0;
}
