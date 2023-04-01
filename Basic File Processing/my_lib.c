/*
PRACTICA 1  -   SISTEMAS OPERATIVOS
GRUPO: LA PEPA
Mario Ventura, Luis Miguel Vargas, Felip Toni Font
11-2021
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "my_lib.h"

//__________________________________________________________________________________________________________________
//                                                      RETO 1
//__________________________________________________________________________________________________________________

//------------------------------------------DECLARACIÓN DE FUNCIONES------------------------------------------------
size_t my_strlen(const char *str); 
int my_strcmp(const char *str1, const char *str2);
char *my_strcpy(char *dest, const char *src);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strcat(char *dest, const char *src);
char *my_strchr(const char *str, int c);


//------------------------------------------IMPLEMENTACIÓN DE FUNCIONES------------------------------------------

//calcula el nº de bytes de la cadena apuntada por str sin incluir caracter nulo de terminación
size_t my_strlen(const char *str) {
    //Devuelve la longitud de la cadena apuntada por str. No devuelve error.
    //str: apunta a la cadena de la cual se ha de encontrar la longitud.

    //Variable que contiene el tamaño de la cadena
    size_t longitud = 0;

    //Iteración en la que contamos el tamaño
    for (int i = 0; str[i] != '\0'; i++) {
        longitud++;
    }

    //Devolver longitud de la cadena
    return longitud;
}

//compara las cadenas apuntadas por str1 y str2
int my_strcmp(const char *str1, const char *str2) {
    //Devuelve un entero: (<0 si str1 < str2) , (>0 si str2 < str1) , (=0 si str1 = str2)
    //str1: apunta a la 1ª cadena a comparar.
    //str2: apunta a la 2ª cadena a comparar.
    /*
    No se basa en la longitud de las cadenas sino en los códigos ASCII de cada carácter. 
    Compara carácter a carácter y cuando encuentra uno diferente reporta el resultado calculado
    como la resta de los códigos ASCII de los caracteres diferentes.
    */

    //Iteración en la que se comparará las cadenas carácter a carácter
    while ((*str1!='\0')&&(*str1==*str2)) {     
        //Aumentar direcciones para acceder al siguiente caracter 
        str1++;
        str2++;
    }

    //Variable entera que devolverá el método
    int resultado;
    resultado = (*str1)-(*str2);
    return resultado;
}

//copia la cadena apuntada por src en la memoria apuntada por dest. Devuelve el puntero dest. No devuelve error.
char *my_strcpy(char *dest, const char *src) {
    /*
    dest: es el puntero a la cadena destino donde se copiará el contenido.
    src: puntero a la cadena a copiar.
    */

   //Variable auxiliar para no alterar las direcciones
   int aux = 0;

   //Usar estructura iterativa para copiar todos los caracteres de src en dest
   while (*(src+aux)!='\0') {
        //Copiar contenido de src en dest
        *(dest+aux) = *(src+aux);
        //Aumentar aux para acceder al siguiente caracter de la cadena apuntada por src
        aux++;

   }
   //Copiar el caracter '\0' en el destino
   *(dest+aux) = *(src+aux);
   //devolver la dirección de destino
   return dest;
   
}

//Copiar n caracteres desde src a dest
char *my_strncpy(char *dest, const char *src, size_t n) {
    //Devuelve el puntero dest. No devuelve error.

    //Variable auxiliar para no alterar las direcciones
   int aux = 0;

   //Usar estructura iterativa para copiar todos n caracteres de src en dest
   while (aux<n) {
        
        //Si hay un caracter, copiarlo
        if (*(src+aux) != '\0') {
            //Copiar contenido de src en dest
            *(dest+aux) = *(src+aux);
        } else {
            *(dest+aux) = '\0';
        }
        //Aumentar aux para acceder al siguiente caracter de la cadena apuntada por src
        aux++;

   }
   return dest;

}

//añade la cadena apuntada por src a la cadena apuntada por dest
char *my_strcat(char *dest, const char *src) {
    //Devuelve el puntero dest. No devuelve error.

    //Usar variables auxiliares para no alterar src y dest
    int aux = 0;
    int aux2 = 0;

    //Iteracion para llegar al ultimo caracter de src
    while (*(dest+aux) != '\0') {
        aux++;
    }

    //En este punto, estamos sobre el ultimo caracter de dest
    while (*(src+aux2) != '\0') {
        //Copiar caracter de src en dest
        *(dest+aux) = *(src+aux2);
        //AUmentar indices auxiliares
        aux++; aux2++;
    }

    //Devolver el puntero dest
    return dest;
}

