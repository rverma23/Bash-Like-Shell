#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

extern int lastRetCode;
extern int totalArgs;
extern char* lastDirec;
extern char* currentDirec;
extern char* prompt;
extern char userToggle;
extern char machineToggle;
extern char* userP;
extern char* hostP;
extern int isRedirec;
extern int permsize;
extern int bg;
extern pid_t fgpid;
extern pid_t sigtstpPID;
extern pid_t sigcontPID;
extern pid_t SPID;
extern pid_t sigtermPID;
extern int jidCounter;
extern char* jobCommandLine;
extern int c;
extern int is_built_in;
extern int builtinwait;
extern int commandFound;
extern int toForeground;
extern char* to_fg_CMD;

typedef struct jobNode{
	int jid;
	int pid;
	char *cmd;
	char *status;
	struct jobNode* next;
} jobNode;

jobNode* head;

jobNode* createJobNode(int jid, pid_t pid, char *cmd, char *status, jobNode* next);

jobNode* addToTail(jobNode* hd, jobNode* node);

void printList(jobNode* head);

jobNode* removeJob(jobNode* hd, jobNode* nodeToRemove);

jobNode* findNode(int jid, int pid);

char * makeJobCommandLine(char *cmd);

void deleteNode(jobNode* nodeToDelete);

int printHelp();

void parseCommand(char* cmdln, char* command, char* arg);

char *cleanUp(char *cmd);

void Allocate(char ***args, unsigned int size);

void printArgs(char ***args, unsigned int size);

void getArgs(char ***args, char* cmd);

void getTotalArgs(char* cmd);

void FreeArgs(char ***args, unsigned int size);

void getPwd();

void setPrompt();

int chpmt(char *setting, char toggle);

int cd (char* dir);

int prt();

int chclr(char *setting, char *color, char bold);

void evaluateProg(char **args, pid_t *pid2, int fdIn, int fdOut, int fdOuterr, int isP, int *fds);

int checkFileExists(char *path);

int isBuiltIn(char *command);

void execBuiltIn(char **args, pid_t *pid);

int isPipe(char **args, int size);

int isInputRedirection(char **args, int size);

int isOutputRedirection(char **args, int size);

int isOutputRedirectionErr(char **args, int size);

int nextRed(char **args, int size);

void sigchild_handler(int sig);

void sigkill_handler(int sig);

void outFileError();

void inFileError(char *f);

void sigtstp_handler(int sig);

void sigcont_handler(int sig);

void sigterm_handler(int sig);

int printHelp_cmd();

int storePID();

int getPID();

int printSFISHInfo();

void fg(char **args);

void disown(char **args);

void background(char **args);

void k_i_l_l(char **args);

void bindKeys();

