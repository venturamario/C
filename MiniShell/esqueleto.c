/*
nivelA.c - Adelaida Delgado (adaptación de nivel3.c)
Cada nivel incluye la funcionalidad de la anterior.
nivel A: Muestra el prompt, captura la línea de comandos, 
la trocea en tokens y chequea si se trata de comandos internos 
(cd, export, source, jobs, fg, bg o exit). Si son externos los ejecuta con execvp()
*/

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

//CONSTANTE CHAR PARA EL PROMPT DEL SHELL
char const PROMPT = '$';

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

//DECLARACIÓN DE FUNCIONES
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

//variable global para guardar el nombre del minishell
static char mi_shell[COMMAND_LINE_SIZE]; 

//static pid_t foreground_pid = 0;
struct info_process {
	pid_t pid; //pid del proceso
	char status; //'N','E','D','F': Ninguno, Ejecutándose, Detenido, Finalizado
	char cmd[COMMAND_LINE_SIZE]; //Línea de comando asociada
};

static struct info_process jobs_list[N_JOBS]; //Tabla de procesos. La posición 0 será para el foreground

void imprimir_prompt();

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
    #if DEBUGN1 
        printf("[internal_jobs()→ Esta función mostrará el PID de los procesos que no estén en foreground]\n");
    #endif
    return 1;
}

int internal_fg(char **args) {
    #if DEBUGN1 
        printf("[internal_fg()→ Esta función enviará un trabajo detenido al foreground reactivando su ejecución, o uno del background al foreground ]\n");
    #endif
    return 1;
}

int internal_bg(char **args) {
    #if DEBUGN1 
        printf("[internal_bg()→ Esta función reactivará un proceso detenido para que siga ejecutándose pero en segundo plano]\n");
    #endif
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
        fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
    #endif
    while (args[i] && args[i][0] != '#') { // args[i]!= NULL && *args[i]!='#'
        i++;
        args[i] = strtok(NULL, " \t\n\r");
        #if DEBUGN1 
            fprintf(stderr, GRIS "[parse_args()→ token %i: %s]\n" RESET_FORMATO, i, args[i]);
        #endif
    }
    if (args[i]) {
        args[i] = NULL; // por si el último token es el símbolo comentario
        #if DEBUGN1 
            fprintf(stderr, GRIS "[parse_args()→ token %i corregido: %s]\n" RESET_FORMATO, i, args[i]);
        #endif
    }
    return i;
}

int execute_line(char *line) {
    char *args[ARGS_SIZE];
    pid_t pid, status;
    char command_line[COMMAND_LINE_SIZE];

    //copiamos la línea de comandos sin '\n' para guardarlo en el array de structs de los procesos
    memset(command_line, '\0', sizeof(command_line)); 
    strcpy(command_line, line); //antes de llamar a parse_args() que modifica line

    if (parse_args(args, line) > 0) {
        if (check_internal(args)) {
            return 1;
        }
        #if DEBUGN3
            fprintf(stderr, GRIS "[execute_line()→ PID padre: %d]\n" RESET_FORMATO, getpid());
        #endif
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char line[COMMAND_LINE_SIZE];
    memset(line, 0, COMMAND_LINE_SIZE);
    while (1) {
        if (read_line(line)) { // !=NULL
            execute_line(line);
        }
    }
    return 0;
}