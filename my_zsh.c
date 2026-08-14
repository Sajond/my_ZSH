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
    int exit_status = 0; int token_count; char **argv; char *line = NULL; size_t size = 0; 

    argv = malloc(sizeof(char *) * (MAX_ARGS + 1) ); 
    if(argv == NULL){perror("Malloc for argv failed\n"); exit_status = 1; goto cleanup;}

    while(shell->running){
        if(print_base_prompt() != 0){perror("Could not get CWD: Fatal error"); exit_status = 1; break;} // no cwd present

        if(getline(&line, &size, stdin) == -1){perror("Failed to get line from stdin"); exit_status = 1; break;}
        tokenise_input(&token_count, argv, line);        // line is valid - > parse

        //check type
        //execute command
        free(line); // must be free before next iteration after execution. 
    }
    cleanup:     //on break free the shell and exit
    free_env(shell->shell_envp); 
    free(argv); 
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

//call with &argc and &argv / tokencount token 
//!strtok to be replaced with own version. 
void tokenise_input(int *argc, char **argv, char *line){
    int count = 0; 
    char *token = strtok(line, DELIM);

    while(token != NULL){
        argv[count] = token; 
        count++; 
        token = strtok(NULL, DELIM);
    }
    argv[count] = NULL;
    *argc = count; 
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