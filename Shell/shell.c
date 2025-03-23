#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char currentpath[200];

bool compare(char* str1, char* str2){
    // Compare la chaîne str1 au début de la chaîne str2.
    int i=0;
    if(str1[0] == '\0' && str2[0] == '\0'){ // Cas deux string vide.
        return true;
    }
    if(str1[0] == '\0' || str2[0] == '\0'){ // Cas une des deux string vide
        return false;
    }
    while(str1[i] != '\0'){ // Tant qu'on a pas fini de lire str1
        // Si les caractères sont différents ou str2 est plus petit que str1
        if (str1[i] != str2[i] || str2[i] == '\0'){
            return false;
        }
        i += 1; 
    }
    return true;
}

void askInput(char* entry){
    /* Affiche le path actuel, demande une entrée,
    et la place dans la variable passée en paramètre. */
    printf("%s $ ", currentpath);
    fgets(entry, sizeof(entry), stdin);
}

void changeDirectory(char* path){
    printf("Path : %s\n", path);
    if (path[0] == '\0'){      // Juste cd
        strcpy(currentpath,"/~");
    } else if(path[0] == ' '){ // cd avec chemin
        path = &path[1];    // On coupe l'espace
        if(path[0] == '.'){ 
            if(path[1] == '\0'){    // Répertoire actuel
                printf("Path : BONJOUR %s\n", path);
                return;
            } else if(path[1] == '.' && path[2] == '\0') { // Répertoire père
                // TODO Si à la racine, ne rien faire, sinon remonter
                return;
            } else{           // Fichier ou dossier commençant par .
                printf("Chemin saisi invalide.\n");
            }
        } else if(path[0] == '/'){ // Chemin absolu
            struct stat stats;
            if (stat(path, &stats)){
                strcpy(currentpath,path);
            }
        } else { // Chemin relatif
            char* finalpath = malloc(strlen(path) + strlen(currentpath) + 2);
            strcpy(finalpath,currentpath);
            strcat(finalpath,"/");
            strcat(finalpath,path);
            struct stat stats;
            if (stat(finalpath, &stats) == 0){
                strcpy(currentpath,finalpath);
            }
            free(finalpath);
        }
    }
    else{   // Si c'est n'importe quoi qui commence par cd.
        printf("Commande invalide.\n");
    }
}

void cutstr(char* str){
    // Formate la string en pour retirer tous les espaces et retour chariot en fin de string
    int i=0;
    // Parcours jusqu'au retour chariot
    while(str[i] != '\n' && str[i] != '\0'){
        i++;
    }
    // Parcours à l'envers tant qu'il y a des espaces
    while(str[i-1] == ' '){
        i--;
    }
    // Placement de la balise finale
    str[i] = '\0';
}

int main(int argc, char *argv[]){

    char* entry;
    do {
        getcwd(currentpath, 200);
        askInput(&entry);
        cutstr(entry);

        if(compare("cd ", entry)) {
            // Recherche de l'indice du début du path
            int start = 3;
            while(entry[start] == ' '){
                start++;
            }
            // Recherche de l'indice de fin du path pour le message d'erreur
            // si plusieurs paramètres ont été donnés à cd
            int end = start + 1;
            while (entry[end] != ' ' && entry[end] != '\0') {
                end++;
            }
            entry[end] = '\0';
            if(chdir(&entry[start]) != 0){
                printf("cd: Ne peut pas aller au dossier %s", &entry[start]);
            }
        } else if(compare("./", entry)) {
            /*Spawn a child to run the program.*/
            pid_t pid = fork();
            if (pid == 0) { /* child process */
                char* name = &entry[2];
                printf("name %s\n", name);
                char *argv2[] = {name, NULL};
                execv(argv[1], argv2);
                exit(127); /* only if execv fails */
            } else { /* pid!=0; parent process */
                waitpid(pid, 0, 0); /* wait for child to exit */
            }
        }

    } while(strcmp(entry, "exit"));
    
    return 0;
}
