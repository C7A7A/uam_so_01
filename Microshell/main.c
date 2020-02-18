#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define historyBuffer 1024

char *readCommand();
char **parseCommand(char *line);
int executeCommand(char **arguments);
int builtInCommand(char *command);
void builtInCommandExecute(char **arguments);
void cdExecute(char *command);
void helpExecute();
void exitExecute();
void historyExecute();

char history[historyBuffer][128];
int historyCounter = 0;

int main(int argc, char **argv)
{
    char *line;
    char **parsedLine;

    char workingDirectory[256];
    char hostname[256];
    char *username;

    while(1){
        if(gethostname(hostname, sizeof(hostname)) == -1){
            perror("gethostname() eroor");
        }else{
            printf("[");
            printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, hostname);
            printf("@");
        }

        username = getenv("USER");
        if(username == NULL){
            perror("Error");
        }else{
            printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET, username);
            printf("]");
        }

        if(getcwd(workingDirectory, sizeof(workingDirectory)) == NULL){
            perror("Error");
        }else{
            printf("[");
            printf(ANSI_COLOR_MAGENTA "%s" ANSI_COLOR_RESET, workingDirectory);
            printf("] ");
        }
        printf("$ ");

        line = readCommand();
        if(strlen(line) > 0){
            parsedLine = parseCommand(line);


            if(historyCounter >= historyBuffer){
                memcpy(history[historyCounter], parsedLine[0], sizeof(parsedLine[0]) / sizeof(char));
            }else{
                memcpy(history[historyCounter++], parsedLine[0], sizeof(parsedLine[0]) / sizeof(char));
            }


            if(builtInCommand(parsedLine[0])){
               builtInCommandExecute(parsedLine);
            }else{
                executeCommand(parsedLine);
            }
        }else{
            printf("Error: no arguments\n");
        }
    }

    return 0;
}

char *readCommand(){
    char *line;
    size_t bufferSize = 0;

    getline(&line, &bufferSize, stdin);

    if(line[strlen(line)-1] == '\n'){
        line[strlen(line)-1] = '\0';
    }

    /*printf("|%s| %d\n", line, strlen(line));*/

    return line;
}

char **parseCommand(char *line){
    int i;
    int numberOfWords = 0,
        j = 0,
        wordCounter = 0, charCounter = 0;

    /* ILOŚĆ PARAMETRÓW */
    for(i = 0; i <= strlen(line); i++){
        if((line[i] == ' ' || line[i] == '\0')){
            while((line[i+1]) == ' '){
                i++;
            }
            numberOfWords++;
        }
    }
    /*printf("%i\n", numberOfWords)*/

    /* DŁUGOŚĆ POSZCZEGÓLNYCH PARAMETRÓW */
    int wordsLenght[numberOfWords];
    for(i = 0; i < numberOfWords; i++){
        wordsLenght[i] = 0;
    }

    for(i = 0; i <= strlen(line); i++){
        if((line[i] == ' ' || line[i] == '\0')){
            while(line[i+1] == ' '){
                i++;
            }
            j++;
        }else{
            wordsLenght[j]++;
        }
    }

    /* ALOKACJA PAMIĘCI DLA POSZCZEGÓLNYCH PARAMETRÓW */
    char **parsedLine = (char **)malloc(numberOfWords * sizeof(char *));
    /*printf("%d ", numberOfWords);*/
    for(i = 0; i < numberOfWords; i++){
        /*printf("%d\n", wordsLenght[i]);*/
        parsedLine[i] = (char *)malloc(wordsLenght[i] * sizeof(char));
    }

    /* PARSOWANIE */
    for(i = 0; i <= strlen(line); i++){
        if((line[i] == ' ' || line[i] == '\0')){
            /*printf("SPACJA");*/
            while(line[i+1] == ' '){
                i++;
            }
            parsedLine[wordCounter][charCounter] = '\0';
            wordCounter++;
            charCounter = 0;
        }else{
            /*printf("%c", line[i]);*/
            parsedLine[wordCounter][charCounter] = line[i];
            charCounter++;
        }
    }
    /*printf("%d %d", strlen(line), numberOfWords);*/
    /*
    for(i = 0; i < numberOfWords; i++){
        printf("%d %s %d\n", i, parsedLine[i], strlen(parsedLine[i]));
    }
    */

    return parsedLine;
}

int executeCommand(char **arguments){
    int pid = fork();

    if(pid != 0){
        /* KOD RODZICA */
        waitpid(-1, &pid, 0);
    }else{
        /* KOD DZIECKA */
        execvp(arguments[0], arguments);
        perror("Error");
        exit(0);
    }
}

int builtInCommand(char *command){
    int i;
    bool isInBuiltCommand = false;
    char *commands[] = {
        "cd",
        "help",
        "exit",
        "history"
    };

    for(i = 0; i < ((sizeof(commands))/(sizeof(char*))); i++){
        if(strcmp(command, commands[i]) == 0){
            isInBuiltCommand = true;
            break;
        }
    }

    return isInBuiltCommand;
}

void builtInCommandExecute(char **arguments){
    if(strcmp(arguments[0], "cd") == 0){
        cdExecute(arguments[1]);
    }else if(strcmp(arguments[0], "help") == 0){
        helpExecute();
    }else if(strcmp(arguments[0], "exit") == 0){
        exitExecute();
    }else if(strcmp(arguments[0], "history") == 0){
        historyExecute();
    }else{
        printf("Error: invalid command\n");
    }
}

void cdExecute(char *command){
    char previousDirectory[512];
    char tmpPreviousDirectory[512];
    char *usernameHome;
    usernameHome = getenv("HOME");

    if(command != NULL){
        if(strcmp(command, "~") == 0){
            getcwd(previousDirectory, sizeof(previousDirectory));
            chdir(usernameHome);
        }else if(strcmp(command, "-") == 0){
            getcwd(tmpPreviousDirectory, sizeof(tmpPreviousDirectory));
            chdir(previousDirectory);
            memcpy(previousDirectory, tmpPreviousDirectory, 512);
        }else{
            memcpy(tmpPreviousDirectory, previousDirectory, 512);
            getcwd(previousDirectory, sizeof(previousDirectory));
            if(chdir(command) == -1){
                memcpy(previousDirectory, tmpPreviousDirectory, 512);
                perror("Error");
            }
        }
    }else{
        getcwd(previousDirectory, sizeof(previousDirectory));
        chdir(usernameHome);
    }
}

void helpExecute(){
    printf("------- MICROSHELL -------\n");
    printf("Autor: Mateusz Czajka\n\n");
    printf("Funkcjonalności:\n");
    printf("- obsługa programów znajdujących się w katalogach opisanych wartością zmiennej PATH poprzez podanie ich nazwy wraz z opcjonalnymi argumentami\n");
    printf("- obsługa poleceń: cd, exit, help\n");
    printf("- wypisywanie komunikatów błedów\n\n");
    printf("\"Bajery:\"\n");
    printf("- bajeranckie kolorki\n");
    printf("- wyświetlanie loginu oraz nazwy komputera\n");
    printf("- ignorowanie spacji ('ls     -l' = 'ls -l')\n");
    printf("- historia\n");
}

void exitExecute(){
    exit(0);
}

void historyExecute(){
    int i;
    for(i = 0; i < historyCounter; i++){
        printf("%s\n", history[i]);
    }
}