//escanea str en busca de la primera ocurrencia del carcater c
char *my_strchr(const char *str, int c) {
    //Devuelve el puntero a la primera ocurrencia del carácter c
    //en la cadena str o NULL si el carácter no se encuentra. No devuelve error.

    bool encontrado = false;

    //Iterar en busca de la letra pasada como parámetro
    while ((*str != '\0')&&(encontrado == false)) {

        //Buscar la letra
        if (*str == (char)c) {
            //Se ha encontrado la letra que se buscaba
            encontrado = true;  
            //Devolver la letra
            return str;       
        }
        //Aumentar direccion almacenada en str para acceder a la siguiente letra
        str++;
    }

    //Si no se ha encontrado, mostrar null por consola
    if (encontrado = false) {
        return NULL;
    }


    //FIN DE LA LIBRERÍA
}



//__________________________________________________________________________________________________________________
//                                                      RETO 2
//__________________________________________________________________________________________________________________

//------------------------------------------DECLARACIÓN DE FUNCIONES------------------------------------------------
struct my_stack *my_stack_init (int size);
int my_stack_push (struct my_stack *stack, void *data);
void *my_stack_pop (struct my_stack *stack);
int my_stack_len (struct my_stack *stack);
int my_stack_purge (struct my_stack *stack);
int my_stack_write (struct my_stack *stack, char *filename);
struct my_stack *my_stack_read (char *filename);


//------------------------------------------IMPLEMENTACIÓN DE FUNCIONES------------------------------------------

//Reserva espacio para una variable que contendrá el puntero top al nodo superior de la pila y el tamaño de datos size
struct my_stack *my_stack_init (int size) {
    /*
    Reserva espacio para la pila (variable de tipo struct_my_stack).
    Inicializa top con NULL, como valor del puntero al nodo superior de la pila, y size con el tamaño de datos
    que nos pasan como parámetro. 
    Devuelve un puntero a la pila inicializada (struct my_stack).
    */

    //Declarar puntero a un struct de tipo stack y asignarle espacio en la memoria dinámica
    struct my_stack *pila;  
    pila = malloc(size);

    //Dar valor a los atributos de la pila
    pila->top = NULL;
    pila->size = size;

    //Devolver el puntero a la pila
    return pila;
}

//Inserta un nuevo nodo en los elementos de la pila
int my_stack_push (struct my_stack *stack, void *data) {
    /*
    Inserta un nuevo nodo en los elementos de la pila
    (hay que reservar espacio de memoria para él). El puntero a los datos de ese
    nodo (data) nos lo pasan como parámetro.
    Devuelve 0 si ha ido bien, -1 si hubo error
    */

   //Comprobar si existe el puntero y si el tamaño > 0
   if ((stack!=NULL)&&(stack->size > 0)) {

       //Insertar un nuevo nodo en la pila y reservar memoria para él mediante la función malloc
       struct my_stack_node *nodo;
       nodo = malloc(sizeof(nodo));
       //Hacer que los datos del nuevo nodo sean los pasados por parámetro
       nodo->data = data;

       //Comprobar si la pila ya tiene un nodo en top
       if (stack->top == NULL) {

            //Ahora este nuevo nodo es el que está arriba del todo de la pila (top)
            stack->top = nodo;
       } else {

           //Tener en cuenta que no se debe aplastar el nodo que ya está en top de la pila
           nodo->next = stack->top; //Hacer que el nuevo nodo apunte al que antes era el top
           //Hacer que el nuevo top sea el nodo que se acaba de crear
           stack->top = nodo; 
       }

       //Devolver 0 para informar de que todo ha ido bien
       return (0);

   } else {
       //Devolver -1 para informar de que ha ido mal o ha habido error en la ejecución de my_stack_init()
       return (-1);
   }

}

