#include "Zsh.h"

int main(int argc, char **argv, char **envp){
  (void)argc; 
  (void)argv; 
  
    int exit_status = 0; 
    shell_t shell; 
    if(initialise_shell(&shell, envp) != 0){perror("Shell initialisation failed\n"); exit_status = 1;} 
    exit_status = shell_loop(&shell); 
    if(exit_status != 0){goto cleanup;}


  cleanup: 
  return exit_status; 

}