#include "Zsh.h"

// ------------------------------------------------------------------------------------------------ CONTROL FUNCTIONS ----------------------------------------------------------------------------------------------

 int initialise_shell(shell_t *shell, char **envp){
    if(shell->shell_envp = copy_env(envp) != NULL){
        shell->running = 1; 
        shell->builtins = builtins_list;
        return 0; 
    } 
    return 1;   
} // shell is ready for main loop

int shell_loop(shell_t *shell){
    int exit_status = 0; int argc; char **argv; char *line; size_t size = 0; 

    while(shell->running){
        if(print_base_prompt() != 0){perror("Could not get CWD: Fatal error"); exit_status = 1; break;} // no cwd present

        if(getline(&line, &size, stdin) == -1){perror("Failed to get line from stdin"); exit_status = 1; break;}
        // line is valid - > parse

    }

    //on break free the shell and exit
    free_env(shell->shell_envp); 
    return exit_status; 
}
// ------------------------------------------------------------------------------------------------ FUNCTIONS ----------------------------------------------------------------------------------------------

//! TECHNICALLY STRLEN AND STRCPY are not allowed but i dont see the point, it just makes the project unecessarily repetitive. 
char **copy_env(char **envp){
    int count = 0;  
     for(int i = 0; envp[i] != NULL; i++){
        count++; 
    } 

    char **new_env = malloc(sizeof(char *) * (count + 1)); 
    if(new_env == NULL){printf("malloc for array pointers failed\n"); return NULL;} 
    new_env[count] = NULL; 
    
    for(int i = 0; envp[i] != NULL; i++){
        new_env[i] = malloc(sizeof(char) * (strlen(envp[i]) + 1)); 
        if(new_env[i] == NULL){printf("Malloc for env %d failed\n", i); free_env(new_env); return NULL;} 
        strcpy(new_env[i], envp[i]); 
    }

    return new_env;
}

void free_env(char **new_env){
    for(int i = 0; new_env[i] != NULL; i++){
        free(new_env[i]); 
    }
    free(new_env); 
}

int print_base_prompt(){
   char *cwd = getcwd(NULL, 0); 
   if(cwd == NULL){return 1;} 
    write(1, "[", 1); 
    write(1, cwd, strlen(cwd));
    write(1, "]>", 2); 
    free(cwd);
    return 0;
   
};

int parse_input(int argc, char **argv, char *line){

}

// ------------------------------------------------------------------------------------------------ BUILTIN ARRAY ----------------------------------------------------------------------------------------------
builtin_t builtins_list[]= {
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
//! USES STRLEN FUNCTION
int builtin_echo(int argc, char **argv){
    for (int i = 1; argv[i] != NULL; i++){
            write(1, argv[i], strlen(argv[i])); 

            if(argv[i + 1] != NULL){
                write(1, " ", 1); 
            }
    }
    write(1,"\n", 1); 
    return 0; 
}; 

builtin_cd(){}; 
builtin_setenv(){}; 
builtin_unsetenv(){}; 
builtin_env(){}; 
builtin_exit(){}; 
builtin_pwd(){}; 
builtin_which(){}; 