#include "../include/shell.h"


typedef struct {
	char *name;
	char *str;
	char *boldstr;
} Color;

Color colors[8] = {
	{ "black", "\33[30m", "\33[1;30m" },
	{ "red", "\33[31m", "\33[1;31m" },
	{ "green", "\33[32m", "\33[1;32m" },
	{ "yellow", "\33[33m", "\33[1;33m" },
	{ "blue", "\33[34m", "\33[1;34m" },
	{ "magenta", "\33[35m", "\33[1;35m" },
	{ "cyan", "\33[36m", "\33[1;36m" },
	{ "white", "\33[37m", "\33[1;37m" }
};

char *end = "\33[0m";

void execBuiltIn(char **args, pid_t *pid) {
	char *command = args[0];

	if (strcmp(command, "help") == 0) {
		if (((*pid) = fork()) == 0) {
			int y ;
			y = printHelp();
			_exit(y);
		}

		builtinwait = 1;
	}
	else if (strcmp(command, "chpmt") == 0) {
		lastRetCode = chpmt(args[1], *args[2]);

	}
	else if (strcmp(command, "cd") == 0) {
		int r = cd(args[1]);
		if (r != 0) {
			char t[500] = {'\0'};
			char *u = "sfish: cd: ";
			char *v = "No such file or directory";
			strncat(t, u, strlen(u) + 1);
			strncat(t, args[1], strlen(args[1]) + 1);
			strncat(t, ": ", 2);
			strncat(t, v, strlen(v) + 1);
			strncat(t, "\n", 2);
			write(STDERR_FILENO, t, strlen(t) + 1);
		}
		lastRetCode = r;
	}
	else if (strcmp(command, "pwd") == 0) {
		if (((*pid) = fork()) == 0) {
			getPwd();
			_exit(0);
		}

		builtinwait = 1;

	}
	else if (strcmp(command, "chclr") == 0) {
		lastRetCode = chclr(args[1], args[2], *args[3]);
	}
	else if (strcmp(command, "prt") == 0) {
		if (((*pid) = fork()) == 0) {
			int y ;
			y = prt();
			_exit(y);
		}

		builtinwait = 1;


	}
	else if (strcmp(command, "disown") == 0) {
		disown(args);
	}
	else if (strcmp(command, "jobs") == 0) {
		if (((*pid) = fork()) == 0) {
			printList(head);
			_exit(0);
		}
		builtinwait = 1;
	}
	else if (strcmp(command, "fg") == 0) {
		fg(args);
	}
	else if (strcmp(command, "bg") == 0) {
		background(args);
	}
	else if (strcmp(command, "kill") == 0) {
		k_i_l_l(args);
	}
}



int printHelp() {
	char *t = "\nhelp\nexit\ncd [DIR]\npwd\nprt\nchpmt [SETTING] [TOOGLE]\nchclr [SETTING] [COLOR] [BOLD]\njobs\nfg PID|JID\nbg PID|JID\nkill [signal] PID|JID\ndisown [PID|JID]\n\n";
	write(STDOUT_FILENO, t, strlen(t) + 1);
	return EXIT_SUCCESS;
}

int prt() {
	fprintf(stderr, "%d\n", lastRetCode);
	fflush(stderr);
	return EXIT_SUCCESS;
}

