#include "../include/shell.h"

void evaluateProg(char **args, pid_t *pid2, int fdIn, int fdOut, int fdOuterr, int isP, int *fds) {


	char *totalPATH = getenv("PATH");
	int x = strlen(args[0]) + strlen(totalPATH) + 1;
	char cmdList[x];
	memset(cmdList, '\0', (x * sizeof(char)));
	pid_t *pid = pid2;
	int found = 0;

	if (strstr(args[0], "/")) {
		//execute directly.
		found = 1;
		int r = checkFileExists(args[0]);
		if (r == EXIT_SUCCESS) {

			int q = 0;
			char *c = args[q];
			//we have to make our args null terminated.
			while (*c != '\0') {
				q++;
				c = args[q];
			}
			args[q] = NULL;

			if (((*pid) = fork()) == 0) {
				if (execve(args[0], args, __environ) < 0) {
					fprintf(stderr, "%s %d\n", "ERROR!  errno:", errno);
					fflush(stderr);
				}
			}
		}
		else {
			found = 0;
		}
	}


	else {
		int i = 0;
		int j = 0;
		for (i = 0; i < strlen(totalPATH); i++) {
			if (totalPATH[i] == ':') {
				cmdList[j] = '/';
				j++;
				strncat(cmdList, args[0], (strlen(args[0]) + 1));
				int r = checkFileExists(cmdList);
				if (r == EXIT_FAILURE) {
					memset(cmdList, '\0', x * sizeof(char));
					j = 0;
				}
				else {
					found = 1;
					//strncpy(args[0],cmdList,strlen(cmdList)+1);
					int q = 0;
					char *c = args[q];
					//we have to make our args null terminated.
					while (*c != '\0') {
						q++;
						c = args[q];
					}
					args[q] = NULL;

					if (((*pid) = fork()) == 0) {
						if (fdIn > 0) {
							dup2(fdIn, STDIN_FILENO);
							close(fdIn);
						}
						if (fdOut > 0) {
							dup2(fdOut, STDOUT_FILENO);
						}
						if (fdOuterr > 0) {
							dup2(fdOuterr, STDERR_FILENO);
							close(fdOuterr);
						}
						if (isP) {
							close(fds[1]);
						}
						if (execve(cmdList, args, __environ) < 0) {
							fprintf(stderr, "%s %d\n", "ERROR!  errno:", errno);
							fflush(stderr);
							_exit(0);
						}
					}
					else {
						break;
					}
				}
			}
			else {
				cmdList[j] = totalPATH[i];
				j++;
			}
		}
	}
	if (!found) {
		commandFound = 0;
		char err[500] = {'\0'};
		strncat(err, "No command '", 13);
		strncat(err, args[0], strlen(args[0]) + 1);
		strncat(err, "' was not found.\n", 18);
		write(STDERR_FILENO, err, strlen(err));
	}
}
