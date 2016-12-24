AUTHOR: Rahul Verma


HOW TO RUN:
	
	USE make clean command in terminal to clear all old executables.
	USE make to create new executables.
	
	You can RUN the program after a make command via bin/shell. 

HOW TO USE SHELL:
	
	BUILT IN COMMANDS:
	
		help -- displays all built in commands

		exit -- exists the shell

		cd [DIR] -- will change the directory to DIR

		pwd -- prints the current working directory.

		prt -- prints the return code of the command that was last executed.

		chpmt [SETTING] [TOOGLE] -- changes prompt setting 

				values of SETTING are :
					user: the user field of the prompt
					machine: the context field of the prompt
				values of TOOGLE are :
					1: Enabled
					0: Disabled

		chclr [SETTING] [COLOR] [BOLD] -- changes color and bold settings of the prompt.

				values of SETTING are :
					user: the user field of the prompt
					machine: the context field of the prompt
				values of COLOR are:
					red
					blue
					green
					yellow
					cyan
					magenta
					black
					white
				values of BOLD are :
					1: Enabled
					0: Disabled
				
		jobs -- prints to the screen all jobs that are running.

		fg PID|JID -- this will bring to the foreground any jobs specified by PID or JID.  If you use JID, '%' must come before the JID. 
					  For example, fg %1 will bring the job with JID 1 to the foreground.

		bg PID|JID -- this will bring to the background any jobs specified by PID or JID.  If you use JID, '%' must come before the JID. 
					  For example, fg %1 will bring the job with JID 1 to the background.

		kill [signal] PID|JID -- this will bring to the hit any jobs specified by PID or JID with the signal specified.  If you use JID, '%' must come 
								 before the JID. For example, kill 20 %1 will bring hit the job with JID 1 with signal 20. If no signal is specified
								 we hit the job with signal 15.

		disown [PID|JID] -- this will drop any jobs specified by PID or JID from the jobs list.  If you use JID, '%' must come before the JID. 
       					    For example, fg %1 will drob the job with JID 1 from the jobs list.

			
	OTHER FEATURES:
	
		This shell can fully run executables. For example if you gcc a helloworld.c file, you can run the executable product of this compilation in this shell.
		This shell also supports output redirections via the usual redirection symbols of > , < , and |.
		This shell supports the following keyboard shortcuts:
			Ctrl+S -- stores the PID of the first job if it exists.
			Ctrl+G -- evaluates if a PID is stored from Ctrl+S and if this is a foreground job, it will terminate it.
			Ctrl+H -- prints the help menu.

	LIMITATIONS:
		This shell CANNOT execute correctly exotic commands such as, ls -a > ls -a > ls -a.
		
	




	
	
	
	
	
