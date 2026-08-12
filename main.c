#include "Zsh.h"

int main(int argc, char **argv, char **envp){

    shell_t shell; 
    if(initialise_shell(&shell, envp) != 0){perror("Shell initialisation failed\n"); return 1;} 


  cleanup: 
}