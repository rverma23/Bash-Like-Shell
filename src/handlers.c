#include "../include/shell.h"


void sigchild_handler(int sig) {

	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	pid_t cpid;
	int status;

	int oe = errno;
	while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {

		sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
		lastRetCode = WEXITSTATUS(status);
		jobNode* n = findNode(-1, cpid);
		if (n != NULL)
			removeJob(head, n);
		if (cpid == fgpid) {
			fgpid = -2;
			sigtstpPID = -2;
		}
		sigprocmask(SIG_SETMASK, &prev_h, NULL);

	}
	if (errno != oe) {
		if (errno != ECHILD) {
			char *e = "An error occured during waitpid";
			write(STDERR_FILENO, e, strlen(e) + 1);
		}
	}
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	c = 0;
	errno = oe;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
}


void sigkill_handler(int sig) {

	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	int oe = errno;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
	if (fgpid > 0) {
		pid_t gpd = getpgid(fgpid);
		if (kill(gpd, SIGINT) < 0) {
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
	}
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	fgpid = -2;
	sigtstpPID = -2;
	c = 0;
	errno = oe;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);

}

void sigtstp_handler(int sig) {

	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	int oe = errno;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
	if (sigtstpPID > 0) {
		pid_t gpd = getpgid(fgpid);
		if (kill(gpd, SIGTSTP) < 0) {
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

			sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
			jobNode* t = findNode(-1, sigtstpPID);
			if (t == NULL) {
				jidCounter += 1;
				if (to_fg_CMD != NULL) {

					jobNode* job = createJobNode(jidCounter, sigtstpPID, to_fg_CMD, "Stopped", NULL);
					addToTail(head, job);
				}
				else {
					jobNode* job = createJobNode(jidCounter, sigtstpPID, jobCommandLine, "Stopped", NULL);
					addToTail(head, job);
				}
			}
			else {
				strncpy(t->status, "Stopped", strlen(t->status));
			}

			if (fgpid == sigtstpPID) {
				fgpid = -2;
				sigtstpPID = -2;
			}
			else {
				sigtstpPID = -2;
			}
			sigprocmask(SIG_SETMASK, &prev_h, NULL);
		}
	}

	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	c = 0;
	errno = oe;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);

}

void sigcont_handler(int sig) {

	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);
	int oe = errno;
	if (sigcontPID > 0) {
		int gpd = getpgid(sigcontPID);
		if (kill(gpd, SIGCONT) < 0) {
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
			sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
			jobNode* t = findNode(-1, sigcontPID);
			if (t == NULL) {
				fprintf(stderr, "Couldn't find the job you were looking for.\n");
				fflush(stderr);
			}
			else {
				strncpy(t->status, "Running", strlen(t->status));
			}
			sigtstpPID = -2;
			sigprocmask(SIG_SETMASK, &prev_h, NULL);
		}
	}

	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	sigcontPID = -2;
	errno = oe;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);
}


void sigterm_handler(int sig) {
	sigset_t maskall_h, prev_h;
	sigfillset(&maskall_h);

	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	int oe = errno;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);

	if (sigtermPID > 0) {
		pid_t gpd = getpgid(sigtermPID);
		if (kill(gpd, SIGTERM) < 0) {
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

			sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
			jobNode* t = findNode(-1, sigtermPID);
			if (t == NULL) {
				fprintf(stderr, "Couldn't find the job you were looking for.\n");
				fflush(stderr);
			}
			else {
				fprintf(stdout, "[%d] %d stopped by signal %d\n", t->jid, t->pid, SIGTERM);
			}
			sigprocmask(SIG_SETMASK, &prev_h, NULL);

		}
	}

	sigprocmask(SIG_BLOCK, &maskall_h, &prev_h);
	sigtermPID = -2;
	fgpid = -2;
	c = 0;
	errno = oe;
	sigprocmask(SIG_SETMASK, &prev_h, NULL);

}










