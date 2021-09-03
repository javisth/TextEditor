/* Text editor in C */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


#define BUFFER_SIZE 1024


// linked list node

struct Node
{
	char *data;
	struct Node *next;
};

struct List
{
    struct Node* head;
    int size;
};

struct findReturnData {
    struct Node* currLine;
    int numCurrLine;
};

struct Node* addNode(struct List *list, char *val) {
    // if empty list:
    if (list->head == NULL) {
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = NULL;
        newNode->data = (char*)malloc(strlen(val) + 1);
        strcpy(newNode->data, val);
        list->head = newNode;
        list->size = 1;
    } else {
        struct Node *current = list->head;
        while (current->next != NULL ) {
            current = current->next;
        }
        /* now we can add a new variable */
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = NULL;
        newNode->data = (char*)malloc(strlen(val) + 1);
        strcpy(newNode->data, val);
        current->next = newNode;
        list->size++;
    }
    return list->head;
}

void deleteNode(struct List *list, struct Node * currLine) {
    struct Node *current = list->head;
    // if list is empty
    if(current == NULL)
    {
        return;
    }
    // if head needs to be removed
    if(current == currLine)
    {
        list->head = current->next;
        free(currLine->data);
        free(currLine);
        return;
    }
    while (current->next != currLine)
    {
        current = current->next;
    }
    if (current->next == NULL) {
        // reached end of list, but no element to delete - do nothing
        return;
    }

    // delete current->next
    current->next = current->next->next;
    free(currLine->data);
    free(currLine);
    list->size--;
    return;

}

void insertAfter(struct List *list, char* val, int pos) {
    struct Node* curr;
    struct Node* prev;
    int i;
    curr = list->head;
    prev = NULL;

    if (list->head == NULL) {
        list->head = addNode(list, val);
        return;
    }

    for (i = 1; i < pos; i++) {
        prev = curr;
        curr = curr->next;
    }
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode->next = curr->next;
    newNode->data = (char*)malloc(strlen(val) + 1);
    strcpy(newNode->data, val);
    curr->next = newNode;
    list->size++;
}

void insertAt(struct List *list, char* val, int pos) {
    struct Node * curr;
    struct Node * prev;
    int i;
    curr = list->head;
    prev = NULL;

    if (list->head == NULL) {
        list->head = addNode(list, val);
        return;
    }
    for (i = 1; i < pos; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        // inserting at start of list
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = list->head;
        newNode->data = (char*)malloc(strlen(val) + 1);
        strcpy(newNode->data, val);
        list->head = newNode;
        list->size++;
    } else {
        struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
        newNode->next = curr;
        newNode->data = (char*)malloc(strlen(val) + 1);
        strcpy(newNode->data, val);
        prev->next = newNode;
        list->size++;
    }
}

void printList(struct List* list, int linum) {
    if (list->head == NULL) {
        return;
    }
    printf("printing text buffer\n");
    struct Node* curr = list->head;
    int i = 1;
    while (curr != NULL) {
        if(linum == 0) {
            printf("%s", curr->data);
        } else {
            printf("%d | %s", i, curr->data);
            i++;
        }
        curr = curr->next;
    }
}

struct Node* getCurrLine(struct List * textBuffer, int pos) {
    struct Node* currLine = textBuffer->head;
    int i;
    for(i = 1; i < pos; i++) {
        currLine = currLine->next;
    }
    return currLine;
}

void printCurrLine(struct Node* currLine) {
    if (currLine != NULL)
        printf("currLine: %s\n", currLine->data);
}

void moveCurrLineTo(struct List * textBuffer, struct Node* currLine, int pos) {
    struct Node* curr = textBuffer->head;
    if (textBuffer == NULL) {
        return;
    }
    if(curr == NULL) {
        return;
    }
    // remove currLine from list
    if (curr == currLine) {
        textBuffer->head = curr->next;
        textBuffer->size -= 1;
    } else {
        while(curr->next != currLine) {
            curr = curr->next;
        }
        if (curr == NULL) {
            // currLine next existed - should never happen
            printf("massive error - exiting...\n");
            exit(0);
        }
        // curr->next is currLine;
        curr->next = curr->next->next;
        textBuffer->size--;
    }
    // now place currLine wherever needed
    char* val = currLine->data;
    insertAt(textBuffer, val, pos);
    free(currLine->data);
    free(currLine);
}

struct findReturnData* findString(struct List * textBuffer, char* needle) {
    struct Node * current = textBuffer->head;
    for(int i = 1; i <= textBuffer->size; i++) {
        if (strstr(current->data, needle)) {
            struct findReturnData *ret = (struct findReturnData*)malloc(sizeof(struct findReturnData));
            ret->currLine = current;
            ret->numCurrLine = i;
            return ret;
        }
        current = current->next;
    }
    //printf("Not found\n");
    return NULL;
}

