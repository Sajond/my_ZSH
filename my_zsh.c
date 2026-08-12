#include "Zsh.h"

// ------------------------------------------------------------------------------------------------ CONTROL FUNCTIONS ----------------------------------------------------------------------------------------------

 int initialise_shell(shell_t *shell, char **envp){

    //  # function 1 ->  copy env to shell 

    // # function/job 2 set running 

    // # function 3 initialise Builtin list (8 entries + 1 for NULL end)
    return 0; 
} // shell is ready for main loop

// ------------------------------------------------------------------------------------------------ BUILTIN ARRAY ----------------------------------------------------------------------------------------------
builtin_t builtins[]= {
    {"echo", builtin_echo}, 
    {"cd", builtin_cd},
    {"setenv", builtin_setenv},
    {"unsetenv", builtin_unsetenv},
    {"env", builtin_env},
    {"exit", builtin_exit},
    {"pwd", builtin_pwd},
    {"which", builtin_which},
    {NULL, NULL}
}; 

// ------------------------------------------------------------------------------------------------ BUILTIN DECLARATIONS ----------------------------------------------------------------------------------------------

builtin_echo(){}; 
builtin_cd(){}; 
builtin_setenv(){}; 
builtin_unsetenv(){}; 
builtin_env(){}; 
builtin_exit(){}; 
builtin_pwd(){}; 
builtin_which(){}; 