int cd(char *dir) {
	int ret = 0;
	if (strcmp(dir, "-") == 0) {
		//check is there exists previous.
		if (*lastDirec == '\0') {
			char * t = "sfish: cd: OLDPWD not set\n";
			write(STDERR_FILENO, t, strlen(t) + 1); 
			ret = 0;
		}
		else {
			ret = chdir(lastDirec);
		}

	}
	else if (dir == NULL) {
		char * home = getenv("HOME");
		ret = chdir(home);
		memset(lastDirec, '\0', 1000);
		strcpy(lastDirec, home);
	}
	else if (strcmp(dir, ".") == 0) {
		memset(lastDirec, '\0', 1000);
		getcwd(lastDirec, 1000);
		ret  = 0;
	}
	else if (strcmp(dir, "..") == 0) {
		// check if your in "/" dir
		char* current = malloc(1000 * sizeof(char));
		memset(current, '\0', 1000);
		getcwd(current, 1000);

		if (strcmp(current, "/") == 0) {
			memset(lastDirec, '\0', 1000);
			strcpy(lastDirec, current);
			ret = 0;

		}
		else {
			memset(lastDirec, '\0', 1000);
			strcpy(lastDirec, current);

			int ch = '/';
			char * ptr = strrchr( current, ch );
			unsigned long index = ptr - current;
			current[index] = '\0';

			ret = chdir(current);
		}

		free(current);

	}
	else {
		memset(lastDirec, '\0', 1000);
		getcwd(lastDirec, 1000);
		ret = chdir(dir);
	}
	setPrompt();

	return ret;
}

void setPrompt() {
	sigset_t maskall , prev;
	sigfillset(&maskall);
	sigprocmask(SIG_BLOCK, &maskall, &prev);

	char *sfish = "shell";

	memset(prompt, '\0', 1000);
	strcpy(prompt, sfish);

	if (userToggle == 48) {
		if (machineToggle == 49) {
			strncat(prompt, "-", 2);
			strncat(prompt, hostP, 257);
		}
	}
	else if (userToggle == 49) {
		strncat(prompt, "-", 2);
		strncat(prompt, userP, 257);
		if (machineToggle == 49) {
			strncat(prompt, "@", 2);
			strncat(prompt, hostP, 257);
		}

	}

	memset(currentDirec, '\0', 1000);
	getcwd(currentDirec, 1000);

	strncat(prompt, ":", 2);
	strncat(prompt, "[", 2);
	strncat(prompt, currentDirec, strlen(currentDirec) + 1);
	strncat(prompt, "]", 2);
	strncat(prompt, "> ", 2);

	sigprocmask(SIG_SETMASK, &prev, NULL);


}

void getPwd() {

	char *b = malloc(1000 * sizeof(char));
	memset(b, '\0', 1000);
	getcwd(b, 1000);
	int i = 0;
	for (i = 0; i < strlen(b); i++) {
		if (b[i] == '\0') {
			break;
		}
	}
	write(STDOUT_FILENO, b, i + 1);
	write(STDOUT_FILENO, "\n", 1);

	free(b);

}

int chpmt(char *setting, char toggle) {
	int i = 0;
	for (i = 0; i < strlen(setting); i++) {
		setting[i] = tolower(setting[i]);
	}

	if (strcmp(setting, "user") == 0) {
		userToggle = toggle;
	}
	else if (strcmp(setting, "machine") == 0) {
		machineToggle = toggle;
	}
	else {
		char *t = "sfish: chpmt: Wrong Setting value\n";
		write(STDERR_FILENO, t, strlen(t) + 1);
		return EXIT_FAILURE;
	}
	if (toggle != 48)
		if (toggle != 49) {
			char *t = "sfish: chpmt: Wrong Toggle value\n";
			write(STDERR_FILENO, t, strlen(t) + 1);
			return EXIT_FAILURE;
		}

	setPrompt();
	return EXIT_SUCCESS;
}

