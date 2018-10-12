/*
Задача B-3. Парсер
Требуется написать программу, которая способна вычислять арифметические выражения. 
Выражения могут содержать: 
1) знаки операций '+', '-', '/', '*' 
2) Скобки '(', ')' 
3) Целые и вещественные числа, в нотации '123', '123.345', все операции должны быть вещественны, а результаты выведены с точностю до двух знаков после запятой в том числе целые '2.00' 
4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего не значат 
5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "[error]" (без кавычек)
*/

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define ALLOCATE_MEMORY_ERROR 1

#define is_operator(c) (c == '+' || c == '-' || c == '/' || c == '*')
#define is_ident(c) ((c >= '0' && c <= '9') || c == '.')

int op_preced(const char c)
{
    switch(c)
    {
        case '*':
        case '/':
            return 3;
            
        case '+':
        case '-':
            return 2;
    }
    
    return 0;
}

bool op_left_assoc(const char c)
{
    switch(c)
    {
        case '*':
        case '/':
        case '+':
        case '-':
            return true;
    }
    
    return false;
}

unsigned int op_arg_count(const char c)
{
    switch(c)
    {
        case '*':
        case '/':
        case '+':
        case '-':
            return 2;
    }
    return 0;
}

double bin_count(double num1, double num2, const char c)
{
    switch(c)
    {
        case '*':
            return num1 * num2;
        case '/':
            return num1 / num2;
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
    }
    
    return 0;
}

double un_count(double num1, const char c)
{
    switch(c)
    {
        case '-':
            return -num1;
        case '+':
            return num1;
    }
    
    return 0;
}

int is_ident_str(char *str)
{
    unsigned int flag = 0;
    
    for(int i = 0; i < strlen(str); ++i)
    {
        if(!(is_ident(str[i])))
            flag = 1;
    }
    
    return flag;
}

bool parse(const char *input, char *output)
{
    const char *strpos = input, *strend = input + strlen(input);
    char c = 0;
    char stack[32];
    char sc = 0;
    char *outpos = output;
    unsigned int sl = 0;
    
    while(strpos < strend)
    {
        c = *strpos;
        if(c != ' ')
        {
            if(is_ident(c))
            {
                *outpos = c;
                ++outpos;
                c = *(strpos + 1);
                if(!is_ident(c))
                {
                    *outpos = '|';
                    ++outpos;
                }
            }
            else if(is_operator(c))
            {
                while(sl > 0)
                {
                    sc = stack[sl-1];
                    if(is_operator(sc) &&
                       ((op_left_assoc(c) && (op_preced(c) <= op_preced(sc))) ||
                        (!op_left_assoc(c) && (op_preced(c) < op_preced(sc)))))
                    {
                        *outpos = sc;
                        ++outpos;
                        *outpos = '|';
                        ++outpos;
                        sl--;
                    }
                    else
                    {
                        break;
                    }
                }
                stack[sl] = c;
                ++sl;
            }
            else if(c == '(')
            {
                stack[sl] = c;
                ++sl;
            }
            else if(c == ')')
            {
                bool pe = false;
                while(sl > 0)
                {
                    sc = stack[sl - 1];
                    if(sc == '(')
                    {
                        pe = true;
                        break;
                    }
                    else
                    {
                        *outpos = sc;
                        ++outpos;
                        *outpos = '|';
                        ++outpos;
                        sl--;
                    }
                }
                if(!pe)
                {
                    return false;
                }
                sl--;
                if(sl > 0)
                {
                    sc = stack[sl - 1];
                }
            }
            else
            {
                return false;
            }
        }
        ++strpos;
    }
    while(sl > 0)
    {
        sc = stack[sl - 1];
        if(sc == '(' || sc == ')')
        {
            return false;
        }
        *outpos = sc;
        ++outpos;
        *outpos = '|';
        ++outpos;
        --sl;
    }
    
    *outpos = 0;
    return true;
}

bool execution_order(char *input, double *result)
{
    char c;
    double stack[32];
    unsigned int sl = 0;
    double num1 = 0;
    double num2 = 0;
    int count = 0;
    char *s = strtok(input, "|");
    
    while(s != NULL)
    {
        if(!is_ident_str(s))
        {
            stack[sl] = atof(s);
            ++sl;
            count++;
        }
        else
        {
            c = s[0];
            unsigned int nargs = op_arg_count(c);
            if(sl < nargs)
            {
                num1 = stack[sl-1];
                stack[sl - 1] = un_count(num1, c);
                sl--;
            }
            else
            {
                num1 = stack[sl - 2];
                num2 = stack[sl - 1];
                stack[sl-2] = bin_count(num1, num2, c);
                sl -= 2;
            }
            ++sl;
        }
        
        s = strtok(NULL, "|");
    }
    
    if(sl == 1 && count >= 1)
    {
        *result = stack[sl - 1];
        sl--;
        return true;
    }
    
    return false;
}

