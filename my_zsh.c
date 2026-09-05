#include "Zsh.h"

// ------------------------------------------------------------------------------------------------ BUILTIN ARRAY ----------------------------------------------------------------------------------------------
builtin_t builtins_list[]= {
    {"echo", builtin_echo}, 
    {"exit", builtin_exit},
    {"quit", builtin_exit},
    {"env", builtin_env},   
    {"pwd", builtin_pwd},
    {"which", builtin_which},
    {"cd", builtin_cd},
    {"setenv", builtin_setenv},
    {"unsetenv", builtin_unsetenv},
    {NULL, NULL}
}; 

// ------------------------------------------------------------------------------------------------ CONTROL FUNCTIONS ----------------------------------------------------------------------------------------------

 int initialise_shell(shell_t *shell, char **envp){
    shell->shell_envp = copy_env(envp); 
    if(shell->shell_envp != NULL){
        shell->running = 1; 
        shell->builtins = builtins_list;
        shell->previous_dir = NULL; 
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

        tokenise_input(&token_count, argv, line);        // line is valid - > parse

       if(token_count > 0){
        execute_command(argv, builtins_list, token_count, shell); 
       }
 
    }
    cleanup:     //on break free the shell and exit
    free(line); 
    free_env(shell->shell_envp); 
    free(argv); 
    return exit_status; 
}
void execute_command(char **argv, builtin_t *builtins_list, int token_count, shell_t *shell ){
    int index; 
    if((index = exists_as_builtin(argv, builtins_list)) != -1){
        builtins_list[index].function(token_count, argv, shell);

    } else{ 
        char *programme_path; 

        if((programme_path = find_programme_path(shell, argv)) != NULL){
            if(execute_programme_path(programme_path, argv, shell) != 0){
                perror("Error with programme path execution\n"); 
            }      
        } else {perror("");}

        free(programme_path); 
    }
}
// ------------------------------------------------------------------------------------------------ FUNCTIONS ----------------------------------------------------------------------------------------------

