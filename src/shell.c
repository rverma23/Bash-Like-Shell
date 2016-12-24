#include "../include/shell.h" //change this later

int lastRetCode = 0;
int totalArgs = 0;
char* lastDirec = NULL;
char* currentDirec;
char* prompt;
char userToggle = 49;
char machineToggle = 49;
char* userP;
char* hostP;
int isRedirec = 0;
int permsize = 0;
int bg = 0;
pid_t fgpid = -2;
pid_t sigtstpPID = -2;
pid_t sigcontPID = -2;
pid_t SPID = -2;
pid_t sigtermPID = -2;
int jidCounter = 0;
char* jobCommandLine;
int c = 1;
int is_built_in = 0;
int builtinwait = 0;
int commandFound = 1;
int toForeground = 0;
char* to_fg_CMD = NULL;

jobNode* head = NULL;

const char *builtin[12] = {"help", "exit", "cd", "pwd", "prt", "chpmt", "chclr", "jobs", "fg", "bg", "kill", "disown"};

int main(int argc, char** argv) {

	rl_catch_signals = 0;

	//set key bindings.
	bindKeys();
	//set up signal handling
	sigset_t maskall, prevmask;
	sigfillset(&maskall);

	if (signal(SIGCHLD, sigchild_handler) == SIG_ERR) {
		char *e = "There was a SIGCHILD signal error";
		write(STDERR_FILENO, e, strlen(e) + 1);
	}
	if (signal(SIGINT, sigkill_handler) == SIG_ERR) {
		char *e = "There was a SIGINT signal error";
		write(STDERR_FILENO, e, strlen(e) + 1);
	}
	if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR) {
		char *e = "There was a SIGTSTP signal error";
		write(STDERR_FILENO, e, strlen(e) + 1);
	}
	if (signal(SIGCONT, sigcont_handler) == SIG_ERR) {
		char *e = "There was a SIGCONT signal error";
		write(STDERR_FILENO, e, strlen(e) + 1);
	}
	if (signal(SIGTERM, sigterm_handler) == SIG_ERR) {
		char *e = "There was a SIGCONT signal error";
		write(STDERR_FILENO, e, strlen(e) + 1);
	}

	char *cmd;
	char *cleanedUp;
	char *command;
	char **args;
	int size = 0;
	int error = 0;
	pid_t pid;

	lastDirec = malloc(1000 * sizeof(char));
	memset(lastDirec, '\0', 1000);

	currentDirec = malloc(1000 * sizeof(char));
	memset(currentDirec, '\0', 1000);

	prompt = malloc(1000 * sizeof(char));
	memset(prompt, '\0', 1000);
	strcpy(prompt, "shell> ");

	userP = malloc(256);
	memset(userP, '\0', 256);

	hostP = malloc(256);
	memset(hostP, '\0', 256);

	strncpy(userP, getenv("USER"), strlen(getenv("USER")));
	gethostname(hostP, 256);
	setPrompt();

	while ((cmd = readline(prompt)) != NULL) {
		error = 0;
		builtinwait = 0;
		toForeground = 0;
		bg = 0;
		isRedirec = 0;
		commandFound = 1;
		if ((*cmd)) {

			cleanedUp = cleanUp(cmd);
			add_history(cmd);

			size = strlen(cleanedUp) * 2;
			if (size < 10)
				size = 10;
			permsize = size;
			Allocate(&args, size);
			getArgs(&args, cleanedUp);
			command = args[0];
			jobCommandLine = makeJobCommandLine(cleanedUp);

			if (strcmp(command, "exit") == 0)
				break;
			if ((is_built_in = isBuiltIn(command)) == EXIT_SUCCESS)
				execBuiltIn(args, &pid);
			else {
				if (!isRedirec) {

					evaluateProg(args, &pid, -1, -1, -1, 0, NULL);
					setpgid(pid, pid);
				}
				else {
					char **temp = args;
					int inR, outR, outR_err, Pipe;
					inR = 5000;
					outR = 5000;
					outR_err = 5000;
					Pipe = 5000;
					int which = 50000;
					outR_err = isOutputRedirectionErr(args, size);
					inR = isInputRedirection(args, size);


					if (outR_err < inR) {
						inR = 0;
						which = outR_err;
					}
					else {
						outR_err = 0;
						which = inR;
					}

					outR = isOutputRedirection(args, size);
					if (which > outR)
						which = outR;
					Pipe = isPipe(args, size);
					if (which > Pipe)
						which = Pipe;

					while (outR_err || inR || outR || Pipe) {
						if (outR == which) {
							temp = temp + outR + 2;
							char *f = *(temp);
							int out = open(f, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
							if (out < 0) {
								outFileError();
								break;
							}
							inR  = isInputRedirection(temp, size - outR - 2 - 1);
							Pipe = isPipe(temp, size - outR - 2 - 1);
							if (inR == ((size - outR - 2 - 1) * 2))
								inR = 0;
							if (Pipe == ((size - outR - 2 - 1) * 2))
								Pipe = 0;
							if (inR) {
								temp = temp + inR + 2;
								f = *(temp);
								int in = open(f, O_RDONLY);
								if (in < 0) {
									inFileError(f);
									break;
								}
								evaluateProg(args, &pid, in, out, 0, 0, NULL);
								setpgid(pid, pid);
								break;
							}
							else if (Pipe) {
								if (inR)
									break;
								if (outR)
									break;
							}
							else {
								evaluateProg(args, &pid, -1, out, 0, 0, NULL);
								setpgid(pid, pid);
								break;
							}
							break;

						}
						else if (outR_err == which) {
							temp = temp + outR + 2;
							char *f = *(temp);
							int out = open(f, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
							if (out < 0) {
								outFileError();
								break;
							}
							inR  = isInputRedirection(temp, size - outR - 2 - 1);
							Pipe = isPipe(temp, size - outR - 2 - 1);

							if (inR == ((size - outR - 2 - 1) * 2))
								inR = 0;
							if (Pipe == ((size - outR - 2 - 1) * 2))
								Pipe = 0;

							if (inR) {
								temp = temp + inR + 2;
								f = *(temp);
								int in = open(f, O_RDONLY);
								if (in < 0) {
									inFileError(f);
									break;
								}
								evaluateProg(args, &pid, in, 0, out, 0, NULL);
								setpgid(pid, pid);
								break;
							}
							else if (Pipe) {
								if (inR)
									break;
								if (outR)
									break;
							}
							else {
								evaluateProg(args, &pid, -1, 0, out, 0, NULL);
								setpgid(pid, pid);
								break;
							}
							break;

						}
						else if (inR == which) {
							temp = args;
							int in = 0;
							int redInd = nextRed(temp, size);
							int s = size;
							char **temp2 = temp + redInd + 2;
							if (*(temp + 2) && *(temp + inR + 2)) {

								while (redInd) {
									redInd = nextRed(temp2, s - redInd - 2);
									if (redInd == 0) {
										char *f = temp2[0];
										in = open(f, O_RDONLY);
										if (in < 0) {
											inFileError(f);
											error = 1;
											break;
										}
										evaluateProg(args, &pid, in , 0, 0 , 0, NULL);
										setpgid(pid, pid);
										break;
									}
									if (*temp2[redInd] == '>') {
										char *f = temp2[0];
										in = open(f, O_RDONLY);
										if (in < 0) {
											inFileError(f);
											error = 1;
											break;
										}
										temp2 = temp2 + redInd + 2;
										int out = open(temp2[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
										if (out < 0) {
											outFileError();
											error = 1;
											break;
										}
										evaluateProg(args, &pid, in , out , 0 , 0, NULL);
										setpgid(pid, pid);
										break;
									}
									else if (strcmp(temp2[redInd], "2>") == 0) {
										char *f = temp2[0];
										in = open(f, O_RDONLY);
										if (in < 0) {
											inFileError(f);
											error = 1;
											break;
										}
										temp2 = temp2 + redInd + 2;
										int out = open(temp2[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
										if (out < 0) {
											outFileError();
											error = 1;
											break;
										}
										evaluateProg(args, &pid, in , 0, out , 0, NULL);
										setpgid(pid, pid);
										break;
									}
									else if (*temp2[redInd] == '<') {
										break;
									}
									else if (*temp2[redInd] == '|') {
										temp = args;
										char *first = temp2[0];
										int pipes[2];
										int in = open(first, O_RDONLY);
										int groupIdBool = 1;
										pid_t gpid = -1;
										if (in < 0) {
											inFileError(first);
											error = 1;
											break;
										}
										int redInd = nextRed(temp, size);
										int firstexc = 1;
										if (*(temp + 2) && *(temp + Pipe + 2)) {
											while (redInd) {

												pipe(pipes);
												evaluateProg(temp, &pid, in, pipes[1], 0, 1, pipes);
												if (groupIdBool) {
													gpid = pid;
													groupIdBool = 0;
												}
												setpgid(pid, gpid);

												in = pipes[0];
												temp = temp + redInd + 2;
												size = size - redInd - 2;
												redInd = nextRed(temp, size);

												if (firstexc) {
													temp = temp + redInd + 2;
													size = size - redInd - 2;
													firstexc = 0;
													redInd = nextRed(temp, size);
													if (redInd) {
														//temp = temp + redInd + 2;
														//size = size - redInd - 2;
													}
												}
												if (!redInd) {
													evaluateProg(temp, &pid, in, 1, 0, 1, pipes);
													setpgid(pid, gpid);
													close(pipes[0]);
													close(pipes[1]);
													break;
												}
												if (*temp[redInd] == '>') {

													char *f = *(temp + redInd + 2);
													int out = open(f, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
													if (out < 0) {
														outFileError(f);
														error = 1;
														break;
													}
													evaluateProg(temp, &pid, in, out, 0, 1, pipes);
													setpgid(pid, gpid);
													close(pipes[1]);
													break;
												}
												if (*temp[redInd] == '<') {
													close(pipes[0]);
													close(pipes[1]);
													break;
												}
												close(pipes[1]);
											}

										}
										break;
									}
								}
							}

							else {
								evaluateProg(args, &pid, in, 0, 0, 0, NULL);
								setpgid(pid, pid);
								break;
							}
						}
						else if (which == Pipe) {
							temp = args;
							int pipes[2];
							int in = 0;
							int redInd = nextRed(temp, size);
							pid_t gpid = -2;
							int groupIdBool = 1;
							int s = size;
							if (*(temp + 2) && *(temp + Pipe + 2)) {
								while (redInd) {

									pipe(pipes);
									evaluateProg(temp, &pid, in, pipes[1], 0, 1, pipes);

									if (groupIdBool) {
										gpid = pid;
										groupIdBool = 0;
									}

									setpgid(pid, gpid);
									in = pipes[0];
									temp = temp + redInd + 2;
									s = s - redInd - 2;
									redInd = nextRed(temp, s);

									if (!redInd) {
										evaluateProg(temp, &pid, in, 1, 0, 1, pipes);
										setpgid(pid, gpid);
										close(pipes[0]);
										close(pipes[1]);
										break;
									}

									if (*temp[redInd] == '>') {

										char *f = *(temp + redInd + 2);
										int out = open(f, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
										if (out < 0) {
											outFileError();
											break;
										}
										evaluateProg(temp, &pid, in, out, 0, 1, pipes);
										setpgid(pid, gpid);
										close(pipes[1]);
										break;
									}

									if (*temp[redInd] == '<') {
										close(pipes[0]);
										close(pipes[1]);
										break;
									}
									close(pipes[1]);
								}

							}
						}
						break;
					}
				}
			}

			if (commandFound&&(!error)) {
				if (!bg) {
					if (!toForeground) {
						fgpid = pid;
						sigtstpPID = pid;
					}
					else {
						sigtstpPID = fgpid;
					}

					c = 1;
					if (is_built_in == EXIT_SUCCESS) {
						if (builtinwait || toForeground) {
							while (c) {
								sigsuspend(&prevmask);
							}
						}
					}
					else {
						while (c) {
							sigsuspend(&prevmask);
						}
					}

				}
				if (bg) {
					sigprocmask(SIG_BLOCK, &maskall, &prevmask);
					jidCounter += 1;
					jobNode* job = createJobNode(jidCounter, pid, jobCommandLine, "Running", NULL);
					addToTail(head, job);
					sigprocmask(SIG_SETMASK, &prevmask, NULL);
				}
			}

			if (cmd != NULL) {
				free(cmd);
			}
			FreeArgs(&args, permsize);

			if (cleanedUp != NULL) {
				free(cleanedUp);
				cleanedUp = NULL;
			}
			if (jobCommandLine != NULL) {
				free(jobCommandLine);
				jobCommandLine = NULL;
			}
			if (to_fg_CMD != NULL) {
				free(to_fg_CMD);
				to_fg_CMD = NULL;
			}
		}
		else
			free(cmd);

	}

	//Free allocated memory, and close file descriptors.
	if (cmd != NULL)
		free(cmd);
	if (args != NULL)
		FreeArgs(&args, permsize);
	if (cleanedUp != NULL)
		free(cleanedUp);
	free(prompt);
	if (lastDirec != NULL)
		free(lastDirec);
	if (currentDirec != NULL)
		free(currentDirec);
	free(userP);
	free(hostP);
	if (jobCommandLine != NULL)
		free(jobCommandLine);
	if (to_fg_CMD != NULL) {
		free(to_fg_CMD);
		to_fg_CMD = NULL;
	}
	exit(0);
	return EXIT_SUCCESS;
}

char * makeJobCommandLine(char *cmd) {
	char *s = cmd;
	char *ret = (char *)malloc(1000);
	memset(ret, '\0', 1000);
	int i = 0;
	while (s[i] != '\0') {

		if (s[i] == '&') {
			break;
		}
		else {
			ret[i] = s[i];
		}
		i++;

	}
	ret[i] = '\0';
	return ret;
}


int nextRed(char **args, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		if (args[i] != NULL) {
			if ((!strcmp(args[i], "|")) || (!strcmp(args[i], "<")) || (!strcmp(args[i], ">")) || (!strcmp(args[i], "2>"))) {
				return i;
			}
		}
	}
	return 0;
}

int isPipe(char **args, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		if (args[i] != NULL) {
			if (strcmp(args[i], "|") == 0) {
				return i;
			}
		}
	}
	return size * 2;
}

int isInputRedirection(char **args, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		if (args[i] != NULL) {
			if (strcmp(args[i], "<") == 0) {
				return i;
			}
		}
	}
	return size * 2;

}

int isOutputRedirection(char **args, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		if (args[i] != NULL) {
			if (strcmp(args[i], ">") == 0) {
				return i;
			}
		}
	}
	return size * 2;
}

int isOutputRedirectionErr(char **args, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		if (args[i] != NULL) {
			if (strcmp(args[i], "2>") == 0) {
				return i;
			}
		}
	}
	return size * 2;

}


int checkFileExists(char *path) {
	struct stat stats;
	int ret;
	ret = stat(path, &stats);
	if (ret == 0)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}


void parseCommand(char *cmdln, char* command, char* arg) {

	char *str = cmdln;
	int i = 0;
	int j = 0;
	while ((str[i]) != 32) {
		command[i] = str[i];
		if (str[i] == 0) {
			break;
		}
		i++;
	}
	if (str[i] == 32) {
		i++;
		while (str[i] != 0) {
			arg[j] = str[i];
			i++;
			j++;
		}
	}

}

char *cleanUp(char *cmd) {

	char *s = cmd;
	char *ret = (char *)malloc(1000);
	memset(ret, '\0', 1000);

	int end = strlen(cmd) - 1;
	while ((*s) == 32) {
		s++;
	}
	while (s[end] == 32) {
		s[end] = '\0';
		end = end - 1;
	}
	int i = 0;
	int j = 0;
	while (s[i] != '\0') {
		if (s[i] == 32) {
			if (s[i + 1] != 32) {
				ret[j] = s[i];
				j++;
			}
		}
		else {
			ret[j] = s[i];
			j++;
		}
		i++;
	}
	ret[j] = '\0';
	return ret;
}

void Allocate(char ***args, unsigned int size) {
	int i = 0;
	*args = malloc(size * sizeof(char *));
	for (i = 0; i < size; ++i) {
		(*args)[i] = (char *)malloc(size + 2);
		memset((*args)[i], '\0', size + 2);
	}
}

void FreeArgs(char ***args, unsigned int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		free(((*args)[i]));
	}
	free(*args);
	*args = NULL;
}

void printArgs(char ***args, unsigned int size) {
	int i = 0;
	for (i = 0; i < size; ++i) {
		printf("%d: %s  %d\n", i, (*args)[i], *(*args)[i]);
	}
}

void getArgs(char ***args, char* cmd) {
	char *str = cmd;
	int a = strlen(cmd) + 1;
	char *m = (char* )malloc(a);
	memset(m, '\0', a);
	int i = 0;
	int j = 0;
	while ((*str) != '\0') {
		if (!bg) {
			if (((*str) != 32) && ((*str) != 124) && ((*str) != 60) && ((*str) != 62) && (!(((*str) == 50) && (((*(str + 1)) == 62))))) {
				if ((*str) == '&') {
					bg = 1;
				}
				else {
					m[i] = (*str);
					i++;
				}
			}
			else {
				strcpy((*args)[j], m);
				i = 0;
				j++;
				if ((*str) != 32) {
					isRedirec += 1;
					if (((*str) == 50) && (((*(str + 1)) == 62))) {
						if (*(str - 1) != 32)
							j++;
						char t = *str;
						strcpy((*args)[j], &(t));
						str++;
						t = *str;
						strncat((*args)[j], &(t), 2);
						j++;
						if (*(str + 2) != 32)
							j++;
					}
					else {
						if (*(str - 1) != 32)
							j++;
						char t = *str;
						strcpy((*args)[j], &(t));
						j++;
						if (*(str + 1) != 32)
							j++;
					}
				}
				free(m);
				m = (char* )malloc(a);
				memset(m, '\0', a);
			}
		}
		str++;
	}
	strcpy((*args)[j], m);
	i = 0;
	free(m);
	totalArgs = j + 1;

}

void getTotalArgs(char* cmd) {

	char *str = cmd;
	char *m = (char* )malloc(1000);
	memset(m, '\0', 1000);

	int i = 0;
	int j = 0;

	while ((*str) != '\0') {
		if (((*str) != 32) && ((*str) != 124) && ((*str) != 60) && ((*str) != 62)) {
			m[i] = (*str);
			i++;
		}
		else {
			i = 0;
			j++;
			if ((*str) != 32) {
				j++;
			}
			free(m);
			m = (char* )malloc(1000);
			memset(m, '\0', 1000);

		}
		str++;
	}
	i = 0;
	free(m);
	totalArgs = j + 1;
}

int isBuiltIn(char *command) {
	int i = 0;
	for (i = 0; i < 12; i++) {
		if (strcmp(command, builtin[i]) == 0)
			return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}



void outFileError() {
	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	char *err = "A couldn't get the outfile";
	write(STDERR_FILENO, err, strlen(err) + 1);
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
	rl_on_new_line();
}

void inFileError(char *f) {
	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	char err[500]={'\0'};
	strncat(err, "File '", 7);
	strncat(err, f, strlen(f) + 1);
	strncat(err, "' ", 3);
	strncat(err, "not found\n", 10);
	write(STDERR_FILENO, err, strlen(err));
	sigprocmask(SIG_SETMASK, &prev_h, NULL);

}


void bindKeys(){

	rl_bind_keyseq("\\C-h", printHelp_cmd);
	rl_bind_keyseq("\\C-b", storePID);
	rl_bind_keyseq("\\C-g", getPID);
	rl_bind_keyseq("\\C-p", printSFISHInfo);

}