//Elimina el nodo superior de los elementos de la pila
void *my_stack_pop (struct my_stack *stack) {
    /*
    Elimina el nodo superior de los elementos de la pila (y libera la memoria que ocupaba ese nodo!).
    Devuelve el puntero a los datos del elemento eliminado.
    */

   //Comprobar si la pila tiene algun nodo, ya que de lo contrario no se puede hacer pop
   bool vacia;
   if (stack->top != NULL) {
       vacia = false;
   } else {
       vacia = true;
   }

   //LLegados a este punto, si 'vacia' es falso, es porque se ha detectado que hay algún nodo
   //En caso de que la pila no esté vacía, hacer las instrucciones correspondientes al pop
   if (vacia == false) {

       //Variable auxiliar para poder acceder al nodo por debajo de top
       struct my_stack_node *nodoAux;
       nodoAux = stack->top; 
       //Puntero a los datos del elemento el eliminado que habrá que devolver
       void *puntero;

       //Comprobar si la lista tiene un solo nodo
       if (nodoAux->next == NULL) {        
           //La lista tiene 1 nodo. Eliminarlo y dejar la lista vacía
           stack->top = NULL; //Ya no hay top, ya que la lista contenía solo 1 nodo y lo vamos a eliminar (quedarán 0 nodos)
           //Dar valor al puntero que apunta a los datos del nodo eliminado
           puntero = nodoAux->data; //Porque nodoAux = stack->top
           //Eliminamos la variable auxiliar nodoAux, ya que no la necesitamos más
           free(nodoAux);
           //Devolvemos puntero con los datos del nodo eliminado para que la funcion free del main en test2a
           //pueda eliminar el nodo como tal, que ya no forma parte de la lista pero sigue almacenado en memoria
           return puntero;

       } else {
           //Como hay más de un nodo, hay que hacer que el nodo por debajo del top sea el nuevo top
           stack->top = nodoAux->next;
           //Dar valor al puntero que apunta a los datos del nodo eliminado
           puntero = nodoAux->data; //Porque nodoAux = stack->top
           //Eliminamos la variable auxiliar nodoAux, ya que no la necesitamos más
           free(nodoAux);
           //Devolvemos puntero con los datos del nodo eliminado para que la funcion free del main en test2a
           //pueda eliminar el nodo como tal, que ya no forma parte de la lista pero sigue almacenado en memoria
           return puntero;
       }  

   } else {
       //Si no existe ningún nodo en la pila, devolver NULL
       return NULL;
   }
}

//Recorre la pila y retorna el número de nodos totales que hay en los elementos de la pila.
int my_stack_len (struct my_stack *stack) {

    //Crear un contador
    int contador = 0;      

    //Comprobar si la pila está vacía
    if (stack->top != NULL) {
        
        //Crear nodo auxiliar para iterar
        struct my_stack_node *nodoAuxiliar = NULL;
        //Iterar para saber el tamaño de la lista de nodos en la pila ya que esta no está vacía
        for (nodoAuxiliar=stack->top; nodoAuxiliar != NULL; nodoAuxiliar=nodoAuxiliar->next) {
            //Aumentar contador de nodos
            contador++;
        }
        return contador;

    //La lista está vacía, devolver tamaño 0
    } else {
        return 0;
    }
}

//Recorre la pila liberando memoria que habiamos reservado y también la memoria que ocupa struck my_stack
int my_stack_purge (struct my_stack *stack) {
    /*
    Recorre la pila liberando la memoria que habíamos reservado para cada uno de los datos (data)
    y la de cada nodo. Finalmente libera también la memoria que ocupa el struck my_stack. Es decir,
    toda la memoria que se reservó con malloc() en algún momento, se libera con la función free(). 
    */

   //Variable contador que devolverá el programa
   int bytes;

   //Crear nodos auxiliares para iterar la lista de nodos enlazados entre sí
   struct my_stack_node *nodoAuxiliar, *nodoP;
   //Inicializar la varuiable contadora
   bytes = (sizeof(stack) * 2);
   
   //Si la pila no está vacía, se puede hacer purgue
   if (stack->top != NULL) {
       //Nodo con el que empezaremos a iterar desde el top de la pila recorriendola hacia "abajo"
       nodoP = stack->top;

       //Iteración sobre la lista de nodos
       while (nodoP != NULL) {
           //Nodo que siempre va una posición por delante
           nodoAuxiliar = nodoP->next;

           //Sumar a la cantidad liberada el tamaño del nodo
           bytes = bytes + (sizeof(nodoP) * 2);
           //Sumar a la cantidad liberada el tamaño del campo size de la pila stack
           bytes = bytes + (stack->size);

           //Liberar memoria usada para el campo data del nodo
           free(nodoP->data);
           //Liberar memoria usada para el nodo
           free(nodoP);

           //Avanzar al próximo nodo para eliminarlo en la próxima iteración
           nodoP = nodoAuxiliar;
       }
   } 
   //Liberar espacio de la pila
   free(stack);
   //En caso de que la pila esté vacía, bytes solo tendrá el valor de esta, y no el de ningún nodo
   //Devolver número de bytes eliminados
   return bytes;
}

