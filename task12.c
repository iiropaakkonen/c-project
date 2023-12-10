#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/ipc.h>
#include <ctype.h>

int main() {
	char testString[100];
	printf("Test string: ");
	fgets(testString, sizeof(testString), stdin);
	
	// create a pipe array
	int pipeArray[2];
	pipe(pipeArray);
	
	// fork to create a child process
	printf("Forking...\n");
	pid_t child = fork();
	
	
	// in case child process forking fails:
	if(child == -1) {
		perror("Failed");
		exit(EXIT_FAILURE);
	}
	
	//child process
	if(child == 0) {
		printf("Child process created\n");
		// close write
		close(pipeArray[1]);
		// redirect
		dup2(pipeArray[0], STDIN_FILENO);
		//close read
		close(pipeArray[0]);
		
		char cleanString[100];
	
		//read string from the pipe
		fgets(cleanString, sizeof(cleanString), stdin);
		cleanString[strcspn(cleanString, "\n")] = '\0';
		printf("String read from pipe\n");
	
		//clean up the string
		char cleanedString[100];
		int cleanedIndex = 0;
	
		for(int i = 0; i < strlen(cleanString); i++) {
			char temp = cleanString[i];
			if(isalpha(temp) != 0) {
				cleanedString[cleanedIndex] = temp;
        	    		cleanedIndex++;
			}
		}
		cleanedString[cleanedIndex] = '\0';
		printf("String cleaned! Cleaned string:");
		printf("%s", cleanedString);
	
	
		//create shared memory
		key_t key = ftok("shared_memory_key", 65);
		int shmid = shmget(key, 1024, 0666|IPC_CREAT);
		char *shmptr = (char*) shmat(shmid, (void*)0, 0);
	
		//write the cleaned string to shared memory
		strcpy(shmptr, cleanedString);
	
		//detach
		shmdt(shmptr);
		
		//move to the missing alphabets task
		printf("\n Creating new process to find missing alphabets");
		execl("./task32", "./task32", NULL);
		perror("Failed to start task 3");
		exit(EXIT_FAILURE);
		
	}
	
	//parent process
	else {
		// close read
		close(pipeArray[0]);
		//write new string
		write(pipeArray[1], testString, sizeof(testString));
		close(pipeArray[1]);
		// wait for child process to finish task
		wait(NULL);
	}


	return 0;
	
}