int chclr(char *setting, char *color, char bold) {

	int i = 0;
	int ret = EXIT_FAILURE;
	int b = 0;
	for (i = 0; i < strlen(setting); i++) {
		setting[i] = tolower(setting[i]);
	}
	i = 0;
	for (i = 0; i < strlen(color); i++) {
		color[i] = tolower(color[i]);
	}

	if (bold != 48) {
		if (bold != 49) {
			char *t = "sfish: chclr: Wrong Bold value\n";
			write(STDERR_FILENO, t, strlen(t) + 1);
			return EXIT_FAILURE;
		}
		else {
			b = 1;
		}
	}



	if (strcmp(setting, "user") == 0) {
		i = 0;
		for (i = 0; i < 8; i++) {
			if (strcmp(color, colors[i].name) == 0) {
				ret = EXIT_SUCCESS;
				char *clr = malloc(256);
				memset(clr, '\0', 256);
				if (b)
					strcpy(clr, colors[i].boldstr);
				else
					strcpy(clr, colors[i].str);
				strncat(clr, userP, strlen(userP) + 1);
				strncat(clr, end, strlen(end) + 1);
				memset(userP, '\0', 256);
				strcpy(userP, clr);
				free(clr);
				break;
			}
		}
	}
	else if (strcmp(setting, "machine") == 0) {
		i = 0;
		for (i = 0; i < 8; i++) {
			if (strcmp(color, colors[i].name) == 0) {
				ret = EXIT_SUCCESS;
				char *clr = malloc(256);
				memset(clr, '\0', 256);
				if (b)
					strcpy(clr, colors[i].boldstr);
				else
					strcpy(clr, colors[i].str);
				strncat(clr, hostP, strlen(hostP) + 1);
				strncat(clr, end, strlen(end) + 1);
				memset(hostP, '\0', 256);
				strcpy(hostP, clr);
				free(clr);
				break;
			}
		}
	}
	else {
		char *t = "sfish: chclr: Wrong Setting value\n";
		write(STDERR_FILENO, t, strlen(t) + 1);
		return EXIT_FAILURE;
	}

	if (ret == EXIT_FAILURE) {
		char *t = "sfish: chclr: Wrong Color value\n";
		write(STDERR_FILENO, t, strlen(t) + 1);
	}
	setPrompt();
	return ret;
}

void printList(jobNode* hd) {
	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	jobNode* n = hd;
	if (n == NULL) {
		write(STDOUT_FILENO, "\n", 2);
	}
	else {
		while (n != NULL) {

			char *t = malloc(1000);
			strncat(t, "[", 2);
			char jidBuf[200] = {'\0'};
			sprintf(jidBuf, "%d", n->jid);
			strncat(t, jidBuf, strlen(jidBuf));
			strncat(t, "]", 2);
			strncat(t, "\t", 2);
			strncat(t, n->status, strlen(n->status) + 1);
			strncat(t, "\t\t", 2);
			char pidBuf[200] = {'\0'};
			sprintf(pidBuf, "%d", n->pid);
			strncat(t, pidBuf, strlen(pidBuf));
			strncat(t, "\t", 2);
			strncat(t, n->cmd, strlen(n->cmd) + 1);
			strncat(t, "\n", 2);
			write(STDOUT_FILENO, t, strlen(t) + 1);
			n = n->next;
		}
	}
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
}


void disown(char **args) {

	sigset_t maskall , prev;
	sigfillset(&maskall);
	sigprocmask(SIG_BLOCK, &maskall, &prev);

	c = 0;


	if (strcmp(args[1], "") == 0) {
		while (head != NULL) {
			removeJob(head, head);
		}
	}
	else if (*args[1] == '%') {

		char temp = args[1][0];
		(args[1])[0] = '0';
		long jidval;
		jidval = strtol(args[1], NULL, 10);

		char *jidBuf = malloc(strlen(args[1]) + 1);
		memset(jidBuf, '\0', strlen(args[1]) + 1);
		jidBuf[0] = '0';
		sprintf((jidBuf + 1), "%lu", jidval);



		if (strcmp(jidBuf, args[1]) != 0) {
			(args[1])[0] = temp;
			fprintf(stderr, "disown: %s: no such job\n", args[1] + 1);
			fflush(stderr);
		}
		else {
			jobNode* toRemove = findNode(jidval, -1);
			if (toRemove != NULL)
				removeJob(head, toRemove);
			else {
				(args[1])[0] = temp;
				fprintf(stderr, "disown: %s: no such job\n", args[1] + 1);
				fflush(stderr);
			}
		}
		free(jidBuf);
		(args[1])[0] = temp;

	}
	else {

		long pidval;
		pidval = strtol(args[1], NULL, 10);
		char *pidBuf = malloc(strlen(args[1]) + 1);
		memset(pidBuf, '\0', strlen(args[1]) + 1);
		sprintf((pidBuf), "%lu", pidval);

		if (strcmp(pidBuf, args[1]) != 0) {
			fprintf(stderr, "disown: %s : no such job \n", args[1]);
			fflush(stdout);
		}

		else {
			jobNode* toRemove = findNode(-1, pidval);
			if (toRemove != NULL)
				removeJob(head, toRemove);
			else {
				fprintf(stderr, "disown: %s: no such job\n", args[1]);
				fflush(stderr);
			}
		}
		free(pidBuf);
	}

	sigprocmask(SIG_SETMASK, &prev, NULL);

}


