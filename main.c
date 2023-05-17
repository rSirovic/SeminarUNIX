#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_ls(char **args);
int lsh_mv(char **args);
int lsh_pwd(char **args);


char *builtin_str[] = {
  "mycd",
  "myhelp",
  "myexit",
  "myls",
  "mymv",
  "mypwd"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_ls,
  &lsh_mv,
  &lsh_pwd
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

<<<<<<< HEAD
/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
=======
>>>>>>> 5e647fedffe9c0b6dbe589311a149e0333b6671b
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}


int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}


int lsh_exit(char **args)
{
  return 0;
}

<<<<<<< HEAD
int lsh_ls(char **args)
{
	DIR *dp = NULL; 
        struct dirent *dptr = NULL; 
        unsigned int count = 0; 
  
        if(NULL == args[1]) 
	{ 
        dp = opendir(".");
   	}
	else 
    	{
        dp = opendir(args[1]);
   	}
  	if(NULL == dp) 
  	{ 
     	   printf("\n ERROR : Could not open.\n");
 	   return 1;
   	} 
   
        for(count = 0; NULL != (dptr = readdir(dp)); count++) 
        { 
       		if(dptr->d_name[0] != '.') 
       		{
                	printf("%s     ",dptr->d_name); 
        	} 
        } 
        printf("\n"); 
        closedir(dp);
 
        return 1; 
}

int lsh_mv(char **args)
{
        DIR* dir_ptr;
        struct dirent* direntp;

        if( args[2] == NULL )
        {
        	printf("Usage:  %s MOVE\n", args[0] );
      		return 1;
        }

        if( args[1] == NULL &&  args[2] == NULL)
        {
   	     printf("Error! Few arguments provided.\n" );
  	      return 1;
        }

        char src_folder[256];
        char dest_folder[256];
        strcpy(src_folder, args[1]);
        strcpy(dest_folder, args[2]);

        dir_ptr = opendir(".");
        if (dir_ptr == NULL)
        {
    	    perror( "." );
   	     return 1;
        }

        while((direntp = readdir(dir_ptr)) != NULL ) 
        {
   		if (strcmp(direntp->d_name, dest_folder) !=0)
                {
            	 	break;
                }
		else
                	printf("Not found.\n");
        break;
        }
        rename(src_folder, dest_folder);
        closedir(dir_ptr);

        return 1;
}

int lsh_pwd(char **args)
{ 
        char cwd[1024]; 
        getcwd(cwd, sizeof(cwd)); 
        printf("%s\n", cwd);
        return 1; 
}




/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
=======

>>>>>>> 5e647fedffe9c0b6dbe589311a149e0333b6671b
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}


char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We received an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}


int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
