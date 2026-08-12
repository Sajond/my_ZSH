#include "Zsh.h"

int main(int argc, char **argv, char **envp){

    shell_t shell; 
    if(initialise_shell(&shell, envp) != 0){printf("Shell initialisation failed\n"); goto cleanup;} // not sure if cleanup path is needed


    cleanup: 
}