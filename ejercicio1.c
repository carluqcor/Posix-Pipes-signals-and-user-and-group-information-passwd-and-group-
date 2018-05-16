#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main (int argc, char **argv){
    int uflag = 0;
    int iflag = 0;
    int gflag = 0;
    int dflag = 0;
    int sflag = 0;
    int aflag = 0;
    int bflag = 0;
    int hflag = 0;
    char *avalue = NULL, *uvalue = NULL, *ivalue = NULL, *gvalue = NULL, *dvalue = NULL, name[50], name2[50], name3[50];
    int index;
    int c;

    // Declaracion de las estructuras que almacenaran la informarcion de un usuario y de un grupo
    char *lgn;
    struct passwd *pw;
    struct group *gr;
    FILE *f;

    char *lang = "LANG";
    char *value;

    // En el caso de que no se ha introducido ningún nombre de usuario,
    // por defecto en lgn almacenara el usuario que ha ejecutado este programa
    
    // OJO, NO USAR getlogin() porque no funciona en determinados sistemas, depende del 
    // terminal y unos ficheros asociados a el, es mejor coger el usuario actual a partir
    // de la variable de entorno USER (getenv("USER")) y pasarselo a getpwnam(). Modifique el programa para ello.
    /* IMPORTANTE:
     Las variables optind, optarg, opterr y optopt son variables externas incluidas en <unistd.h> */
    /*
     If getopt() does not recognize an option character, it prints an error message to stderr,
     stores the character in optopt, and returns '?'. The calling program may prevent the error
     message by setting opterr to 0 */
    //Prueba a ejecutar el programa comentando esta linea, podrás observar como se obtiene un
    // error por defecto por la salida estandar, en este caso el terminal.
    opterr = 0; 

    // "abc:" -> busca como opciones a y b sin argumentos y c con un argumento OBLIGATORIO.
    // ':' indica que la opcion debe llevar un argumento obligario
    
    // getopt va iterando, y devuelve -1 si ya hemos consultado toda la linea de argumentos.
    // Sino, devuelve el caracter de opción encontrado para caracteres validos 
    // o devuelve ? si el caracter no es valido segun la cadena especificada.
    while ((c = getopt (argc, argv, "u:i:g:d:sa:bh")) != -1){
            // Podemos observar qué pasa con las variables externas que va gestionando
            //   getopt() durante las sucesivas llamadas. 
            //   - optind Indice del siguiente elemento a procesar del vector argv[]
            //   - optarg recoge el valor del argumento obligario de una opcion.
            //   - optopt recoge el valor de la opcion cuando es desconocida (?) o INCOMPLETA respecto a las opciones indicadas.        
            switch (c){
                case 'u':
                    uflag = 1; //Bandera que indica que la opción -u se ha pasado por linea de argumentos
                    uvalue = optarg;
                    break;


                case 'i':
                    iflag = 1;    
                    ivalue = optarg;                 
                    break;
               
                case 'g':
                    gflag = 1;
                    gvalue = optarg;
                    break;

                case 'd':
                    dflag = 1;
                    dvalue = optarg;                   
                    break;
                
                case 's':
                    sflag = 1;
                    f=fopen("/etc/group", "r");
                    if(f==NULL){
                      fprintf(stderr,"Error en la lectura del fichero %s\n", argv[2]);
                      exit(-1);
                    }
                    while(fscanf(f, "%[^:]:x:%s%*[^\n]",name2, name)==2){
                        gr=getgrgid(atoi(name));
                        printf("Nombre del grupo principal: %s\n", gr->gr_name);
                        printf("Contraseña del grupo: %s\n", gr->gr_passwd);
                        printf("Id del grupo: %d\n", gr->gr_gid);
                        printf("Miembro grupo: %p\n\n", gr->gr_mem);
                    }
                    fclose(f);
                    break;

                case 'a':
                    aflag=1;  
                    avalue = optarg;
                    break;

                case 'b':
                    if(argv[2]){
                        fprintf(stderr,"Error, esta opción no necesita argumentos\n");
                        printf("La opción -b muestra la información correspondiente al grupo del usuario actual.\n");
                        exit (-1);
                    }else{
                        lgn = (getenv("USER"));
                        pw = getpwnam(lgn);
                        gr=getgrgid(pw->pw_gid);
                        printf("Nombre del grupo principal: %s\n", gr->gr_name);
                        printf("Id del grupo: %d\n", gr->gr_gid);
                    }
                    break;

                case 'h':
                    hflag=1;
                    if(argv[2]){
                        fprintf(stderr, "Error, esta opción no necesita argumentos\n");
                        printf("La opción -h muestra la información sobre los comandos posibles.\n");
                        exit (-1);
                    }else{
                        printf("La opción -u <Nombre de Usuario> muestra el nombre de usuario de passwd.\n");
                        printf("La opción -i <ID de Usuario> muestra el ID de usuario de passwd.\n");
                        printf("La opción -g <Nombre de Grupo> muestra el nombre del grupo del usuario de group.\n");
                        printf("La opción -d <ID de Grupo> muestra el ID del grupo del sistema de group.\n");
                        printf("La opción -s se muestra todos los grupos del sistema junto a su ID.\n");
                        printf("La opción -a <Nombre de Usuario> muestra la información correspondiente de passwd y group.\n");
                        printf("La opción -b muestra la información correspondiente al grupo del usuario actual.\n");
                        printf("La opción -h muestra la información sobre los comandos posibles.\n");
                        printf("Si se ejecuta sin opciones muestra la información sobre passwd y group del usuario actual en Español o Inglés.\n");
                    }
                break;

                case '?': //Opcion no reconocida o INCOMPLETA. Probar tambien la diferencia entre ejecutar %$>./a.out m   ó   %$>./a.out -m
                    if (optopt == 'u'){ //Para el caso de que 'c' no tenga el argumento obligatorio.
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -u <Nombre de Usuario> muestra el nombre de usuario de passwd.\n");
                  
                    }else if (optopt == 'i'){
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -i <ID de Usuario> muestra el ID de usuario de passwd.\n");

                    }else if (optopt == 'g'){
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -g <Nombre de Grupo> muestra el nombre del grupo del usuario de group.\n");

                    }else if (optopt == 'd'){
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -d <ID de Grupo> muestra el ID del grupo del sistema de group.\n");

                    }else if (optopt == 'a'){
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -a <Nombre de Usuario> muestra la información correspondiente de passwd y group.\n");

                    }else if (optopt == 's'){
                        fprintf (stderr, "La opción %c requiere un argumento. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -s se muestra todos los grupos del sistema junto a su ID.\n");

                    }else if (isprint (optopt)){ //Se mira si el caracter es imprimible
                        fprintf (stderr, "Opción desconocida \"-%c\". Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -u <Nombre de Usuario> muestra el nombre de usuario de passwd.\n");
                        printf("La opción -i <ID de Usuario> muestra el ID de usuario de passwd.\n");
                        printf("La opción -g <Nombre de Grupo> muestra el nombre del grupo del usuario de group.\n");
                        printf("La opción -d <ID de Grupo> muestra el ID del grupo del sistema de group.\n");
                        printf("La opción -s se muestra todos los grupos del sistema junto a su ID.\n");
                        printf("La opción -a <Nombre de Usuario> muestra la información correspondiente de passwd y group.\n");
                        printf("La opción -b muestra la información correspondiente al grupo del usuario actual.\n");
                        printf("La opción -h muestra la información sobre los comandos posibles.\n");
                        printf("Si se ejecuta sin opciones muestra la información sobre passwd y group del usuario actual en Español o Inglés.\n");

                    }else{ //Caracter no imprimible o especial
                        fprintf (stderr, "Caracter `\\x%x'. Valor de opterr = %d\n", optopt, opterr);
                        printf("La opción -u <Nombre de Usuario> muestra el nombre de usuario de passwd.\n");
                        printf("La opción -i <ID de Usuario> muestra el ID de usuario de passwd.\n");
                        printf("La opción -g <Nombre de Grupo> muestra el nombre del grupo del usuario de group.\n");
                        printf("La opción -d <ID de Grupo> muestra el ID del grupo del sistema de group.\n");
                        printf("La opción -s se muestra todos los grupos del sistema junto a su ID.\n");
                        printf("La opción -a <Nombre de Usuario> muestra la información correspondiente de passwd y group.\n");
                        printf("La opción -b muestra la información correspondiente al grupo del usuario actual.\n");
                        printf("La opción -h muestra la información sobre los comandos posibles.\n");
                        printf("Si se ejecuta sin opciones muestra la información sobre passwd y group del usuario actual en Español o Inglés.\n");
                    }
                    return 1;  //Finaliza el programa
             
                default:
                    abort();
            }
            printf("optind: %d, optarg: %s, optopt: %c, opterr: %d\n", optind, optarg, optopt, opterr);
    }
    //Caso -u
    if(uflag==1){
        if((iflag == 1) || (sflag == 1) || (aflag == 1)){
            printf("No se puede activar la opción -u porque ya ha sido activada -i, -g, -d o -a\n");
            exit (-1);   
        }else{
            if(getpwnam(uvalue)==NULL){
                        printf("Error u\n");
                fprintf(stderr,"Error, el nombre del usuario introducido no se encuentra en el fichero passwd\n");
                exit (-1);
            }else{
                pw = getpwnam(uvalue);
                printf("Nombre: %s\n", pw->pw_gecos);
                printf("Login: %s\n", pw->pw_name);
                printf("Password: %s\n", pw->pw_passwd);
                printf("UID: %d\n", pw->pw_uid);
                printf("Home: %s\n", pw->pw_dir);
                printf("Número de grupo principal: %d\n\n", pw->pw_gid);
            }
        }
    }

    //Caso -i
    if(iflag == 1){
        if((uflag == 1) || (sflag == 1) || (aflag == 1)){
            printf("No se puede activar la opción -i porque ya ha sido activada -u, -g, -d o -a\n");
            exit (-1); 
        }else{
            if(getpwuid(atoi(ivalue))==NULL){
                fprintf(stderr,"Error, el id del usuario introducido no se encuentra en el fichero passwd\n");
                exit (-1);
            }else{
                pw = getpwuid(atoi(ivalue));
                printf("Nombre: %s\n", pw->pw_gecos);
                printf("Login: %s\n", pw->pw_name);
                printf("Password: %s\n", pw->pw_passwd);
                printf("UID: %d\n", pw->pw_uid);
                printf("Home: %s\n", pw->pw_dir);
                printf("Número de grupo principal: %d\n\n", pw->pw_gid);
            }  
        }
    }

    //Caso -g
    if(gflag == 1){
        if(sflag == 1 || dflag == 1 || aflag == 1){
            printf("No se puede activar la opción -g porque ya ha sido activada -u, -i, -d o -a\n");
            exit (-1);    
        }else{
            if(getgrnam(gvalue)==NULL){
                fprintf(stderr,"Error, el nombre del grupo introducido no se encuentra en el fichero group\n");
                exit (-1);
            }else{
                gr=getgrnam(gvalue);
                printf("Nombre del grupo principal: %s\n", gr->gr_name);
                printf("Contraseña del grupo: %s\n", gr->gr_passwd);
                printf("Id del grupo: %d\n", gr->gr_gid);
                printf("Miembro grupo: %p\n", *gr->gr_mem);
            }
        }
    }

    //Caso -d
    if(dflag == 1){
        if(sflag == 1 || dflag == 1 || aflag == 1){
            printf("No se puede activar la opción -d porque ya ha sido activada -u, -i, -g o -a\n");
            exit (-1);                    
        }else{
            if(getgrgid(atoi(dvalue))==NULL){
                fprintf(stderr,"Error, el id del grupo introducido no se encuentra en el fichero group\n");
                exit (-1);
            }else{
                gr=getgrgid(atoi(dvalue));
                printf("Nombre del grupo principal: %s\n", gr->gr_name);
                printf("Contraseña del grupo: %s\n", gr->gr_passwd);
                printf("Id del grupo: %d\n", gr->gr_gid);
                printf("Miembro grupo: %p\n", *gr->gr_mem);
            }
        }
    }

    //Caso -a
    if(aflag == 1){
        if(iflag == 1 || uflag == 1 || dflag == 1 || gflag == 1){
            printf("No se puede activar la opción -a porque ya ha sido activada -u, -i, -g o -d\n");
            exit (-1);                    
        }else{
            if(getpwnam(avalue)==NULL){
                fprintf(stderr,"Error, el usuario del sistema es inexistente\n");
                exit (-1);
            }else{
                pw = getpwnam(avalue);
                printf("Nombre: %s\n", pw->pw_gecos);
                printf("Login: %s\n", pw->pw_name);
                printf("Password: %s\n", pw->pw_passwd);
                printf("UID: %d\n", pw->pw_uid);
                printf("Home: %s\n", pw->pw_dir);
                printf("Número de grupo principal: %d\n\n", pw->pw_gid);
                gr=getgrgid(pw->pw_gid);
                printf("Nombre del grupo principal: %s\n", gr->gr_name);
                printf("Contraseña del grupo: %s\n", gr->gr_passwd);
                printf("Id del grupo: %d\n", gr->gr_gid);
                printf("Miembro grupo: %p\n\n", gr->gr_mem);
            }
        }
    }


    if(uflag==0 && iflag==0 && gflag==0 && dflag==0 && sflag==0 && aflag==0 && bflag==0 && hflag==0){
        value = getenv(lang);
        lgn = (getenv("USER"));
        pw = getpwnam(lgn);
        gr=getgrgid(pw->pw_gid);
        if (strstr(value,"ES")){
            printf("Nombre: %s\n", pw->pw_gecos);
            printf("Acceso: %s\n", pw->pw_name);
            printf("Contraseña: %s\n", pw->pw_passwd);
            printf("ID de usuario: %d\n", pw->pw_uid);
            printf("La carpeta del usuario es: %s\n", pw->pw_dir);
            printf("Número de grupo principal: %d\n\n", pw->pw_gid);
            printf("Nombre del grupo principal: %s\n", pw->pw_name);

            printf("Nombre del grupo principal: %s\n", gr->gr_name);
            printf("Contraseña del grupo: %s\n", gr->gr_passwd);
            printf("Id del grupo: %d\n", gr->gr_gid);
            printf("Miembro grupo: %p\n\n", gr->gr_mem);
        }else{
            printf("Name: %s\n", pw->pw_gecos);
            printf("Login: %s\n", pw->pw_name);
            printf("Password: %s\n", pw->pw_passwd);
            printf("UID: %d\n", pw->pw_uid);
            printf("Home: %s\n", pw->pw_dir);
            printf("GID: %d\n", pw->pw_gid);
            printf("Group Name: %s\n\n", pw->pw_name);

            printf("Group Name: %s\n", gr->gr_name);
            printf("Group's password: %s\n", gr->gr_passwd);
            printf("Group's ID: %d\n", gr->gr_gid);
            printf("Group's member: %p\n\n", gr->gr_mem);
        }
    }
    //Este último bucle controla opciones introducidas por el usuario que no hayan sido procesadas
    //por ser no reconocidas al no llevar un guion "-" delante.
    //Compara el número de argumentos recibidos con el número de opciones reconocidas mediante "-". 
    //Como getopt() internamente reordena los valores de argv, las primeras posiciones de argv
    //corresponden a opciones conocidas y las últimas, a partir de optind, a opciones no reconocidas.

    for (index = optind; index < argc; index++)
        printf ("Argumento \"%s\" de la línea de comandos que NO es una opción.\n", argv[index]);

    // El siguiente condicional permitiría dar un valor por defecto a la opción c.
    // Comprueba y razona si se llegaría a ejecutar y en qué circunstancias
    if(avalue==NULL)
        avalue="ValorPorDefecto";
    if(uvalue==NULL)
        uvalue="ValorPorDefecto";
    if(ivalue==NULL)
        ivalue="ValorPorDefecto";
    if(gvalue==NULL)
        gvalue="ValorPorDefecto";
    if(dvalue==NULL)
        dvalue="ValorPorDefecto";

    //Para visualizar que opciones se han activado y sus argumentos
    printf ("uflag=%d, iflag=%d, gflag=%d, dflag=%d, sflag=%d, aflag=%d, bflag=%d, hflag=%d, avalue = %s, uvalue = %s, ivalue = %s, gvalue = %s, dvalue = %s\n", uflag, iflag, gflag, dflag, sflag, aflag, bflag, hflag, avalue, uvalue, ivalue, gvalue, dvalue);
    

    return 0;
}


