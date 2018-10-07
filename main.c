/* 
Задача A-7. Задача о скобочной последовательности
Составить программу построчной обработки текста. Суть обработки - отбор строк, содержащих одинаковое количество открывающих и закрывающих круглых скобок. 

Программа считывает входные данные со стандартного ввода, и печатает результат в стандартный вывод.

Процедура отбора нужных строк должна быть оформлена в виде отдельной функции, которой на вход подается массив строк (который необходимо обработать), количество переданных строк, а также указатель на переменную, в которой необходимо разместить результат - массив отобранных строк. 
В качестве возвращаемого значения функция должна возвращать количество строк, содержащихся в результирующем массиве. 

Программа должна уметь обрабатывать ошибки - такие как неверные входные данные(отсутствие входных строк) или ошибки выделения памяти и т.п.
В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и завершать выполнение программы. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOCATE_MEMORY_ERROR 1

typedef struct Node_tag {
    char value;
    struct Node_tag *next;
} Node;


void printStack(const Node* head) {
    printf("stack >");
    while (head) {
        printf("%c ", head->value);
        head = head->next;
    }
}

size_t getSize(const Node *head) {
    size_t size = 0;
    while (head) {
        size++;
        head = head->next;
    }
    return size;
}

int isEmpty(const Node *head) {
    // если стэк пустой то 1 если нет, то 0
    return getSize(head) == 0 ? 1 : 0;
}

void push(Node **head, char value) {
    Node *tmp = malloc(sizeof(Node));
    if(tmp != NULL) {
        tmp->next = *head;
        tmp->value = value;
        *head = tmp;
    }
    else {
        printf("Can't allocate memory for element");
    }
}

char pop(Node **head) {
    Node *out;
    char value;
    if (*head == NULL) {
        return -5;
    }
    out = *head;
    *head = (*head)->next;
    value = out->value;
    free(out);
    return value;
}

void free_matrix(char **data, size_t n)
{
    for(int i = 0; i < n; i++)
        free(data[i]);
    
    free(data);
}

int addRow(char *(**matrix), char *string, size_t n) {
    
    int error = 0;
    
    if((*matrix) == NULL)
    {
        (*matrix) = calloc(1, sizeof(char*));
        if((*matrix) != NULL)
        {
            (*matrix)[0] = malloc((strlen(string) + 1) * sizeof(char));
            if((*matrix)[0] != NULL)
                strcpy((*matrix)[0], string);
            else
                error = ALLOCATE_MEMORY_ERROR;
        }
        else
            error = ALLOCATE_MEMORY_ERROR;
    }
    else
    {
        char **tmp = calloc(n, sizeof(char*));
        if(tmp != NULL)
        {
            for(int i = 0; i < (n-1); i++)
            {
                tmp[i] = malloc((strlen((*matrix)[i])+1) * sizeof(char));
                if(tmp[i] != NULL)
                    strcpy(tmp[i], (*matrix)[i]);
                else
                    error = ALLOCATE_MEMORY_ERROR;
            }
            
            tmp[n-1] = malloc((strlen(string)+1) * sizeof(char));
            if(tmp[n-1] != NULL)
                strcpy(tmp[n-1], string);
            else
                error = ALLOCATE_MEMORY_ERROR;
            
            free_matrix((*matrix), (n-1));
            
            (*matrix) = calloc(n, sizeof(char*));
            if((*matrix) != NULL)
            {
                for(int i = 0; i < n; i++)
                {
                    (*matrix)[i] = malloc((strlen(tmp[i])+1) * sizeof(char));
                    if((*matrix)[i] != NULL)
                        strcpy((*matrix)[i], tmp[i]);
                    else
                        error = ALLOCATE_MEMORY_ERROR;
                }
            }
            else
                error = ALLOCATE_MEMORY_ERROR;
        }
        else
            error = ALLOCATE_MEMORY_ERROR;
        free_matrix(tmp, n);
    }
    
    return error;
}

void destroy(Node **s) {

    while (!isEmpty(*s))
        (void) pop(&*s);
}

int isBalance(char *string) {
    Node *head = NULL;
    char top = 0;
    size_t len = strlen(string);
    int count = 0;
    int res = 0;
    
    for(int i = 0; i < len; i++)
    {
        if(string[i] == '(')
        {
            push(&head, string[i]);
            count++;
        }
        if(string[i] == ')')
        {
            count++;
            if(isEmpty(head))
                return 0;
            top = pop(&head);
            if((top == '(' && string[i] != ')'))
                return 0;
        }
    }

    res = isEmpty(head);

    destroy(&head);

    return res;
}

size_t balance(char **data, size_t n, char *(**result)) {
    
    size_t count = 0;
    int error = 0;
    
    for(int i = 0; i < n; i++)
    {
        if(isBalance(data[i]))
        {
            count++;
            error = addRow(&(*result), data[i], count);
        }
    }
    
    return error ? 0 : count;
}

int readLine(char *buffer, char *(**matrix), size_t *n) {
    int ch, i = 0;
    int error = 0;
    
    while((ch = getc(stdin)))
    {
        buffer[i++] = ch;
        
        if(ch == EOF)
        {
            *n = *n + 1;
            buffer[i-1] = '\0';
            error = addRow(&(*matrix), buffer, *n);
            i = 0;
            break;
        }
        if(ch == '\n' || ch == EOF)
        {
            *n = *n + 1;
            buffer[i-1] = '\0';
            error = addRow(&(*matrix), buffer, *n);
            i = 0;
        }
    }
    
    return error;
}

int main(int argc, const char * argv[]) {
    
    char **matrix = NULL;
    char **result = NULL;
    char *buffer = malloc(1000 * sizeof(char));
    size_t count = 0;
    size_t n = 0;
    int error = 0;
    
    error = readLine(buffer, &matrix, &n);
    
    if(error)
        printf("[error]");
    else
    {
        count = balance(matrix, n, &result);
        if(count == 0)
            printf("[error]");
        else
        {
            for(int i = 0; i < count; i++)
                printf("%s\n", result[i]);
        }
    }
    
    if(matrix != NULL)
    {
        free_matrix(matrix, n);
        free_matrix(result, count);
    }
    free(buffer);
     
    return 0;
}
