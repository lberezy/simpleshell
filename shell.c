#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>
#include <libc.h>

#define DEBUG 0

#define PS "\x1b[32mshell~>\x1b[0m" /* default prompt statement */
#define BUFFER_LEN 256


char **tokenizer(char *str);
void str_array_free(char **str_array);
int str_char_count(char *str, char cmp);
int m_strcpy_slice(char *dst, char *src, int n);
void m_strcpy(char *dst, char *src);
int m_strcmp(char *a, char *b);
int m_strlen(char *str);


char **tokenizer(char *str){
  char *cur = str;
  char delimiter = ' ';
  int num_tokens = str_char_count(str, delimiter);
  int cur_num = 0;
  char **str_array = malloc((size_t)sizeof(char*) * (size_t)(num_tokens+2));
  /* should you check malloc ?  - no*/
  char *prev = str;
  while (*cur) {
    if (!*(cur+1)) {
      cur++;
      *cur = delimiter;
    }
    if (*cur == delimiter) {
      char *len = (char*)(cur-prev);
      char *token =  malloc(sizeof(char) * (size_t)(len+1));
      m_strcpy_slice(token, prev, (int) len);
      str_array[cur_num] = token;
      prev = cur;
      prev++;
      cur_num++;
    }
    cur++;
  }
  str_array[num_tokens+1] = NULL;
  return str_array;
}



int str_char_count(char *str, char cmp) {
  int count = 0;
  while (*str != '\0') {
    if (*str++ == cmp) {
      count++;
    }
  }
  return count;
}

int m_strcpy_slice(char *dst, char *src, int n) {
  for (int i = 0; i < n; i++) {
    if ((*dst++ = *src++) == '\0') {
      return -1;
    }
  }
  *dst = '\0';
  return 0;
}


int main(int argc, char const *argv[])
{

	/* everything here happens only once */
	char* ps;
	char *line_buffer = malloc(sizeof(line_buffer) * BUFFER_LEN);
	size_t buffer_len = BUFFER_LEN;
	//char line_buffer[BUFFER_LEN];
	//char cmd_buffer[BUFFER_LEN];
	//char args_buffer[BUFFER_LEN];
	//int i = 1;
	clock_t begin = 0, end = 0;
	pid_t pid;
	char** tokens;
	double time_spent;
	/* setup prompt string */
	ps = malloc(strlen(PS) * sizeof (char) + 1);
	strncpy(ps, PS, BUFFER_LEN);
	signal(SIGINT, SIG_IGN);

	while(1) {
		printf("%-.30s ", ps); /* print prompt */
		getline(&line_buffer, &buffer_len, stdin);
		//fgets(line_buffer, BUFFER_LEN, stdin); /* get user input */
		line_buffer[strlen(line_buffer) - 1] = '\0'; /* drop newline */
		tokens = tokenizer(line_buffer); /* tokenize input */
		begin = clock();
		if ((pid = fork()) < 0) {
			perror("Fork failed!");
		};
		/* continue execution after fork */
		if (pid) {
			/* do shell builtin stuff here */
			if (tokens[0]) {
				if (strcmp(tokens[0], "cd") == 0) {
					chdir(tokens[1]);
				} else if (strcmp(tokens[0], ":setps") == 0) {
					if (tokens[1] && strlen(tokens[1]) > 0) {

						if (strcmp(tokens[1], "default") == 0) {
							strcpy(ps, PS);
						} else {
							strcpy(ps, tokens[1]);
						}
						puts("Prompt statement changed.");
					} else {
						puts("Invalid string");
					}
				}
			}

			/* parent process wait for children, then regains control */
			wait(NULL);

			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			if (DEBUG) {
				printf("Fork time: %f seconds\n", time_spent);
			}
			continue;
		} else {
			/* child process */
			if(tokens[0]) {
				if (strcmp(tokens[0], "exit") == 0) {
					puts("Exiting...");
					kill(getppid(), SIGTERM);
					kill(pid, SIGTERM);
				}
				execvp(tokens[0], tokens);
				/* execution only reaches this point if exec fails */
				printf("Command not found: %.*s\n", BUFFER_LEN, line_buffer);
				}
			}
			/* done with child, time to exit */
			_exit(0);
		}
		return EXIT_SUCCESS;
	}
