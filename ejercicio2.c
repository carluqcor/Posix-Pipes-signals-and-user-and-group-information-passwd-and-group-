#include <stdio.h>
#include <stdlib.h> //exit, rand, srand
#include <unistd.h> //pipe, close, fork, usleep, read, write, getpid, getppid
#include <sys/wait.h> //wait
#include <errno.h> //Control de errores
#include <string.h> //strerror
#include <time.h> //Para la semilla del generador de aleatorios

#define PAR_NAME "PAR"
#define IMPAR_NAME "IMPAR"

int main() 
{
pid_t flag;
int status, status2;
int fildes[2];
int pipe2[2];
const int BSIZE = 100;
char buf[BSIZE];
ssize_t nbytes, nbytes2;
char numero[50];
char *name;

status = pipe(fildes);
status2 = pipe(pipe2);
if (status == -1 ) 
{
	perror("Error en pipe 1"); 
	exit(EXIT_FAILURE);
}

if (status2 == -1 ) 
{
	perror("Error en pipe 2"); 
	exit(EXIT_FAILURE);
}


int rf=fork();
switch (rf) 
{
	case -1: // Ocurrió un error al hacer fork()
		perror("No se ha podido crear el proceso hijo...");
		exit(EXIT_FAILURE);

	case 0: // El hijo lee desde la tubería, no necesitamos escribir.
		if (close(fildes[1]) == -1) 
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
	
		printf("[HIJO]: mi PID es %d y mi PPID es %d\n", getpid(), getppid());


		// Leer usando READ
		nbytes=read(fildes[0], buf, BSIZE);
		if (nbytes == -1)
		{
			perror("Error en read"); 
			exit(EXIT_FAILURE);
		}
		else if (nbytes == 0) //read() no ha leido nada -> Se ha llegado a EOF -> El padre ha cerrado la tuberia
			printf("[HIJO]: Detecto que mi padre ha cerrado la tuberia 1...\n"); 
		else
			printf("[HIJO]: Leido %d de la tuberia 1.\n", atoi(buf)); 
	
		if (close(fildes[0]) == -1){
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
	
		printf("[HIJO]: Tuberia 1 cerrada\n\n");


		if (nbytes2 == -1){
			perror("Error en escritura"); 
			exit(EXIT_FAILURE);
		}
		
		if (close(pipe2[0]) == -1) 
		{
			perror("Error en Escritura"); 
			exit(EXIT_FAILURE);
		}

		if((atoi(buf)%2)==0){
			name=PAR_NAME;
		}
		else{
			name=IMPAR_NAME;
		}

		if(write(pipe2[1], name, 20)){
			printf("[HIJO]: Enviando %s por tuberia 2.\n", name); 
			}
		if (close(pipe2[1]) == -1) //Se termina de escriber -> Cerramos filfes[0]
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
		else
			printf("[HIJO]: Tuberia 2 cerrada.\n\n"); 
		exit(EXIT_SUCCESS);

	
	default: 		
		// El padre no lee
		if (close(fildes[0]) == -1) 
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}

		// El padre escribe en la tubería
		printf ("[PADRE]: mi PID es %d y el PID de mi hijo es %d \n", getpid(), rf);


		// Escribimos datos en la tubería
		printf("Introduzca un numero entero --> ");
		scanf("%s", numero);
		printf("\n");
		
		if ( write(fildes[1], numero, 20) == -1) //El padre escribe por la tuberia 1
		{
			perror("Error en write"); 
			exit(EXIT_FAILURE);
		}

		printf("[PADRE]: Enviando %s por la tuberia 1.\n", numero); 
				
		// El hijo verá FEOF (por hacer close)
		if (close(fildes[1]) == -1) 
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
		else
			printf("[PADRE]: Tuberia 1 cerrada.\n\n"); //El padre cierra su parte de la tuberia 1

		// El padre lee por la tuberia 2
		if (close(pipe2[1]) == -1) 
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
		
		nbytes2=read(pipe2[0], buf, BSIZE); //El padre lee desde la tuberia 2
		
		if (nbytes2 == -1){
			perror("Error en read"); 
			exit(EXIT_FAILURE);
		}else if (nbytes2 == 0) //read() no ha leido nada -> Se ha llegado a EOF -> El hijo ha cerrado la tuberia
		printf("[PADRE]: Detecto que mi hijo ha cerrado la tuberia 2...\n"); 
		else{
			printf("[PADRE]: Leyendo %s de la tuberia 2: \n", buf);
		}

		if (close(pipe2[0]) == -1)
		{
			perror("Error en close"); 
			exit(EXIT_FAILURE);
		}
		else{
			printf("[PADRE]: Tuberia 2 cerrada.\n\n"); //El padre cierra su parte de la tuberia
		}



		
			
		while ( (flag=wait(&status)) > 0 ) 
		{
			if (WIFEXITED(status)) 
			{
				printf("[PADRE]: Hijo con PID %ld finalizado, status = %d\n", (long int)flag, WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {  //Para seniales como las de finalizar o matar
				printf("[PADRE]: Hijo con PID %ld finalizado al recibir la señal %d\n", (long int)flag, WTERMSIG(status));
			} else if (WIFSTOPPED(status)) { //Para cuando se para un proceso. Al usar wait() en vez de waitpid() no nos sirve.
				printf("[PADRE]: Hijo con PID %ld parado al recibir la señal %d\n", (long int)flag,WSTOPSIG(status));
			} else if (WIFCONTINUED(status)){ //Para cuando se reanuda un proceso parado. Al usar wait() en vez de waitpid() no nos sirve.
				printf("[PADRE]: Hijo con PID %ld reanudado\n",(long int) flag);		  
			}	
		}
		if (flag==(pid_t)-1 && errno==ECHILD)
		{
			printf("[PADRE]: valor de errno = %d, definido como %s, no hay más hijos que esperar!\n", errno, strerror(errno));
		}
		else
		{
			printf("Error en la invocacion de wait o la llamada ha sido interrumpida por una señal.\n");
			exit(EXIT_FAILURE);
		}
		
		exit(EXIT_SUCCESS);	
	}//switch		 
}
