#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


#define FILE_SIZE 55 // assume the file is at most 55 chars
#define NAME_SIZE 21 // assume the name of to_client file is at most 21 chars


void handleServer() {
	/* 
		when we get a signal from the server we call this function, and handle the server -
		read data from server, and print it to the screen.
    */
	int pid, status;
	char client_file_name[FILE_SIZE], client_file_data[FILE_SIZE], client_pid[5];

	// create client's file name
	strcpy(client_file_name, "to_client_");
	sprintf(client_pid, "%d", getpid());
	strcat(client_file_name, client_pid);
	strcat(client_file_name, ".txt");

	// open client's file
	int fid_client_file = open(client_file_name, O_RDONLY);
	if (fid_client_file == -1) { 
		write(2, "Cannot open to_client.txt\n", 26); 
		exit(-1); 
		}

	// read data and print to screen
	char curr_data;
	int ret_read_file = read(fid_client_file, &curr_data, 1);
	if (ret_read_file == -1) { 
		write(2, "Cannot read to_client.txt\n", 26); 
		close(fid_client_file);
		exit(-1); 
	}
	strncpy(client_file_data, &curr_data, 1);
	while(curr_data != '.') {
		int ret_read_file = read(fid_client_file, &curr_data, 1);
		if (ret_read_file == -1) { 
			write(2, "Cannot read to_client.txt\n", 26); 
			close(fid_client_file);
			exit(-1); 
		}
		strncat(client_file_data, &curr_data, 1);
	}

	write(1, client_file_data, strlen(client_file_data));
	write(1, "\n", 1);
	close(fid_client_file);

	pid = fork();
	if (pid == 0) {
		execl("/bin/rm", "/bin/rm", client_file_name, NULL);
	
	}
	else {

		waitpid(pid, &status, 0);
	}
	exit(0);
}
 
int main(int argc, char* argv[]) {
	// update signal
	signal(SIGUSR2, handleServer);

	// check arguments
	if (argc != 5) { // too many / less args
		write(2, "Invalid amount of arguments, need 4\n", 36);
		exit(-1);
	}
	
	
	int i, j, len_number;
	for (i = 1; i < argc; i++) {
		len_number = strlen(argv[i]);
    	for (j = 0; j < len_number; j++) {
			if (!isdigit(argv[i][j])) { // part of the input is not a digit
				write(2, "Invalid argument, only need numbers\n", 36);
				exit(-1);
			}
		}	
	}

	if (strcmp(argv[3], "4") == 0 && strcmp(argv[4], "0") == 0) {
		write(2, "Cannot divide by zero\n", 24);
		exit(-1);
	}
	
	// check if to_srv.txt exists
	int count_tries = 0, wait_time;
	int exist = access("to_srv.txt", F_OK); // check if exists

	while (exist == 0 && count_tries < 10) { // while exists
		count_tries++;
		wait_time = rand() % 5 + 1; // random number between 1 to 5
		sleep(wait_time);
		exist = access("to_srv.txt", F_OK); // check if exists
	}
	if (count_tries == 10) { // while stopped bc it tried 10 times
		write(2, "Tried 10 times, but to_srv.txt still exists\n", 44);
		exit(-1);
	}

	// create to_srv.txt
	int fid_srv_file = open("to_srv.txt", O_RDWR | O_CREAT, 0666);
	if (fid_srv_file == -1) { write(2, "Cannot open to_srv.txt\n", 23); exit(-1); }

	
	// write data to to_srv.txt 
	char s_pid[5];
	sprintf(s_pid, "%d", getpid());
	write(fid_srv_file, s_pid, strlen(s_pid)); // write client's pid
	write(fid_srv_file, " ", 1); // space to split data
	write(fid_srv_file, argv[2], strlen(argv[2])); // write first number
	write(fid_srv_file, " ", 1); // space to split data
	write(fid_srv_file, argv[3], strlen(argv[3])); // write operation number 
	write(fid_srv_file, " ", 1); // space to split data
	write(fid_srv_file, argv[4], strlen(argv[4])); // write second number
	write(fid_srv_file, " ", 1); // space to split data
	close(fid_srv_file);

	// modify server we finised
	pid_t pid_server = (pid_t)atoi(argv[1]); // str -> pid_t
	int ret_val_kill = kill(pid_server, SIGUSR1);
	if (ret_val_kill == -1) { 
		write(2, "This pid does not exist\n", 24); 
		int pid_rm, status;
		pid_rm = fork();
		if (pid_rm == 0) { // child to delete to_srv.txt if we did not send it
			execl("/bin/rm", "/bin/rm", "to_srv.txt", NULL);
		}
		else {
			waitpid(pid_rm, &status, 0);
		}
		close(fid_srv_file);
		exit(-1); 
	}
	pause(); // Waiting to answer from the server
}
