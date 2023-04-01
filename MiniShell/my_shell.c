/*
PRACTICA 2  -   SISTEMAS OPERATIVOS
GRUPO: LA PEPA
Mario Ventura, Luis Miguel Vargas, Felip Toni Font
12-2021
*/

//__________________________________________________________________________________________________________________
//                                                      NIVEL D
//__________________________________________________________________________________________________________________


//---------------------------------------------------INCLUDE---------------------------------------------------
#define _POSIX_SOURCE  //Para que no salten warnings al hacer kill 
#include <errno.h>  //errno
#include <stdio.h>  //printf(), fflush(), fgets(), stdout, stdin, stderr, fprintf()
#include <stdlib.h> //setenv(), getenv()
#include <string.h> //strcmp(), strtok(), strerror()
#include <unistd.h> //NULL, getcwd(), chdir()
#include <sys/types.h> //pid_t
#include <sys/wait.h>  //wait()
#include <signal.h> //signal(), SIG_DFL, SIG_IGN, SIGINT, SIGCHLD
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR


//---------------------------------------------------DEFINE---------------------------------------------------------
#define _POSIX_C_SOURCE 200112L
#define DEBUGN1 1 //parse_args()
#define DEBUGN3 1 //execute_line()
#define PROMPT_PERSONAL 1 // si no vale 1 el prompt será solo el carácter de PROMPT
#define RESET_FORMATO "\033[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define GRIS "\x1b[94m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define NEGRITA "\x1b[1m"
#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 24 // cantidad de trabajos permitidos

//------------------------------------------DECLARACIÓN DE CONSTANTES------------------------------------------------
char const PROMPT = '$';            //char para el prompt del shell
char mensaje[2500];                 //Mensaje que se mostrará en comentarios de debugging

//------------------------------------------DECLARACIÓN DE VARIABLES------------------------------------------------
//variable global para guardar el nombre del minishell
static char mi_shell[COMMAND_LINE_SIZE]; 
//static pid_t foreground_pid = 0;
struct info_process {
    //STRUCT CON LOS ATRIBUTOS QUE DEFINEN UN PROCESO: pid, estado y línea de comando asociada
	pid_t pid; //pid del proceso
	char status; //'N','E','D','F': Ninguno, Ejecutándose, Detenido, Finalizado
	char cmd[COMMAND_LINE_SIZE]; //Línea de comando asociada
};
//Tabla de procesos. La posición 0 será para el foreground
static struct info_process jobs_list[N_JOBS];
//VARIABLE QUE REPRESENTA EL NUMERO DE TRABAJOS NO FINALIZADOS
int n_pids = 0;