void fg(char** args) {

	sigset_t maskall , prev;
	sigfillset(&maskall);

	if (strcmp(args[1], "") == 0) {
		fprintf(stderr, "fg: needs arguments that are either PIDs or JIDs\n");
		fflush(stderr);

	}
	else if (*args[1] == '%') {
		char temp = args[1][0];
		(args[1])[0] = '0';
		long jidval;
		jidval = strtol(args[1], NULL, 10);

		char *jidBuf = malloc(strlen(args[1]) + 1);
		memset(jidBuf, '\0', strlen(args[1]) + 1);
		jidBuf[0] = '0';
		sprintf((jidBuf + 1), "%lu", jidval);
		if (strcmp(jidBuf, args[1]) != 0) {
			(args[1])[0] = temp;
			fprintf(stderr, "fg: %s: no such job\n", args[1] + 1);
			fflush(stderr);
		}
		else {
			jobNode* toRemove = findNode(jidval, -1);
			if (toRemove != NULL) {
				toForeground = 1;
				fgpid = toRemove->pid;

				to_fg_CMD = malloc(strlen(toRemove->cmd) + 1);
				memset(to_fg_CMD, '\0', strlen(toRemove->cmd) + 1);
				strncpy(to_fg_CMD, toRemove->cmd, strlen(toRemove->cmd));
				if (strcmp(toRemove->status, "Stopped") == 0) {
					sigcontPID = toRemove->pid;
					raise(SIGCONT);

				}

				sigtstpPID = toRemove->pid;
				sigprocmask(SIG_BLOCK, &maskall, &prev);
				removeJob(head, toRemove);
				sigprocmask(SIG_SETMASK, &prev, NULL);


			}
			else {
				(args[1])[0] = temp;
				fprintf(stderr, "fg: %s: no such job\n", args[1] + 1);
				fflush(stderr);
			}

		}
		free(jidBuf);
		(args[1])[0] = temp;

	}
	else {


		long pidval;
		pidval = strtol(args[1], NULL, 10);
		char *pidBuf = malloc(strlen(args[1]) + 1);
		memset(pidBuf, '\0', strlen(args[1]) + 1);
		sprintf((pidBuf), "%lu", pidval);

		if (strcmp(pidBuf, args[1]) != 0) {
			fprintf(stderr, "fg: %s : no such job \n", args[1]);
			fflush(stdout);
		}

		else {
			jobNode* toRemove = findNode(-1, pidval);
			if (toRemove != NULL) {
				toForeground = 1;
				fgpid = toRemove->pid;
				to_fg_CMD = malloc(strlen(toRemove->cmd) + 1);
				memset(to_fg_CMD, '\0', strlen(toRemove->cmd) + 1);
				strncpy(to_fg_CMD, toRemove->cmd, strlen(toRemove->cmd));
				if (strcmp(toRemove->status, "Stopped") == 0) {
					sigcontPID = toRemove->pid;

					raise(SIGCONT);

				}

				sigtstpPID = toRemove->pid;

				sigprocmask(SIG_BLOCK, &maskall, &prev);
				removeJob(head, toRemove);
				sigprocmask(SIG_SETMASK, &prev, NULL);

			}
			else {
				fprintf(stderr, "fg: %s: no such job\n", args[1]);
				fflush(stderr);
			}
		}
		free(pidBuf);
	}
}

