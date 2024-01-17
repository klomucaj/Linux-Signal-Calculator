#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE_FILE 50 // i assume the file is at most 50 chars
#define SIZE_NAME 19 // i assume the name of to_client file is at most 19 chars

void handleClient() {
    /* 
        when we got a signal from the client we call this function, and handle the client -
        read data from client, calculate the math problem, 
        and write the answer to the problem in a file for the client.
    */
   signal(SIGUSR1, handleClient);  // keep the signal reffering to this function

   int pid;
   pid = fork();
   if (pid == 0) { // child - preform operation
        int fid_srv_file, fid_client_file, status, ret_read_val;
        char srv_file_txt[SIZE_FILE], client_file_name[SIZE_NAME];
        char* s_pid_client; char* s_num1; char* s_num2; char* operation;
        
        // get data and delete file
        fid_srv_file = open("to_srv.txt", O_RDONLY);
        if (fid_srv_file == -1) { write(2, "Cannot open to_srv.txt\n", 23); exit(-1); }

        char curr_data;
        ret_read_val = read(fid_srv_file, &curr_data, 1);
        if (ret_read_val == -1) { 
            write(2, "Cannot read to_srv.txt\n", 23); 
            close(fid_srv_file);
            exit(-1); 
        }
        strncpy(srv_file_txt, &curr_data, 1);
        while (curr_data != '.') {
            ret_read_val = read(fid_srv_file, &curr_data, 1);
            if (ret_read_val == -1) { 
                write(2, "Cannot read to_srv.txt\n", 23); 
                close(fid_srv_file);
                exit(-1); 
            }
            strncat(srv_file_txt, &curr_data, 1);
        }
        
        int pid_rm = fork(); // fork to rm to_srv.txt
        if (pid_rm == 0) { // child to rm to_srv.txt
            execl("/bin/rm", "/bin/rm", "to_srv.txt", NULL);
        }
        
        else { // father - continue operation
            waitpid(pid, &status, 0);
            // get data from the string we read from file
            s_pid_client = strtok(srv_file_txt, ","); // get pid of client
            s_num1 = strtok(NULL, ",");       // get first number as string
            operation = strtok(NULL, ",");    // get the operation
            s_num2 = strtok(NULL, ".");       // get second number as string

            // open to_client_pid file
            strcpy(client_file_name, "to_client_");
            strcat(client_file_name, s_pid_client);
            strcat(client_file_name, ".txt");
            fid_client_file = open(client_file_name, O_RDWR | O_CREAT | O_APPEND, 0666);
            if (fid_client_file == -1) { 
                write(2, "Cannot open to_client.txt\n", 26); 
                close(fid_srv_file);
                exit(-1); 
            }

            // preform math operation
            int num1, num2, result, flag = 0;
            char result_msg[SIZE_FILE];
            num1 = atoi(s_num1); // str -> int
            num2 = atoi(s_num2); // str -> int

            switch (operation[0]) {
                case '1':
                    result = num1 + num2;
                    break;

                case '2':
                    result = num1 - num2;
                    break;

                case '3':
                    result = num1 * num2;
                    break;

                case '4':
                    result = num1 / num2;
                    break;
                
                default:
                    strcpy(result_msg, "Invalid operation.");
                    flag = 1; // error
                    break;
            }
            if (flag == 0) { // succed calculation
                char s_result[20];
                strcpy(result_msg, "The result is, ");
                sprintf(s_result, "%d", result);
                strcat(result_msg, s_result);
                strcat(result_msg, ".");
            }

            // write to to_client_pid file
            write(fid_client_file, result_msg, strlen(result_msg));

            // modify client that we finished and exit
            close(fid_client_file);
            pid_t pid_client = (pid_t)atoi(s_pid_client);
            int ret_val_kill = kill(pid_client, SIGUSR2);
            if (ret_val_kill == -1) { write(2, "This pid does not exist\n", 24); exit(-1); }
            exit(0);
        }
   }
   else { // father
        signal(SIGCHLD, SIG_IGN); // prevent zombies
   }
}


int main(int argc, char* argv[]) {
    int status, exist, pid;
    exist = access("to_srv.txt", F_OK); // check if exists
    if (exist == 0) { // if exists, create a child to remove it
        pid = fork();
        if (pid == 0) { // child
            execl("/bin/rm", "/bin/rm", "to_srv.txt", NULL);
        }
        else { // father
            waitpid(pid, &status, 0);
        }
    }
    signal(SIGUSR1, handleClient);
    while(1) {
        pause(); // Waiting to answer from the client
    }

    exit(0);
}