//------------------------------------------DECLARACIÓN DE FUNCIONES------------------------------------------------
int check_internal(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs();
int internal_bg(char **args);
int internal_fg(char **args);
char *read_line(char *line);
int parse_args(char **args, char *line);
int execute_line(char *line);
void imprimir_prompt();
void reaper(int signum);
void ctrlc(int signum);
int is_background(char **args);
int jobs_list_add(pid_t pid, char status, char *cmd);
int jobs_list_find(pid_t pid);
int  jobs_list_remove(int pos);
void ctrlz(int signum);
int is_output_redirection (char **args);


//---------------------------------------------DESARROLLO DE LAS FUNCIONES---------------------------------------------

int check_internal(char **args) {
    if (!strcmp(args[0], "cd"))
        return internal_cd(args);
    if (!strcmp(args[0], "export"))
        return internal_export(args);
    if (!strcmp(args[0], "source"))
        return internal_source(args);
    if (!strcmp(args[0], "jobs"))
        return internal_jobs(args);
    if (!strcmp(args[0], "bg"))
        return internal_bg(args);
    if (!strcmp(args[0], "fg"))
        return internal_fg(args);
    if (!strcmp(args[0], "exit"))
        exit(0);
    return 0; // no es un comando interno
}


int internal_cd(char **args) {
    printf("[internal_cd()→ comando interno no implementado]\n");
    return 1;
} 


int internal_export(char **args) {
    printf("[internal_export()→ comando interno no implementado]\n");
    return 1;
}


int internal_source(char **args) {
    printf("[internal_source()→ comando interno no implementado]\n");
    return 1;
}


int internal_jobs(char **args) {
    //MIRAR SI SE HA ESCRITO JOBS EN LA TERMINAL (ya que este es un comando externo para ver todos los 'jobs' en ese instante,
    //por tanto, si escribe 'jobs' se iterará la lista y se mostrará todo; y en caso contrario, solo args[0])
    int iterar = strcmp(args[0],"jobs");
    if (iterar == 0) {
        //RECORRER LISTA PARA MOSTRAR TODOS LOS JOBS
        for (int ind = 1; (jobs_list[ind].pid>0 && ind<=n_pids); ind++) {
            //IMPRIMIR EN PANTALLA LOS DATOS
            sprintf(mensaje, VERDE_T "[%d], %d, %s, %c\n"RESET_FORMATO, ind, jobs_list[ind].pid,jobs_list[ind].cmd,jobs_list[ind].status);
            write(2,mensaje,strlen(mensaje));
        }
    } else {
        //MOSTRAR SOLO args[0]
        sprintf(mensaje, VERDE_T "[%d], %d, %s, %c\n" RESET_FORMATO, n_pids, jobs_list[n_pids].pid, jobs_list[n_pids].cmd, jobs_list[n_pids].status);
        write(2, mensaje, strlen(mensaje));
    }
    return 1;
}

//ENVIA TRABAJO DETENIDO A FOREGROUND
int internal_fg(char **args) {
    //POSICION
    int pos = 0;
    //QUITAR LOS '&' SI ES QUE LOS HAY
    pos = (int) strtol((char*)args[1],NULL,10);
    //COMPARAR pos Y n_pids
    if((pos >= n_pids+1)||(pos == 0)) {
        return pos;
    }
    //SI SU STATUS ES 'D'
    if (jobs_list[pos].status == 'D') {
        //ENVIAR SEÑAL Y NOTIFICAR POR PANTALLA
        kill(jobs_list[pos].pid, SIGCONT);
    }

    //COPIAR DATOS DE JOBSLIST[POS] A JOBSLIST[0]
    jobs_list[0] = jobs_list[pos];
    //ELIMINAR '&'
    for (int i=0; i<COMMAND_LINE_SIZE; i++) {
        if(jobs_list[pos].cmd[i] == '&') {
            jobs_list[pos].cmd[i-1] = '\0';
            jobs_list[pos].cmd[i] = '\0';
        }
    }
    
    //ELIMINAR jobslist[pos]
    jobs_list_remove(pos);
    //SI HAY ALGUN PROCESO EN FOREGROUND (pid > 0), HACER UN pause()
    if (jobs_list[0].pid > 0) {
        pause();
    }

    return 1;
}

//REACTIVA UN PROCESO DETENIDO QUE ESTÁ EJECUTÁNDOSE EN SEGUNDO PLANO
int internal_bg(char **args) {
    //POSICION
    int pos = 0;
    pos = (int) strtol((char*)args[1],NULL,10);
    //COMPARAR pos Y n_pids
    if((pos >= n_pids+1)||(pos == 0)) {
        //NOTIFICAR ERROR Y RETORNAR
        //fprintf(stderr, "internal_bg() --> NO EXISTE ESTE TRABAJO\n"RESET_FORMATO);
    } else {
        if (jobs_list[pos].status == 'E') {
            //SI EL status ES 'E' ENTONCES ERROR Y SALIR
            //fprintf(stderr, "internal_bg() --> EL TRABAJO YA ESTÁ EN 2º PLANO\n" RESET_FORMATO);
        }
        jobs_list[pos].status = 'E';
        //AÑADIR '&' AL CMD
        for (int i = 0; i < COMMAND_LINE_SIZE; i++) {
            if (jobs_list[pos].cmd[i] == '\0') {
                jobs_list[pos].cmd[i] = ' ';
                jobs_list[pos].cmd[i + 1] = '&';
                i = COMMAND_LINE_SIZE; //PARA NO SEGUIR ITERANDO
            }
        }
        //ENVIAR SEÑAL SIGCONT
        kill(jobs_list[pos].pid, SIGCONT);
    }
    return 1;
}


void imprimir_prompt() {
#if PROMPT_PERSONAL == 1
    printf(NEGRITA ROJO_T "%s" BLANCO_T ":", getenv("USER"));
    printf(AZUL_T "MINISHELL" BLANCO_T "%c " RESET_FORMATO, PROMPT);
#else
    printf("%c ", PROMPT);

#endif
    fflush(stdout);
    return;
}


char *read_line(char *line) {
  
    imprimir_prompt();
    char *ptr=fgets(line, COMMAND_LINE_SIZE, stdin); // leer linea
    if (ptr) {
        // ELiminamos el salto de línea (ASCII 10) sustituyéndolo por el \0
        char *pos = strchr(line, 10);
        if (pos != NULL){
            *pos = '\0';
        } 
	}  else {   //ptr==NULL por error o eof
        printf("\r");
        if (feof(stdin)) { //se ha pulsado Ctrl+D
            fprintf(stderr,"Bye bye\n");
            exit(0);
        }   
    }
    return ptr;
}


int parse_args(char **args, char *line) {
    int i = 0;

    args[i] = strtok(line, " \t\n\r");
    #if DEBUGN1 
        //fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
    #endif
    while (args[i] && args[i][0] != '#') { // args[i]!= NULL && *args[i]!='#'
        i++;
        args[i] = strtok(NULL, " \t\n\r");
        #if DEBUGN1 
            //fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
        #endif
    }
    if (args[i]) {
        args[i] = NULL; // por si el último token es el símbolo comentario
        #if DEBUGN1 
            //fprintf(stderr, GRIS "[parse_args()→ token %i corregido: %s]\n" RESET_FORMATO, i, args[i]);
        #endif
    }
    return i;
}

//PERMITIR EJECUTAR COMANDOS EXTERNO
int execute_line(char *line) {
    char *args[ARGS_SIZE];
    pid_t pid;
    char command_line[COMMAND_LINE_SIZE];

    //copiamos la línea de comandos sin '\n' para guardarlo en el array de structs de los procesos
    memset(command_line, '\0', sizeof(command_line)); 
    strcpy(command_line, line); //antes de llamar a parse_args() que modifica line

    if (parse_args(args, line) > 0) {
        //COMPROBAR SI ES UN PROCESO INTERNO
        if (check_internal(args)) {
            return 1;
        } else {
            //HACER LLAMADA A LA FUNCIÓN ISBACKGROUND 
            int tipo = is_background(args);
            //HACER FORK PARA OBTENER PID DE PADRE E HIJOS
            pid = fork();
            //COMPROBAR SI ES EL PROCESO PADRE O HIJO
            if (pid == 0) {
                //ES EL PROCESO HIJO
                signal(SIGCHLD,SIG_DFL);
                //Ignorar SIGINT con signal (Cambiar comportamiento por defecto)
                signal(SIGINT,SIG_IGN); //Solo el padre hace uso de la función ctrlc(int signum)
                //INDICAR CON SIGNAL QUE SE IGNORARÁ LA SEÑAL SIGTSTP
                signal(SIGTSTP,SIG_IGN); //Solo el padre ejecuta el manejador de la señal (señal de ctrl+Z)
                //LLAMADA A FUNCION IS OUTPUT REDIRECTION
                is_output_redirection(args);      //Para ver si encuentra el token
                //Ejecutar comando externo con la función execvp()
                execvp(line, args);
                //realizar un exit(-1);
                exit(-1);

            } else if (pid > 0) {
                //ES EL PROCESO PADRE
                signal(SIGINT,ctrlc);
                signal(SIGTSTP,ctrlz);
                
                //COMPROBAR SI ES UN PROCESO BACKGROUND O FOREGROUND
                if (tipo == 1) {
                    //---------ES BACKGROUND---------
                    jobs_list_add(pid,'E',command_line);
                    internal_jobs(args);
                } else if (tipo == 0) {
                    //---------ES FOREGROUND---------
                    //ACTUALIZAR LOS DATOS DE jobs_list[0] CON LOS DEL PROCESO HIJO EN FOREGROUND
                    strcpy(jobs_list[0].cmd, command_line); //Actualizar linea de comando
                    jobs_list[0].status = 'E';              //Obligatorio establecer que está ejecutandose
                    jobs_list[0].pid = pid;                 //Por si acaso, también establecer:ç
                    //SI HAY UN PROCESO HIJO EJECUTÁNDOSE EN PRIMER PLANO, EL PADRE ESPERA (FUNCIÓN pause())
                    pause();    //ESPERAR SEÑALES
                }        

            } else {
                //HA HABIDO ERROR AL HACER FORK
                perror("ERROR: ");
            }
        }
    }
    return 0;
}

//MÉTODO MAIN
int main(int argc, char *argv[]) {
    //argv[0] contiene el nombre del programa
    //argv[1] es un puntero a la primera línea de comandos proporcionada
    //argv[n] es el último argumento

    //Variable que representa una linea del minishell
    char line[COMMAND_LINE_SIZE];
    //Pone en el string line, 
    memset(line, 0, COMMAND_LINE_SIZE);

    //CREAR EL NOMBRE DEL MINISHELL
    strcpy(mi_shell,argv[0]);

    //MANEJADORES SIGCHLD,SIGINT
    signal(SIGCHLD,reaper); //Asociar señal a reaper(int signum)
    signal(SIGINT,ctrlc);   //Asociar señal a ctrlc(int signum)
    signal(SIGTSTP, ctrlz); //Asociar señal al manejador ctrlz(int signum)

    //INICIALIZAR LOS DATOS DE jobs_list[0] CON pid=0, status='N', Y cmd CON TODO A '\0'
    jobs_list[0].pid = 0;                           //pid a 0
    jobs_list[0].status = 'N';                      //status a 'N' (Ninguno)
    for(int i=0; i < COMMAND_LINE_SIZE; i++) {
        jobs_list[0].cmd[i] = '\0';                 //cmd = '\0'
    } 

    //ITERACIÓN PARA QUE EL SHELL SE EJECUTE
    while (1) {
        if (read_line(line)) { // !=NULL
            execute_line(line);
        }
    }
    return 0;
}

//MÉTODO REAPER (ENTERRADOR)
void reaper(int signum) {
    //MANEJADOR PROPIO DE SEÑAL ENVIADA A UN PROCESO CUANDO UNO DE SUS PROCESOS HIJOS TERMINA
    signal(SIGCHLD,reaper); //Volver a poner la instrucción
    //VARIABLES AUXILIARES
    int status;                 //Status
    int acabado;                //Indica si ha acabado

    //COMPROBAR QUÉ PROCESO HIJO TERMINA SU EJECUCIÓN (si el hijo que acaba es el que está en primer plano)
    while ((acabado=waitpid(-1, &status, WNOHANG)) > 0) {
        //VER SI EL PROCESO QUE HA ACABADO ESE EJECUTABA EN FOREGROUND
        if(jobs_list[0].pid == acabado) {
            //---------------FOREGROUND---------------
            //RESETEAR pid, PONER status EN 'F', ESTABLECER cmd[indice] A '\0'
            for (int indice = 0; indice < COMMAND_LINE_SIZE; indice++) {
                jobs_list[0].cmd[indice] = '\0';
            }
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
        } else {
            //---------------BACKGROUND---------------
            //BUSCAR pid DEL PROCESO EN jobs_list 
            int finalizado = jobs_list_find(acabado);
            //ELIMINAR EL PROCESO DE LA LISTA
            jobs_list_remove(finalizado);
        }
    }
    //VACÍA EL BUFFER DE SALIDA
    fflush(stdout);
}

//MÉTODO CTRLC
void ctrlc(int signum) {
    //MANEJADOR PROPIO PARA LA SEÑAL SIGINT (ctrl+C)
    signal(SIGINT,ctrlc); //Volver a poner la instrucción

    /*
    para que no afecte también a los de segundo plano, enviaremos a través del padre una señal diferente, SIGTERM,
    para controlar la terminación sólo del proceso en primer plano, siempre y cuando éste no sea nuestro propio mini shell
    */

    //HAY PROCESO EN FOREGROUND? (pid>0)
    if (jobs_list[0].pid > 0) {
        //EN CASO DE HABERLO, ES EL MINI SHELL? (nombre_proceso_actual == nombre_minishell)
        int iguales = strcmp(jobs_list[0].cmd, mi_shell);
        if (iguales == 0) {
            //EL PROCESO EN FOREGROUND ES EL SHELL
            //sprintf(mensaje, VERDE_T "ctrlc() --> PROCESO EN FOREGOUND %d (%s) es el minishell: NO SE ENVÍA SEÑAL %d ]\n" RESET_FORMATO, getpid(),mi_shell,SIGTERM);
            //write(2,mensaje,strlen(mensaje));

        } else {
            //SE ENVÍA LA SEÑAL SIGTERM MEDIANTE LA FUNCIÓN KILL Y EL pid DEL PROCESO DETECTADO
            kill(jobs_list[0].pid,SIGTERM);
        }
    }
}

//MÉTODO isBackground
int is_background(char **args) {
    //DEVUELVE 1 SI LOCALIZA EL TOKEN '&' (background) Y 0 EN CASO CONTRARIO (foregroubnd)
    //RESULTADO
    int resultado = 0;

    //ENCONTRAR '&' ITERANDO args[0] (POSICIÓN 0 REPRESENTA EL PROCESO EN FOREGROUND)
    for(int i=0; args[i] && args[i][0] != '#'; i++) {
        //SI ES '&', SUSTITUIRLO POR NULL
        if(args[i][0] == '&') {
            //SUSTITUIR POR NULL
            args[i]=NULL;
            //DEVOLVER 1
            resultado = 1;
            return resultado;
        }
    }
    //SI NO SE ENCUENTRA SE DEVUELVE 0 (valor por defecto de la variable resultado)
    return resultado;
}

//SI NO HEMOS LLEGADO AL NUMERO MAXIMO DE TRABAJOS PERMITIDOS AÑADIMOS EL NUEVO ELEMENTO AL ARRAY
int jobs_list_add(pid_t pid, char status, char *cmd) {
    //COMPROBAR SI LA LISTA ESTA LLENA
    if (n_pids < N_JOBS) {
        //CABE UN TRABAJO MÁS, POR TANTO EL NUMERO DE pid's AUMENTA
        n_pids++;
        //AÑADIR EL NUEVO TRABAJO
        jobs_list[n_pids].pid = pid;
        jobs_list[n_pids].status = status;
        strcpy(jobs_list[n_pids].cmd,cmd);
    }
    return 0; 
}

//BUSCA EL TRABAJO CUYO pid ES EL QUE SE PASA POR PARÁMETRO Y DEVUELVE SU POSICION EN EL ARRAY
int jobs_list_find(pid_t pid) {
    int posicion;
    //ITERAR EL ARRAY
    for(posicion = 0; posicion < n_pids; posicion++) {
        //COMPROBAR SI SE HA ENCONTRADO EL TRABAJO
        if(jobs_list[posicion].pid == pid) {
            return posicion;
        }
    }
    return posicion;
}

//RECIBE COMO PARÁMETRO LA POSICIÓN DEL ARRAY DE TRABAJO QUE HAY QUE ELIMINAR Y MUEVE EL REGISTRO
int  jobs_list_remove(int pos) {
    //APLASTAR EL CONTENIDO DEL ARRAY EN POSICIÓN pos MOVIENDO AHI EL 'job' EN POSICION n_pids
    jobs_list[pos] = jobs_list[n_pids];
    //DECREMENTAR n_pids
    n_pids--;
    //DEVOLVER EL VALOR
    return n_pids;
}

//FUNCIÓN QUE CONTROLA LA SEÑAL DE ctrl+Z
void ctrlz(int signum) {
    //ASOCIAR SEÑAL OTRA VEZ
    signal(SIGTSTP,ctrlz);
    //VARIABLES AUXILIARES
    int comparacion;        //Resultado de comparaciones
    printf("\n");           //Para que quede mejor

    //MIRAR SI HAY PROCESO EN FOREGROUND (pid > 0)
    if (jobs_list[0].pid > 0) {

        //MIRAR SI ES EL MINISHELL
        comparacion = strcmp(jobs_list[0].cmd,mi_shell);
        if (comparacion != 0) {
            //ENVIAR SEÑAL SIGSTOP
            kill(jobs_list[0].pid,SIGSTOP);
            //CAMBIAR STATUS
            jobs_list[0].status = 'D';
            //AÑADIR LOS DATOS DEL PROCESO DETENIDO A jobs_list[n_pids]
            jobs_list_add(jobs_list[0].pid,jobs_list[0].status,jobs_list[0].cmd);
            //RESETEAR LOS DATOS DE jobs_list[0]
            for (int indice = 0; indice < COMMAND_LINE_SIZE; indice++) {
                jobs_list[0].cmd[indice] = '\0';
            }
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F'; 
        } 
    }
}

//FUNCIÓN BOOLEANA QUE RECORRE LA LISTA DE ARGUMENTOS EN BUSCA DE UN TOKEN '>' SEGUIDO DE...
int is_output_redirection (char **args) {
    //VARIABLES QUE SE USARÁN A LO LARGO DE LA FUNCIÓN
    int encontrado = 0;     //Actua como booleano
    int fd;
    char *file;
    
    //ITERACIÓN EN LA QUE SE COMPRUEBA QUÉ TOKEN SE LEE
    for (int i = 1;(args[i]) && (args[i][0] != '#') && (encontrado == 0); i++) {
        //MIRAR SI EL TOKEN ES EL QUE SE BUSCA
        if (args[i][0] == '>') {
            args[i] = '\0';
            file = args[i+1];
            encontrado = 1;     //PARA NO SEGUIR ITERANDO    
        }
    }
    //SI ES UN REDIRECCIONAMIENTO
    if (encontrado == 1) {
        fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        dup2(fd,1);
        close(fd);
    }
    //DEVOLVER EL RESULTADO DE LA BÚSQUEDA
    printf("\n");           //Para simular una consola
    return encontrado;      //Return boolean encontrado
}


//__________________________________________________________FIN DEL NIVEL D______________________________________________________