#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

//Limites del juego
#define MAX_PISTOLEROS 128
#define MIN_PISTOLEROS 3

// CREADORES:
// ÁLVARO GARCIA SANCHEZ DNI:70924450V
// CARLOS BLÁZQUEZ MARTÍN DNI:70837164Q

int creaPistoleros(int, pid_t*);


int main(int argc, char *argv[]){

void esperar_al_desbloqueo_del_fichero();

    int num_pistoleros, i, fd, codigo;
    char buffer[100]; // Buffer para escribir por pantalla mediante llamadas al sistema y para el tratamiento de datos entre ficheros
    char str[100];
    pid_t pid_padre;
    
    pid_t pids[MAX_PISTOLEROS]; // vector donde se van a guardar los PID de los procesos para que el padre los pueda escribir en el fichero
    pid_t pids_leidos[MAX_PISTOLEROS];
    
    //Mascaras
    sigset_t bloquear;
    
    sprintf(buffer, "\n============================\n=LOS PISTOLEROS DE KENTUCKY=\n============================\n\n");
    write(1, buffer, strlen(buffer));
    
    // ================================
    // =COMPROBACIONES PREVIAS        =
    // ================================
    if(argc != 2)
    {
    	fprintf(stderr, "ERROR: Numero de argumentos incorrecto\n");
    	fprintf(stderr, "Uso: %s numero_de_pistoleros(%d-%d)\n\n", argv[0], MIN_PISTOLEROS, MAX_PISTOLEROS);
    	return 1;
    }
    else
    {
        // Se guarda el numero de pistoleros introducido por el usuario en la variable num_pistoleros
        num_pistoleros = atoi(argv[1]);
        
        // Compobacion de rangos
        if(num_pistoleros<MIN_PISTOLEROS || num_pistoleros>MAX_PISTOLEROS)
        {
        	fprintf(stderr, "ERROR: Numero de pistoleros (%d) incorrecto:\n", num_pistoleros);
        	fprintf(stderr, "Rango valido: %d-%d\n\n", MIN_PISTOLEROS, MAX_PISTOLEROS);
        	return 1;
        }
        
        // ================================
        // =EJECUCION NORMAL DE LA PARTIDA=
        // ================================
        sprintf(buffer, "Numero de pistoleros: %d\n", num_pistoleros);
        write(1, buffer, strlen(buffer)); // Escribimos el mesaje por pantalla
        
        pid_padre = getpid();// Se guarda el PID del proceso padre en la variable pid
        
        // Creamos todos los pistoleros (procesos hijos del padre)
        creaPistoleros(num_pistoleros, pids);
        
        if(getppid()!=pid_padre) //Zona accesible por el padre
        { 
            
            sprintf(buffer, "Pids de los pistoleros: \n");
            write(1, buffer, strlen(buffer)); // Escribimos el mesaje por pantalla

            // Creacion del fichero que contendrá los PID del juego
            fd = open("PIDS.txt", O_RDWR | O_CREAT | O_TRUNC);
            if (fd == -1) // En caso de error en la apertura del fichero
            {
                perror("pistolos:open");
                return 1;
            }    
            // El padre imprime los pids de los hijos por pantalla
            for(i=0; i<num_pistoleros; i++)
            {
                sprintf(buffer, "%d ", pids[i]);
                write(1, buffer, strlen(buffer));
            }
            // El padre guarda los pids en un fichero
            // Se bloquea el fichero
            lockf(fd, F_LOCK, 0);
            // Se escriben los pids de los procesos hijos en el fichero
            for(i=0; i<num_pistoleros; i++)
            {
                sprintf(buffer, "%d\n", pids[i]);
                write(fd, buffer, strlen(buffer));
            }
            // Movemos el puntero del fichero al principio
            lseek(fd, 0, SEEK_SET);
            // Se desbloquea el fichero
            lockf(fd, F_ULOCK, 0);
            // Se cierra el fichero
            //close(fd);
            pause();
        }
        else  // Zona accesible por los hijos
        {
            // El proceso hijo abre el fichero
            fd = open("PIDS.txt", O_RDONLY);
            lockf(fd, F_TEST, 0); // Si deja bloquearlo es que ya no esta bloqueado por el padre
            if(errno==EACCES)
            {
                esperar_al_desbloqueo_del_fichero();
            }
            else
            {
                /*//Leemos el fichero de pids
                if(read(fd, buffer, strlen(buffer))==-1)
                {
                    perror("pistolos:read");
                }
                for(i=0; i<num_pistoleros; i++)
                {
                    sprintf(str, "%s", buffer);
                    pids_leidos[i]=atoi(str);
                }*/
            }
        }
        pause();
    }
}


int creaPistoleros(int num_pistoleros, pid_t *pids)
{
    pid_t pid;
    if(num_pistoleros>0)
    {
        pid = fork();
        switch(pid)
        {
            case -1:
                perror("pistolos:fork");
                break;
            case 0: // Hijo
                break;
            default: // Padre
                *pids = pid; // Guardamos el pid en el vector de pids
                if(creaPistoleros(num_pistoleros-1, (pids+1))==1) // En caso de que ya haya creado todos los pistoleros
                {
                    //pause();
                }
        }
    }
    else
    {return 1;}
}

void esperar_al_desbloqueo_del_fichero(){
    
}





