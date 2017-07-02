#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#define MAX_LEN 1024
struct filelist
{
	char *name; //file name
};

union semun {
 int val;                  /* значение для SETVAL */
 struct semid_ds *buf;     /* буферы для  IPC_STAT, IPC_SET */
 unsigned short *array;    /* массивы для GETALL, SETALL */
                           /* часть, особенная для Linux: */
 struct seminfo *__buf;    /* буфер для IPC_INFO */
};

/* сумма элементов массива в строке */
void runProg(char * str){
	for(int i = 0; i<10; i++)
	{
		printf("\nprogramm %d started", i);
		execl(str, NULL);
		printf("\nprogramm %d completed", i);
	}
}	

void doProgramms(){

	char cwd[1024];
	DIR *cd;
	int cntobf = 0;
	pid_t pid[cntobf];
	pid_t wpid;
	int status = 0;
	
	int shmid;
	key_t key = 69;
	double *shm;
	int n=0, m=0;
	struct filelist flist[256];//, dirlist[256];//dirlist ne nuzhen
	struct dirent * cf; // number of file and index descriptor
	struct stat fs;	// file parameters
	getcwd (cwd, sizeof(cwd)); // curent dirrectory
	strcat (cwd, "/"); // add "/" to end of catalog
	
	int semid;
	union semun arg;
	struct sembuf lock_res = {0, -1, 0};	//блокировка ресурса
	struct sembuf rel_res = {0, 1, 0};	//освобождение ресурса
	
	 /* Получим ключ, Один и тот же ключ можно использовать как
    для семафора, так и для разделяемой памяти */
	if ((key = ftok(".", 'S')) < 0) {
		printf("Невозможно получить ключ\n");
		exit(1);
	}
	
	/* Создадим семафор - для синхронизации работы с разделяемой памятью.*/
	semid = semget(key, 1, 0666 | IPC_CREAT);
	
	/* Установить в семафоре № 0 (Контроллер ресурса) значение "1" */
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);
	 
	 /* Создадим область разделяемой памяти */
	if ((shmid = shmget(key, sizeof(double), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		printf("not created place for devided memory\n");
		exit(1);
	}
	if (cd = opendir(cwd)) // open current directory
				{
					printf("directory opened\n");
					while(cf = readdir(cd)) // read currnt directory
					{
						printf("readed curent directory\n");
						stat(cf->d_name, &fs);
						if (fs.st_mode&S_IFREG) // add file to array
						{
							printf("added file to array\n");
							flist[n].name = cf->d_name; // d.name
							n++;
							cntobf++;
						}
					};
					closedir(cd);
				}
  	
	for (int i = 0; i < 10; i++){
			pid[i] = fork();
			printf("%d\n", (int)pid[i]);
			srand(getpid());
			char * str = malloc(MAX_LEN * sizeof(char));
			if (0 == pid[i]) {
				printf("PID=%d i=%d\n", getpid(), i);
				if (cd = opendir(cwd)) // open current directory
				{
					printf("directory opened\n");
					while(cf = readdir(cd)) // read currnt directory
					{
						printf("readed curent directory\n");
						stat(cf->d_name, &fs);
						if (fs.st_mode&S_IFREG) // add file to array
						{
							printf("added file to array\n");
							flist[n].name = cf->d_name; // d.name
							n++;
							cntobf++;
						}
					};
					closedir(cd);
				}
				printf("%s", flist[n].name);
				fflush(stdout);
				int ran = rand() % cntobf;
				printf("%d", cntobf);
				if(flist[n].name != "7")
				runProg(flist[ran].name);
				fflush(stdout);
			
				/* Получим доступ к разделяемой памяти */
				if ((shm = (double *)shmat(shmid, NULL, 0)) == (double *) -1) {
					perror("shmat");
					printf("get order to memory\n");
					exit(1);
				}
				
				printf("Процесс ожидает PID=%d i=%d\n", getpid(), i);
				fflush(stdout);
				
				/* Заблокируем разделяемую память */	
				if((semop(semid, &lock_res, 1)) == -1){
					exit(1);
				} else{
					printf("Semaphore resources decremented by one (locked) i=%d\n", i);
					fflush(stdout);
				}
				
				sleep(rand() % 2);
				 
				/* Освободим разделяемую память */
				if((semop(semid, &rel_res, 1)) == -1){
						 exit(1);
				} else{
					printf("Semaphore resources incremented by one (unlocked) i=%d\n", i);
					fflush(stdout);
				}
				
				printf("Math avarage [i=%d] = %f\n", i, *(shm));
				fflush(stdout);
				
				 /* Отключимся от разделяемой памяти */
				if (shmdt(shm) < 0) {
					printf("Ошибка отключенияn");
					exit(1);
				}
				exit(0);
			}	else if (pid[i]<0){
					perror("fork"); /* произошла ошибка */
					exit(1); /*выход из родительского процесса*/
			}
	}	
	
	for (int i = 0; i < cntobf; i++) {
		wpid = waitpid(pid[i], &status, 0);
		if (pid[i] == wpid) {
			printf("процесс-потомок %d done,  result=%d\n", i, WEXITSTATUS(status));
			fflush(stdout);
		}
	}	

	/* Получим доступ к разделяемой памяти */
	if ((shm = (double*)shmat(shmid, NULL, 0)) == (double *) -1) {
		perror("shmat");
		exit(1);
	}

	printf("------------------\n");
	
	fflush(stdout);

	if (shmdt(shm) < 0) {
		printf("Ошибка отключения\n");
		exit(1);
	} 
	
	/* Удалим созданные объекты IPC */	
	 if (shmctl(shmid, IPC_RMID, 0) < 0) {
		printf("Невозможно удалить область\n");
		exit(1);
	} else
		printf("Shared memory segment marked for deletion\n");
	
	if (semctl(semid, 0, IPC_RMID) < 0) {
		printf("Невозможно удалить семафор\n");
		exit(1);
	}
	
}	

int main(int argc, char **argv){
	doProgramms();
	return 0;
}