#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

/* Builtin command descriptor
 * defines a type which is a pointer to a function that takes argc and argvv (as most builtins will) args will be defined by the parser. 
 */
typedef int (*builtin_func_t)(int argc, char **argv);

typedef struct {
	const char *name;         /* builtin name, e.g. "cd" */
	builtin_func_t function;  /* callable as builtin[i].function */
} builtin_t;

/* Example declaration (define the array in a .c file):
 * builtin_t builtins[] = { {"cd", builtin_cd}, {"exit", builtin_exit} };
 */

 typedef struct
 {
	int running; 
	char **shell_envp; 
	builtin_t *builtins; 

 } shell_t;
 

// ----------------------------------------------------------- FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------------
int initialise_shell(shell_t *shell, char **envp); 
int shell_loop(shell_t *shell); 
char **copy_env(char **envp); 
void free_env(char **new_env); 
int print_base_prompt(); 

// ----------------------------------------------------------- BUILTIN DECLARATIONS ----------------------------------------------------------------------------------------------
int builtin_echo(int argc, char **argv);

