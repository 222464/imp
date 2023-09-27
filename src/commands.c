#include "commands.h"

void imp_command_list_clear(imp_CommandList command_list) {
    command_list.num_commands = 0;
}

b32 imp_command_list_add(imp_CommandList command_list, imp_Command command) {
    if (command_list.num_commands < IMP_MAX_COMMANDS) {
        command_list.commands[command_list.num_commands] = command;
        command_list.num_commands++;

        // Added command successfully
        return true;
    }

    return false;
}
