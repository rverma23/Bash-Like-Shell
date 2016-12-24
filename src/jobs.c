#include "../include/shell.h"


jobNode* findNode(int jid, int pid) {

	jobNode* n = head;

	while (n != NULL) {
		if (jid == -1) {
			if (n->pid == pid)
				break;
		}
		else if (pid == -1) {
			if (n->jid == jid)
				break;
		}

		n = n->next;
	}
	if (n != NULL) {
		if (jid == -1) {
			if (n->pid != pid)
				return NULL;
		}
		else if (pid == -1) {
			if (n->jid != jid)
				return NULL;
		}
	}
	return n;
}


jobNode* createJobNode(int jid, pid_t pid, char *cmd, char *status, jobNode* next) {
	jobNode* newJob = (jobNode*)malloc(sizeof(jobNode));
	newJob -> jid = jid;
	newJob -> pid = pid;
	newJob -> cmd = malloc(strlen(cmd) + 1);
	newJob -> status = malloc(strlen(status) + 1);
	newJob -> next = next;
	strncpy(newJob->cmd, cmd, strlen(cmd) + 1);
	strncpy(newJob->status, status, strlen(status) + 1);
	return newJob;
}

jobNode* addToTail(jobNode* hd, jobNode* node) {
	jobNode* n = hd;
	if (n == NULL) {
		head = node;
		return head;
	}
	while ((n->next) != NULL) {
		n = n->next;
	}
	n->next = node;
	return head;
}

jobNode* removeJob(jobNode* hd, jobNode* nodeToRemove) {

	jobNode* n = hd;
	if (nodeToRemove == hd) {
		jobNode* n_hd = hd->next;
		deleteNode(hd);
		head = n_hd;
		return head;
	}
	else {
		while (n != NULL) {
			if (n->next == nodeToRemove) {
				n->next = n->next->next;
				deleteNode(nodeToRemove);
				return head;
			}
			n = n->next;
		}
	}
	return head;
}

void deleteNode(jobNode* nodeToDelete) {
	free(nodeToDelete->cmd);
	free(nodeToDelete->status);
	nodeToDelete->next = NULL;
	free(nodeToDelete);
	nodeToDelete = NULL;
}








