#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <regex.h>
#include <libc.h>



#define PS1 "\x1b[32mshell~>\x1b[0m"
#define BUFFER_LEN 256
char* ps1;
char line_buffer[BUFFER_LEN];
char cmd_buffer[BUFFER_LEN];
char args_buffer[BUFFER_LEN];
clock_t begin, end;
pid_t pid;

char** tokens;

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
  /* should you check malloc ? */
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


void str_array_free(char **str_array) {
  char **cur = str_array;
  while (*cur) {
    free(*cur);
    cur++;
  }
  free(str_array);
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

void m_strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0');
}



int m_strcmp(char *a, char *b) {
  char *q1 = a;
  char *q2 = b;
  while((*q1 == *q2) && *q1) {
    q1++;
    q2++;
  }
  return (*q1 - *q2);
}

int m_strlen(char *str) {
  int i = 0;
  while (*str++) {
    i++;
  }
return i;
}

void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

void cdHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(99, cdHandler);
    puts("CD!");
    fflush(stdout);
    //execlp(tokens[0], tokens[0], tokens[1], NULL);
    chdir(tokens[1]);
}

int main(int argc, char const *argv[])
{

	double time_spent;

	signal(SIGINT, SIG_IGN);
	signal(99, cdHandler);
	ps1 = malloc(strlen(PS1) * sizeof (char) + 1);
	strncpy(ps1, PS1, BUFFER_LEN);

	while(1) {
		printf("pre PID = %d, PPID = %d\n", getpid(), getppid());

		if ((pid = fork()) < 0) {
			perror("Fork failed!");
		};
		/* continue execution after fork */
		if (pid) {
			/* parent process wait for children, then regains control */
			wait(NULL);
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("Time Spent: %f seconds\n", time_spent);
		} else {

			printf("%-.30s ", ps1);
			fgets(line_buffer, BUFFER_LEN, stdin);
			/* drop newline */
			line_buffer[strlen(line_buffer) - 1] = '\0';
			/* tokenize input */
			tokens = tokenizer(line_buffer);
			if(tokens[0]) {
				if (strcmp(tokens[0], "exit") == 0) {
					puts("Exiting...");
					fflush(stdout);
					/* kill parent */
					kill(getppid(), SIGTERM);
					//kill(getpid(), SIGTERM);
					//exit(0);
				} else if (strcmp(tokens[0], "cd") == 0) {
					puts("cd doesn't work!");
					chdir(tokens[1]);
					kill(getppid(), 99);
				} else if (!strcmp(tokens[0], "ps1") && tokens[1]) {
					puts("Changing PS1");
					strcpy(ps1, tokens[1]);
				} else {
				execlp(tokens[0], tokens[0], tokens[1], NULL);
				/* execution only reaches this point if exec fails */
				printf("Command not found: %.*s\n", BUFFER_LEN, line_buffer);
				}
			}
			/* done with child, time to exit */
			_exit(0);
		}
	}
	return 0;
}