void free_matrix(char **data, size_t n)
{
    for(int i = 0; i < n; i++)
    {
        free(data[i]);
    }
    
    free(data);
}

int add_row(char *(**matrix), char *string, size_t n) {
    
    int error = 0;
    
    if((*matrix) == NULL || matrix == NULL)
    {
        (*matrix) = calloc(1, sizeof(char*));
        if((*matrix) != NULL)
        {
            (*matrix)[0] = malloc((strlen(string) + 1) * sizeof(char));
            if((*matrix)[0] != NULL)
            {
                strcpy((*matrix)[0], string);
            }
            else
            {
                free_matrix((*matrix), (n-1));
                return ALLOCATE_MEMORY_ERROR;
            }
        }
        else
        {
            free_matrix((*matrix), (n-1));
            return ALLOCATE_MEMORY_ERROR;
        }
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
                {
                    strcpy(tmp[i], (*matrix)[i]);
                }
                else
                {
                    free_matrix((*matrix), (n-1));
                    free_matrix(tmp, n);
                    return ALLOCATE_MEMORY_ERROR;
                }
            }
            
            tmp[n-1] = malloc((strlen(string)+1) * sizeof(char));
            if(tmp[n-1] != NULL)
            {
                strcpy(tmp[n-1], string);
            }
            else
            {
                free_matrix((*matrix), (n-1));
                free_matrix(tmp, n);
                return ALLOCATE_MEMORY_ERROR;
            }
            
            free_matrix((*matrix), (n-1));
            
            (*matrix) = calloc(n, sizeof(char*));
            if((*matrix) != NULL)
            {
                for(int i = 0; i < n; i++)
                {
                    (*matrix)[i] = malloc((strlen(tmp[i])+1) * sizeof(char));
                    if((*matrix)[i] != NULL)
                    {
                        strcpy((*matrix)[i], tmp[i]);
                    }
                    else
                    {
                        free_matrix((*matrix), (n-1));
                        free_matrix(tmp, n);
                        return ALLOCATE_MEMORY_ERROR;
                    }
                }
            }
            else
            {
                free_matrix((*matrix), (n-1));
                free_matrix(tmp, n);
                return ALLOCATE_MEMORY_ERROR;
            }
        }
        else
        {
            free_matrix((*matrix), (n-1));
            free_matrix(tmp, n);
            return ALLOCATE_MEMORY_ERROR;
        }
        free_matrix(tmp, n);
    }
    
    return error;
}

int readLine(char *buffer, char *(**matrix), size_t *n)
{
    int ch, i = 0;
    int error = 0;
    
    while((ch = getc(stdin)))
    {
        buffer[i++] = ch;
        
        if(ch == EOF)
        {
            *n = *n + 1;
            buffer[i-1] = '\0';
            error = add_row(matrix, buffer, *n);
            i = 0;
            break;
        }
        if(ch == '\n' || ch == EOF)
        {
            *n = *n + 1;
            buffer[i-1] = '\0';
            error = add_row(matrix, buffer, *n);
            i = 0;
        }
    }
    
    return error;
}

int one_result(char *str, double *res)
{
    char output[128];
    int error = 0;
    
    if(parse(str, output))
    {
        error = execution_order(output, res);
    }
    
    return error;
}

size_t count_str(char **data, size_t n, double *result)
{
    size_t count = 0;
    double res = 0;
    
    for(int i = 0; i < n; i++)
    {
        if(one_result(data[i], &res))
        {
            result[count] = res;
            count++;
        }
    }
    
    return count;
}

int main(void)
{
    char **matrix = NULL;
    double *result = NULL;
    char *buffer = malloc(1000 * sizeof(char));
    size_t count = 0;
    size_t n = 0;
    int error = 0;
    
    error = readLine(buffer, &matrix, &n);
    result = malloc(n * sizeof(double));
    
    if(error)
    {
        printf("[error]");
    }
    else
    {
        count = count_str(matrix, n, result);
        if(count == 0)
        {
            printf("[error]");
        }
        else
        {
            for(int i = 0; i < count; i++)
            {
                printf("%0.2f\n", result[i]);
            }
        }
    }
    
    if(matrix != NULL)
    {
        free_matrix(matrix, n);
        free(result);
    }

    free(buffer);
}