void background(char** args) {


	if (strcmp(args[1], "") == 0) {

		fprintf(stderr, "bg: needs arguments that are either PIDs or JIDs\n");
		fflush(stderr);

	}

	else if (*args[1] == '%') {

		char temp = args[1][0];
		(args[1])[0] = '0';
		long jidval;
		jidval = strtol(args[1], NULL, 10);

		char *jidBuf = malloc(strlen(args[1]) + 1);
		memset(jidBuf, '\0', strlen(args[1]) + 1);
		jidBuf[0] = '0';
		sprintf((jidBuf + 1), "%lu", jidval);

		if (strcmp(jidBuf, args[1]) != 0) {
			(args[1])[0] = temp;
			fprintf(stderr, "bg: %s: no such job\n", args[1] + 1);
			fflush(stderr);
		}
		else {
			jobNode* toRemove = findNode(jidval, -1);
			if (toRemove != NULL) {
				if (strcmp(toRemove->status, "Stopped") == 0) {
					sigcontPID = toRemove->pid;

					raise(SIGCONT);
				}
				else {
					fprintf(stderr, "bg: %s: is already running\n", args[1] + 1);
					fflush(stderr);
				}
			}
			else {
				(args[1])[0] = temp;
				fprintf(stderr, "bg: %s: no such job\n", args[1] + 1);
				fflush(stderr);
			}

		}
		free(jidBuf);
		(args[1])[0] = temp;
	}
	else {

		long pidval;
		pidval = strtol(args[1], NULL, 10);
		char *pidBuf = malloc(strlen(args[1]) + 1);
		memset(pidBuf, '\0', strlen(args[1]) + 1);
		sprintf((pidBuf), "%lu", pidval);

		if (strcmp(pidBuf, args[1]) != 0) {
			fprintf(stderr, "bg: %s : no such job \n", args[1]);
			fflush(stdout);
		}

		else {
			jobNode* toRemove = findNode(-1, pidval);
			if (toRemove != NULL) {
				if (strcmp(toRemove->status, "Stopped") == 0) {
					sigcontPID = toRemove->pid;

					raise(SIGCONT);
				}
				else {
					fprintf(stderr, "bg: %s: is already running\n", args[1] + 1);
					fflush(stderr);
				}
			}
			else {
				fprintf(stderr, "bg: %s: no such job\n", args[1] + 1);
				fflush(stderr);
			}
		}
		free(pidBuf);
	}
}


