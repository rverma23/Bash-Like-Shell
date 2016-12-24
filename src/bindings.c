#include "../include/shell.h"

int printHelp_cmd() {

	char *t = "\nhelp\nexit\ncd [DIR]\npwd\nprt\nchpmt [SETTING] [TOOGLE]\nchclr [SETTING] [COLOR] [BOLD]\njobs\nfg PID|JID\nbg PID|JID\nkill [signal] PID|JID\ndisown [PID|JID]\n\n";
	write(STDOUT_FILENO, t, strlen(t) + 1);
	rl_on_new_line();
	return 0;
}

int storePID() {
	if (head != NULL) {
		SPID = head->pid;
	}
	else {
		SPID = -1;
	}
	return 0;
}

int getPID() {
	sigset_t maskall , prev;
	sigfillset(&maskall);
	sigprocmask(SIG_BLOCK, &maskall, &prev);

	if (SPID != -1) {
		jobNode* nodeToRemove = findNode(-1, SPID);
		if (nodeToRemove != NULL) {
			sigtermPID = SPID;
			raise(SIGTERM);
			SPID = -1;
		}
		else {
			char *t = "\nCouldn't find the job\n";
			write(STDOUT_FILENO, t, strlen(t) + 1);
		}
	}
	else {
		char *t = "\nSPID does not exist and has been set to -1\n";
		write(STDOUT_FILENO, t, strlen(t) + 1);
	}

	sigprocmask(SIG_SETMASK, &prev, NULL);
	rl_on_new_line();
	return 0;
}

int printSFISHInfo() {

	return 0;
}