//! TECHNICALLY STRLEN AND STRCPY are not allowed but i dont see the point, it just makes the project unecessarily repetitive. 
char **copy_env(char **envp){
    int count = 0;  
     for(int i = 0; envp[i] != NULL; i++){
        count++; 
    } 

    char **new_env = malloc(sizeof(char *) * (count + 1)); 
    if(new_env == NULL){perror("malloc for array pointers failed\n"); return NULL;} 
    new_env[count] = NULL; 
    
    for(int i = 0; envp[i] != NULL; i++){
        new_env[i] = malloc(sizeof(char) * (strlen(envp[i]) + 1)); 
        if(new_env[i] == NULL){perror("Malloc for env variable failed\n"); free_env(new_env); return NULL;} 
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
            //file exists & is an executable file.
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

char **prefix_search(shell_t *shell, char *prefix){
 int i = 0; 

    while(shell->shell_envp[i] != NULL){ 
        if(strncmp(shell->shell_envp[i], prefix, strlen(prefix)) == 0 
        && (shell->shell_envp[i][strlen(prefix)] == '=')){
            return &shell->shell_envp[i];
            
        }else { 
            i++; 
        }
    }
    return NULL; 
}

char **setenv_prefix_search(shell_t *shell, char **argv){ 
    int count = 0; 
    for(int i = 0; argv[1][i] != '=' && argv[1][i] != '\0'; i++){ 
        count++; 
    }

    if(argv[1][count] == '\0'){
        write(2, "setenv: expected NAME=VALUE\n", 28); 
        return NULL; 
    }

    int i = 0; 
    while(shell->shell_envp[i] != NULL){
        if(strncmp(shell->shell_envp[i], argv[1], count) == 0 && (shell->shell_envp[i][count] == '=')){
            return &shell->shell_envp[i]; 
        } else{ 
            i++; 
        }
    }
    return NULL; 
}

void copy_replacement_string(char **argv, char *replacement_string){ 
    strncpy(replacement_string, argv[1], strlen(argv[1])); 
    replacement_string[strlen(argv[1])] = '\0'; 
}

char **reallocate_env(char **current_env, char *new_string){
    int count = 0;  
    for(int i = 0; current_env[i] != NULL; i++){
       count++; 
   } 

   char **new_env = malloc(sizeof(char *) * (count + 2)); // + 2 to account for the new variable being added and NULL. 
   if(new_env == NULL){perror("malloc for array pointers failed\n"); return NULL;} 

   for(int i = 0; i < count; i++){
    new_env[i] = current_env[i]; 
   }
   new_env[count] = new_string; 
   new_env[count + 1] = NULL; 

   return new_env; 
}

void std_echo_out(char **argv){ 
    for(int i = 1; argv[i] != NULL; i++){ 
        write(1, argv[i], strlen(argv[i])); 
        if(argv[i + 1] != NULL){ 
            write(1, " ", 1); 
        }
    }
    write(1, "\n", 1); 
}
// ------------------------------------------------------------------------------------------------ BUILTIN DECLARATIONS ----------------------------------------------------------------------------------------------
//! USES STRLEN FUNCTION
int builtin_echo(int argc, char **argv, shell_t *shell){
    (void)argc;
    (void)shell; 
    
    int i = 0; 
    if(argv[1][i] == '$'){
        
        char **slot; 
        if((slot = prefix_search(shell, argv[1] + 1)) != NULL){
            while((*slot)[i] != '=' && (*slot)[i] != '\0'){
                i++; 
            }
            char *value = *slot + i + 1; 
            write(1, value, strlen(value)); 
            write(1, "\n", 1);
            return 0; 
        }
        write(1, "\n", 1); 
        return 0; 
    }
    std_echo_out(argv); 
    
    return 0; 
}; 

int builtin_exit(int argc, char **argv, shell_t *shell){
    (void)argv; 
    if (argc != 1) {
        write(2, "exit: no arguments expected\n", 28);
        return 1;
    }
    shell->running = 0; 
    return 0; 
}; 
//!USES STRLEN
int builtin_pwd(int argc, char **argv, shell_t *shell){ 
    (void)argv; (void)shell; 
    if (argc != 1) {
        write(2, "pwd: no arguments expected\n", 27);
        return 1;
    }

    char *working_dir = getcwd(NULL, 0);
    if(working_dir == NULL){return 1;}
    write(1, working_dir, strlen(working_dir));
    write(1, "\n", 1);  
    free(working_dir); 
    return 0; 
}

int builtin_which(int argc, char **argv, shell_t *shell){
    if (argc != 2) {write(2, "which: expected NAME\n", 21);return 1;}
    int status = 0; 
    //new argv given to allow functionality of find programme functioin, else would read the command as part of the path search.    
    char **temp_argv = malloc(sizeof(char *) * 2); 
    if(temp_argv == NULL){perror("'Which malloc failed'\n"); return 1;}
    temp_argv[0] = argv[1]; 
    temp_argv[1] = NULL; 

    char *path = NULL; 
    if ((path = find_programme_path(shell, temp_argv)) != NULL){
        write(1, path, strlen(path)); 
        write(1, "\n", 1); 
        free(path); 

    } else {
        status = 1;
    }
    free(temp_argv); 
    return status; 
}
//!USES STRLEN
int builtin_env(int argc, char **argv, shell_t *shell){
    (void)argv;
    if (argc != 1) {
        write(2, "env: no arguments expected\n", 27);
        return 1;
    }
    int i = 0;
    while(shell->shell_envp[i] != NULL){
        write(1, shell->shell_envp[i], strlen(shell->shell_envp[i])); 
        write(1, "\n", 1); 
        i++; 
    }
    return 0; 
}; 

//todo: handle the "cd -" command where it returns to the previous directory
int builtin_cd(int argc, char **argv, shell_t *shell){
    (void)shell;
    if (argc > 2) {write(2, "cd: too many arguments\n", 23); return 1;}

    //? set the current dir so that we can change easily
    char* current_dir; 
    if(getcwd(current_dir, strlen(current_dir) == NULL)){perror(""); return 1;}

    if(argv[1] == NULL){
       char *path = home_search(shell); 
       if(path == NULL){write(2, "HOME not set\n", 13); return 1;}

       if(chdir(path) != 0){
        perror(""); 
        return 1; 

       } 
       //add else branch for getCWD/ set prev dir

    } else if((chdir(argv[1]) != 0)){
        perror(""); 
        return 1; 
    } 
    //add else branch for getCWD/ set prev dir 
    return 0; 
}; 
//! strncmp
char *home_search(shell_t *shell){
    int i = 0; char *path; 
    while(shell->shell_envp[i] != NULL){
        if(strncmp(shell->shell_envp[i], HOME_PREFIX, 5) != 0){
            i++; 
        } else { 
            path = shell->shell_envp[i] + 5; 
            return path; 
        }
    }
    return NULL; 
}; 


int builtin_setenv(int argc, char **argv, shell_t *shell){
    if(argc != 2){write(2, "setenv: expected NAME=VALUE\n", 28); return 1;}
    char *new_string = malloc((strlen(argv[1]) + 1)); 
    if(new_string == NULL){perror("Setenv malloc\n"); return 1;}
    copy_replacement_string(argv, new_string); //build e.g FOO + = + newstringvalue

    char **slot; 
    //*slot = char pointing to PREFIX=
    if((slot = setenv_prefix_search(shell, argv)) != NULL){ //address of shell slot
        free(*slot); 
        *slot = new_string;
        return 0;
        
    } else { 
        char **new_env = reallocate_env(shell->shell_envp, new_string); 
        if(new_env == NULL){return 1;}
        free(shell->shell_envp); 
        shell->shell_envp = new_env; 
        return 0; 
    }
}; 
//!USES STRLEN

int builtin_unsetenv(int argc, char **argv, shell_t *shell){
    if(argc != 2){write(2, "unsetenv: expected NAME\n", 25); return 1;}
    char **slot; 
    if((slot = prefix_search(shell, argv[1])) != NULL){
        free(*slot); 
        while(*(slot + 1) != NULL){
            *slot = *(slot + 1); 
            slot++; 
        }
        *slot = NULL; 
        return 0; 
    };

    write(2, "unsetenv: variable not found\n", 29);
    return 1;
}; 