void k_i_l_l(char **args) {

	if (strcmp(args[1], "") == 0) {
		fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
		fflush(stderr);
	}

	else if (strcmp(args[2], "") == 0) {
		if (*args[1] == '%') {

			char temp = args[1][0];
			(args[1])[0] = '0';
			long jidval;
			jidval = strtol(args[1], NULL, 10);

			char *jidBuf = malloc(strlen(args[1]) + 1);
			memset(jidBuf, '\0', strlen(args[1]) + 1);
			jidBuf[0] = '0';
			sprintf((jidBuf + 1), "%lu", jidval);

			if (strcmp(jidBuf, args[1]) != 0) {
				(args[1])[0] = temp;
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stderr);
			}
			else {
				jobNode* n = findNode(jidval, -1);
				if (n != NULL) {
					if (kill(n->pid, SIGTERM) < 0) {
						if (errno == EINVAL) {
							char *e = "Invalid signal specified.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == EPERM) {
							char *e = "Targer does not have permission to send the signal to any of the target processes.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == ESRCH) {
							char *e = "The process or process group does not exist.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
					}
					else {
						fprintf(stdout, "[%lu] %d was hit by signal 15\n", jidval, n->pid);
						fflush(stdout);
					}
				}
				else {
					fprintf(stderr, "kill: %s : no such job \n", args[1]);
					fflush(stdout);
				}

			}
			free(jidBuf);
			(args[1])[0] = temp;
		}
		else {

			long pidval;
			pidval = strtol(args[1], NULL, 10);
			char *pidBuf = malloc(strlen(args[1]) + 1);
			memset(pidBuf, '\0', strlen(args[1]) + 1);
			sprintf((pidBuf), "%lu", pidval);

			if (strcmp(pidBuf, args[1]) != 0) {
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stdout);
			}

			else {
				jobNode* n = findNode(-1, pidval);
				if (n != NULL) {
					if (kill(n->pid, SIGTERM) < 0) {
						if (errno == EINVAL) {
							char *e = "Invalid signal specified.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == EPERM) {
							char *e = "Targer does not have permission to send the signal to any of the target processes.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == ESRCH) {
							char *e = "The process or process group does not exist.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
					}
					else {
						fprintf(stdout, "[%d] %d was hit by signal 15\n", n->jid, n->pid);
						fflush(stdout);
					}
				}
				else {
					fprintf(stderr, "kill: %s : no such job \n", args[1]);
					fflush(stdout);
				}
			}
			free(pidBuf);
		}
	}


	else if (strcmp(args[3], "") == 0) {
		if (*args[2] == '%') {

			char temp = args[2][0];
			(args[2])[0] = '0';
			long jidval;
			jidval = strtol(args[2], NULL, 10);

			char *jidBuf = malloc(strlen(args[2]) + 1);
			memset(jidBuf, '\0', strlen(args[2]) + 1);
			jidBuf[0] = '0';
			sprintf((jidBuf + 1), "%lu", jidval);

			long sig;
			sig = strtol(args[1], NULL, 10);
			char *sigBuf = malloc(strlen(args[1]) + 1);
			memset(sigBuf, '\0', strlen(args[1]) + 1);
			sprintf((sigBuf), "%lu", sig);

			if (strcmp(sigBuf, args[1]) != 0) {
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stdout);
			}

			else if (strcmp(jidBuf, args[2]) != 0) {
				(args[2])[0] = temp;
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stderr);
			}
			else {
				jobNode* n = findNode(jidval, -1);
				if (n != NULL) {
					if (kill(n->pid, sig) < 0) {
						if (errno == EINVAL) {
							char *e = "Invalid signal specified.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == EPERM) {
							char *e = "Targer does not have permission to send the signal to any of the target processes.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == ESRCH) {
							char *e = "The process or process group does not exist.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
					}
					else {
						fprintf(stdout, "[%lu] %d was hit by signal %lu\n", jidval, n->pid, sig);
						fflush(stdout);
					}
				}
				else {
					fprintf(stderr, "kill: %s : no such job \n", args[2]);
					fflush(stdout);
				}

			}
			free(jidBuf);
			(args[1])[0] = temp;
		}
		else {

			long pidval;
			pidval = strtol(args[2], NULL, 10);
			char *pidBuf = malloc(strlen(args[2]) + 1);
			memset(pidBuf, '\0', strlen(args[2]) + 1);
			sprintf((pidBuf), "%lu", pidval);
			long sig;
			sig = strtol(args[1], NULL, 10);
			char *sigBuf = malloc(strlen(args[1]) + 1);
			memset(sigBuf, '\0', strlen(args[1]) + 1);
			sprintf((sigBuf), "%lu", sig);

			if (strcmp(sigBuf, args[1]) != 0) {
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stdout);
			}

			else if (strcmp(pidBuf, args[2]) != 0) {
				fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
				fflush(stdout);
			}

			else {
				jobNode* n = findNode(-1, pidval);
				if (n != NULL) {
					if (kill(n->pid, SIGTERM) < 0) {
						if (errno == EINVAL) {
							char *e = "Invalid signal specified.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == EPERM) {
							char *e = "Targer does not have permission to send the signal to any of the target processes.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
						else if (errno == ESRCH) {
							char *e = "The process or process group does not exist.";
							write(STDERR_FILENO, e, strlen(e) + 1);
						}
					}
					else {
						fprintf(stdout, "[%d] %d was hit by signal %lu\n", n->jid, n->pid, sig);
						fflush(stdout);
					}
				}
				else {
					fprintf(stderr, "kill: %s : no such job \n", args[2]);
					fflush(stdout);
				}
			}
			free(pidBuf);
		}
	}
	else {
		fprintf(stderr, "kill: usage: kill [signal] JID|PID\n");
		fflush(stderr);
	}

}