int main(int argc, char* argv[]) {
    // text buffer
    struct List* textBuffer = (struct List*)malloc(sizeof(struct List));
    textBuffer->head = NULL;
    textBuffer->size = 0;

    int ch;
    char nl;
    int line_num;
    int ln;
    int counter;
    int count;
    int i;
    int r;
    int length;

    // current line number
    int numCurrLine;

    // current line pointer
    struct Node* currLine;
    struct Node* n;

    //file pointer
    FILE *fp;
    FILE *fp2;
    FILE *p;

    //file name
    char *fileName;

    // random variables
    ssize_t read;
    char *line = NULL;
    size_t len = 0;
    char command[BUFFER_SIZE];
    char str[BUFFER_SIZE];
    char substr[BUFFER_SIZE];
    struct findReturnData* ret;
    char *tok;
    char path[BUFFER_SIZE];

    if(argc < 2) {
        printf("File name missing\n");
        return 0;
    }
    fp = fopen(argv[1], "r+");
    fileName = argv[1];
    // load file into text buffer
    if (fp != NULL) {
        while((read = getline(&line, &len, fp)) != -1) {
            textBuffer->head = addNode(textBuffer, line);
        }
        fclose(fp);
        if (line){
            free(line);
        }
        numCurrLine = 0;
        currLine = textBuffer->head;
    } else {
        textBuffer->head = NULL;
        numCurrLine = 0;
        currLine = textBuffer->head;
    }

    printf("number of lines: %d\n", textBuffer->size);

//    printList(head, 0);

    while(1) {
        // print curr line:
        printCurrLine(currLine);
        printf("Your options are:\n");
        printf("p: The p command results in the lines of the file being printed to the standard output.\n");
        printf("n: The n command is like p, but also prefixes each line with a line number.\n");
        printf("d: Delete the current line.  If you are supporting a range of line numbers, the d command is a good one to apply to the range.\n");
        printf("m: Move the current line to before the line whose number follows the m command.  To move to the end of the file, specify the line number as $.  This is another command that is especially useful when used with a range of line numbers.\n");
        printf("i: Begin inserting text before the current line.\n");
        printf("a: Begin inserting text after the current line.\n");
        printf("w: Write the buffer to the file named on the command line.  Be sure to correctly handle the case where the file has become shorter.\n");
        printf("s: Search the file in the editor for the string following the s command.  Begin the search with the line after the current line and continue to the end of the file.  If the string is found, the current line becomes the line on which the string is found.\n");
        printf("|: The vertical bar (|) command causes the remainder of the line to be run as a command in a sub-process with the file buffer presented as the standard input and replaced with the standard output.\n");
        printf("q: Quit.\n");
        printf("\nEnter a command:\n");
        fgets(command, sizeof(command), stdin);
        switch (command[0]) {
            case 'p':
                printList(textBuffer, 0);
                break;
            case 'n':
                printList(textBuffer, 1);
                break;
            case 'i':
                fgets(str, BUFFER_SIZE, stdin);
                for (i = 0; i < BUFFER_SIZE; i++) {
                    if (str[i] == '\n') {
                        break;
                    }
                }
                str[i] = '\0';
                if (numCurrLine == 0) {
                    numCurrLine = 1;
                }
                strcat(str,"\n");
                insertAt(textBuffer, str, numCurrLine);
                currLine = getCurrLine(textBuffer, numCurrLine);
                break;
            case 'a':
                fgets(str, BUFFER_SIZE, stdin);
                for (i = 0; i < BUFFER_SIZE; i++) {
                    if (str[i] == '\n') {
                        break;
                    }
                }
                str[i] = '\0';
                strcat(str,"\n");
                insertAfter(textBuffer, str, numCurrLine);
                numCurrLine += 1;
                currLine = getCurrLine(textBuffer, numCurrLine);
                break;
            case 'q':
                exit(0);
                break;
            case 'm':
                if(isdigit(command[2]))
                    line_num = command[2] -'0';
                else if(command[2] == '$')
                    line_num = textBuffer->size;
                moveCurrLineTo(textBuffer, currLine, line_num);
                currLine = getCurrLine(textBuffer, numCurrLine);
                break;
            case 'w':

                fp2 = fopen(fileName, "w+");
                n = textBuffer->head;
                printf("%s", n->data);
                while(n!=NULL)
                {
                    fprintf(fp2, "%s", (n->data));
                    n = n->next;
                }
                fclose(fp2);
                break;
            case 's':
                for (i = 0; i < BUFFER_SIZE; i++) {
                    if (command[i] == '\n') {
                        break;
                    }
                }
                command[i] = '\0';
                ret = findString(textBuffer, command+2);
                if(ret != NULL) {
                    currLine = ret->currLine;
                    numCurrLine = ret->numCurrLine;
                }
                break;
            case 'd':
                if (numCurrLine == 0) {
                    printf("Buffer is empty\n");
                } else {
                    deleteNode(textBuffer, currLine);
                    numCurrLine--;
                    currLine = getCurrLine(textBuffer, numCurrLine);
                }
                break;
            case '|':
                fp2 = fopen(fileName, "w+");
                n = textBuffer->head;
                printf("%s", n->data);
                while(n!=NULL)
                {
                    fprintf(fp2, "%s", (n->data));
                    n = n->next;
                }
                i = 0;
                // build shell command
                while(command[i]!='\0')
                {
                    *(path + i) = command[i+1];
                    i++;
                }
                *(path + i) = '\0';
    
                p = popen(path, "r+");
                if (p == NULL)
                {
                    printf("Non-existent file\n");
                    return 1;
                }
                while((ch=fgetc(p))!=EOF)
                {
                    putchar(ch);
                }
                pclose(p); 
                fclose(fp2);      
                break;
            default:
                if (isdigit(command[0])) {
                    // move curr line to number
                    int moveTo = command[0] - '0';
                    if (moveTo <= textBuffer->size) {
                        int i;
                        struct Node* temp = textBuffer->head;
                        for (i = 1; i < moveTo; i++) {
                            temp = temp->next;
                        }
                        currLine = temp;
                        numCurrLine = moveTo;
                    } else {
                        printf("line doesn't exist\n");
                    }
                } else {
                    printf("wrong option, try again\n");
                }
                break;
        }
    }
}
