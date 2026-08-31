#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define DELIM " \t\n"
#define MAX_ARGS 100 //used to avoid double pass in tokeniser (100 args is reasonable for a shell)
#define PATH_PREFIX "PATH="

/* Builtin command descriptor
 * defines a type which is a pointer to a function that takes argc and argvv (as most builtins will) args will be defined by the parser. 
 */

typedef struct shell shell_t;
typedef struct builtin builtin_t;

typedef int (*builtin_func_t)(int argc, char **argv, shell_t *shell);

typedef struct builtin {
	const char *name;         /* builtin name, e.g. "cd" */
	builtin_func_t function;  /* callable as builtin[i].function */
} builtin_t;

 typedef struct shell{
	int running; 
	char **shell_envp; 
	builtin_t *builtins; 
 } shell_t;

/* Example declaration (define the array in a .c file):
 * builtin_t builtins[] = { {"cd", builtin_cd}, {"exit", builtin_exit} };
 */
// ----------------------------------------------------------- FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------------
int initialise_shell(shell_t *shell, char **envp); 
int shell_loop(shell_t *shell); 
char **copy_env(char **envp); 
void free_env(char **new_env); 
int print_base_prompt(); 
void tokenise_input(int *argc, char **argv, char *line); 
void execute_command(char **argv, builtin_t *builtins_list, int token_count, shell_t *shell ); 
int exists_as_builtin(char **argv, builtin_t *builtins_list); 
char *find_programme_path(shell_t *shell, char**argv);
void build_full_path(char *full_path, char *directory, char **argv); 
char *search_path(shell_t *shell, char **argv, int i);
char *search_directory(char *directory, char **argv); 
int execute_programme_path(char *programme_path, char **argv, shell_t *shell); 


// ----------------------------------------------------------- BUILTIN DECLARATIONS ----------------------------------------------------------------------------------------------
int builtin_echo(int argc, char **argv, shell_t *shell);

