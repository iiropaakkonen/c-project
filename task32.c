#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
	//access shared memory
	printf("Accessing shared memory...\n");
	key_t key = ftok("shared_memory_key", 65);
	int shmid = shmget(key, 1024, 0666);
	char *shmptr = (char*) shmat(shmid, (void*)0, 0);
	
	
	//find the missing alphabets
	printf("Finding missing alphabets...\n");
	char missing[26] = "abcdefghijklmnopqrstuvwxyz";
	
	for (int i = 0; i < strlen(shmptr); i++) {
        	for (int j = 0; j < 26; j++) {
            		if (shmptr[i] == missing[j]) {
            		 	// Mark found alphabet as space
                		missing[j] = ' ';
                		break;
            		}
        	}
    	}
    	// Print missing
    	printf("Missing alphabets: ");
    	for (int i = 0; i < 26; i++) {
        	if (missing[i] != ' ') {
            		printf("%c", missing[i]);
        	}
    	}
    	printf("\n");
    	
    	//detach
    	shmdt(shmptr);
    	// Delete shared memory segment
    	shmctl(shmid, IPC_RMID, NULL);

    	return 0;
}