//Almacena los datos de la pila en el fichero indicado por filename
int my_stack_write (struct my_stack *stack, char *filename) {
    //Almacena los datos de la pila en el fichero indicado por filename

    //Crear e inicializar una nueva pila auxiliar llamada newStack
    struct my_stack *newStack;
    int sizeStack = stack->size;
    newStack = my_stack_init(sizeStack);
    //Nodo auxiliar que se usará para iterar la pila empezando por el top de la original
    struct my_stack_node *nodoAuxiliar;
    nodoAuxiliar = stack->top;
    //Puntero void para almacenar el campo data de cada nodo
    void *data;
    
    //Recorrer la pila stack y copiar su información en newStack (en caso de que esta no esté vacía)
    if (stack->top != NULL) {
        //Copiar en pila auxiliar la pila original pero de forma invertida
        while(nodoAuxiliar != NULL) {
            //Dar valor al puntero data
            data = nodoAuxiliar->data;
            //Push del ultimo nodo de la pila original como primero de la nueva pila
            my_stack_push(newStack, data);
            //Avanzar al próximo nodo
            nodoAuxiliar = nodoAuxiliar->next;
        }
    }

    //Abrir enlace con un fichero para poder escribir en él
    int fichero = open(filename,(O_WRONLY | O_CREAT | O_TRUNC),(S_IRUSR | S_IWUSR));
    //Almacenar tamaño datos
    write(fichero,&sizeStack,sizeof(int));
    //Variable contador que devolverá la función
    int almacenado = 0;

    //Comprobar si ha habido algún error y en caso de que sea así devolver -1
    if (fichero == -1) {     
        almacenado = -1;

    } else {
        //En teoría, llegados a este punto, la pila 'newStack' es igual a la pila original 'stack', pero invertida
        //Procedemos a escribir la información de la pila invertida newStack
        while ((data = my_stack_pop(newStack))) {
            //Escribir la informacion del nodo en un fichero
            write(fichero,data,sizeStack);
            //Aumentar almacenado
            almacenado++;
        }
        //Cerrar el enlace creado con el fichero
        close(fichero);
    }
    //Devolver lo almacenado
    return almacenado;
}

//Lee los datos de la pila almacenados en el fichero
struct my_stack *my_stack_read (char *filename) {
    //Lee los datos de la pila almacenados en el fichero.

    //Abrir enlace con el fichero
    int fichero;
    fichero = open(filename,O_RDONLY);   //No hace falta conceder permisos
    //Puntero al campo data del nodo
    void *data;
    //Leer los primeros 4 bytes
    int sizeDato;
    read(fichero,&sizeDato,sizeof(int));  
    //Hacer nueva pila para reconstruir la original en ella
    struct my_stack *newStack;
    newStack = my_stack_init(sizeDato);
    //Reservar espacio para el campo data de los nodos
    data = malloc(sizeDato);
    //Variable que contendrá lo leído
    int lectura;
    lectura = read(fichero, data, sizeDato);

    //Iteración de lectura del fichero
    while (lectura != 0) { //Porque EOF cuando read() devuelve 0
        //Hacer push de lo leído en la pila
        my_stack_push(newStack, data);
        //Reservar espacio para el dato del siguiente nodo
        data = malloc(sizeDato);
        //Próxima lectura
        lectura = read(fichero, data, sizeDato);
    }
    //Cierre del enlace con el fichero
    close(fichero);
    //Retorno de la pila
    return newStack;
    
}


//__________________________________________________________________________________________________________________
//                                               FIN DE LA LIBRERÍA my_lib.c
//__________________________________________________________________________________________________________________