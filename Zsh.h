#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>

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

// ----------------------------------------------------------- FUNCTION DECLARATIONS ----------------------------------------------------------------------------------------------
void initialise_shell(); 


