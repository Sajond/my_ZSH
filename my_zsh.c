#include "Zsh.h"

// ------------------------------------------------------------------------------------------------ BUILTIN ARRAY ----------------------------------------------------------------------------------------------
builtin_t builtins_list[]= {
    {"echo", builtin_echo}, 
    //?DEBUG
    /*
    {"cd", builtin_cd},
    {"setenv", builtin_setenv},
    {"unsetenv", builtin_unsetenv},
    {"env", builtin_env},
    {"exit", builtin_exit},
    {"pwd", builtin_pwd},
    {"which", builtin_which},
    */
    {NULL, NULL}
}; 

// ------------------------------------------------------------------------------------------------ CONTROL FUNCTIONS ----------------------------------------------------------------------------------------------

 int initialise_shell(shell_t *shell, char **envp){
    shell->shell_envp = copy_env(envp); 
    if(shell->shell_envp != NULL){
        shell->running = 1; 
        shell->builtins = builtins_list;
        //?DEBUG 
        printf("Shell intiialised\n"); 
        return 0; 
    } 
    return 1;   
} // shell is ready for main loop

int shell_loop(shell_t *shell){
    int exit_status = 0; int token_count; char **argv; char *line = NULL; size_t size = 0; // removed for compile test: char *path; 

    argv = malloc(sizeof(char *) * (MAX_ARGS + 1) ); 
    if(argv == NULL){perror("Malloc for argv failed\n"); exit_status = 1; goto cleanup;}

    while(shell->running){
        if(print_base_prompt() != 0){perror("Could not get CWD: Fatal error"); exit_status = 1; break;} // no cwd present

        if(getline(&line, &size, stdin) == -1){perror("Failed to get line from stdin"); exit_status = 1; break;}

        //?DEBUG 
        //printf("received line: %s\n", line); 
        //printf("line size is: %zu\n", size); 
       
        tokenise_input(&token_count, argv, line);        // line is valid - > parse

        /*
        //?DEBUG token print
        for(int i = 0; argv[i] != NULL; i++){
            printf("argv [%d] = %s\n", i, argv[i]); 
            if(argv[i + 1] == NULL){printf("argv [%d] = NULL\n", i + 1);}
        }
        */
        
       if(token_count > 0){
        check_function_type(argv, builtins_list, token_count, shell); 
       }
 
        //?DEBUG
        //printf("Reached end of iteration, waiting for next input\n");
    }
    cleanup:     //on break free the shell and exit
    free(line); 
    free_env(shell->shell_envp); 
    free(argv); 
    return exit_status; 
}
//TODO to finish this and programme path finding. 
void check_function_type(char **argv, builtin_t *builtins_list, int token_count, shell_t *shell ){
    int index; 
    if((index = exists_as_builtin(argv, builtins_list)) != -1){
        builtins_list[index].function(token_count, argv, shell);
        //DEBUG
        printf("DEBUG: programme executed as builtin\n"); 
    } else{ 
        //todo NOT FULL ALLOCATION, to be finished
        char *programme_path; 

        if((programme_path = find_programme_path(shell, argv)) != NULL){
            //todo Execute programme -> using fork and execve
            if(execute_programme_path(programme_path, argv, shell) != 0){
                perror("Error with programme path execution\n"); 
            }
            
        }
        perror(""); 
        //else loop continues 
        free(programme_path); 
    }
   
//bebug prints
exit(EXIT_SUCCESS); //!REMOVE THIS KILLS SHELL
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
//!USES STRCMP
int exists_as_builtin(char **argv, builtin_t *builtins_list){

    int i = 0; 
    while(builtins_list[i].name != NULL){
        if(strcmp(argv[0], builtins_list[i].name) == 0){
            return i;
        } else {
            i++; 
        }
    }
    return -1; 
}
//!Uses STRCPY & STRLEN
char *find_programme_path(shell_t *shell, char**argv){
    int i = 0; 
    while(shell->shell_envp[i] != NULL){
        //for every string at position I, check the beginning to see if it matches PATH=
        if(strncmp(shell->shell_envp[i], PATH_PREFIX, 5) != 0){
            i++;} 
        else{
           char *result = search_path(shell, argv, i); 
           return result; 
        }   
    }
    printf("DEBUG: programme doesnt exist\n");
    return NULL;
}
//! REPLACE WITH NON STRNCAT VERSION 
void build_full_path(char *full_path, char *directory, char **argv){
    full_path[0] = '\0'; 
    strncat(full_path, directory, strlen(directory)); 
    strcat(full_path, "/"); 
    strncat(full_path, argv[0], strlen(argv[0])); 
}
//!STRLEN 
char *search_path(shell_t *shell, char **argv, int i){
    char *path_string = malloc(strlen(shell->shell_envp[i]) + 1);
        if(path_string == NULL){perror("Malloc failed\n"); return NULL;}
        strcpy(path_string, shell->shell_envp[i]);
        char *directories = path_string + 5; 
        char *directory = strtok(directories, ":"); 

        while(directory != NULL){
            char *result = search_directory(directory, argv); 
            if(result != NULL){
                free(path_string); 
                return result; 
            }
            directory = strtok(NULL, ":"); 
        }
      free(path_string); 
      return NULL; 
}
//!STRLEN
char *search_directory(char *directory, char **argv){
    char *full_path = malloc(strlen(directory) + strlen(argv[0]) + 2); 
    if(full_path == NULL){perror("Fullpath malloc failed\n"); return NULL;}
    
    build_full_path(full_path, directory, argv); 

         struct stat file_info; 

        if(stat(full_path, &file_info) == 0 && S_ISREG(file_info.st_mode) && (file_info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))){
            //file exists & is an executable file 
            printf("DEBUG: PATH executable found\n"); 
            return full_path; 

            } else { 
            free(full_path); 
            }

    return NULL; 
}

int execute_programme_path(char *programme_path, char **argv, shell_t *shell){
int status; 
pid_t pid = fork(); 

if(pid > 0){
    //parent
    waitpid(pid, &status, 0); 
    if(WIFEXITED(status)){
        if(WEXITSTATUS(status) != 0 ){
            return 1; 
        }
        return 0; 

    } else if(WIFSIGNALED(status)){
        return 1;
    }
 
} else if(pid == 0){
    //child
    if(execve(programme_path, argv, shell->shell_envp) == -1){
        perror("execve\n"); 
        exit(1); 
    }; 
}
perror("Fork failed\n"); 
return 1; 

}
// ------------------------------------------------------------------------------------------------ BUILTIN DECLARATIONS ----------------------------------------------------------------------------------------------
//! USES STRLEN FUNCTION
int builtin_echo(int argc, char **argv, shell_t *shell){
    (void)argc;
    (void)shell; 

    for (int i = 1; argv[i] != NULL; i++){
            write(1, argv[i], strlen(argv[i])); 

            if(argv[i + 1] != NULL){
                write(1, " ", 1); 
            }
    }
    write(1,"\n", 1); 
    
    return 0; 
}; 

/*
builtin_cd(){}; 
builtin_setenv(){}; 
builtin_unsetenv(){}; 
builtin_env(){}; 
builtin_exit(){}; 
builtin_pwd(){}; 
builtin_which(){}; 
*/

//TODO: Replace string functions with own version? 
//TODO: strncat needs to be replaced with custom version. 