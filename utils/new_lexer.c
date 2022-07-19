#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define noop
#define DEFAULT_TOKEN_SIZE 128
#define STARTING_TOKENS 16
#define TOTAL_OPERANDS 5


typedef enum operands_t {
    O_PIPE,
    O_OR,
    O_AND,
    O_RE,
    O_APP,
    O_NONE  /* token is not an operand */

} operands_t;


typedef struct token_t {
    char *str;               /* the actual token string */
    enum operands_t type;    /* the semantic intepretation of the token */
    size_t str_len;          /* the current len of the token string */
    size_t str_allocated;    /* the allocated size of the token string */
} token_t;


typedef struct tokenized_str_t {
    struct token_t **tokens;  /* list of the tokens */
    size_t total_tokens;      /* actual total tokens in the token list */
    size_t tokens_allocated;  /* totoal allocated tokens */
} tokenized_str_t;


struct token_t *token_t_malloc();  /* sets the type to O_NONE by default */
void token_t_free(struct token_t *t);
void token_t_resize(struct token_t *t, size_t new_size);

struct tokenized_str_t *tokenized_str_t_malloc();
void tokenized_str_t_free(struct tokenized_str_t *ts);
void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_size);


struct token_t *token_t_malloc()
{
    struct token_t *t = (struct token_t *) malloc(sizeof(struct token_t));
    t->str = (char *) malloc(DEFAULT_TOKEN_SIZE * sizeof(char));
    t->type = O_NONE;
    t->str_len = 0;
    t->str_allocated = DEFAULT_TOKEN_SIZE;

    return t;
}

void token_t_free(struct token_t *t)
{
    if (t == NULL)
        return;

    free(t->str);
    free(t);
}

void token_t_resize(struct token_t *t, size_t new_size)
{
    t->str = (char *) realloc(t->str, new_size * sizeof(char));
    t->str_allocated = new_size;
}


struct tokenized_str_t *tokenized_str_t_malloc() 
{
    struct tokenized_str_t *ts = (struct tokenized_str_t *) malloc(sizeof(struct tokenized_str_t));

    ts->tokens = (struct token_t **) malloc(STARTING_TOKENS * sizeof(struct token_t *));
    for (int i = 0; i < STARTING_TOKENS; i++)
        ts->tokens[i] = token_t_malloc();

    ts->total_tokens = 0;
    ts->tokens_allocated = STARTING_TOKENS;

    return ts;
}

void tokenized_str_t_free(struct tokenized_str_t *ts)
{
    if (ts == NULL)
        return;

    for (size_t i = 0; i < ts->tokens_allocated; i++)
        token_t_free(ts->tokens[i]);

    free(ts);
}

void tokenized_str_t_resize(struct tokenized_str_t *ts, size_t new_size)
{
    ts->tokens = (struct token_t **) realloc(ts->tokens, new_size * sizeof(struct token_t *));
    for (size_t i = ts->tokens_allocated; i < new_size; i++)
        ts->tokens[i] = token_t_malloc();

    ts->tokens_allocated = new_size;
}

/* just for debugging purpsos */
void tokenized_str_t_print(struct tokenized_str_t *ts)
{
    printf("metadata: total tokens: %ld, total tokens allocated: %ld\n\n", ts->total_tokens + 1, ts->tokens_allocated);

    for (size_t i = 0; i < ts->total_tokens + 1; i++)
        printf("token num %ld, token str: '%s', token type: %d, token len: %ld, token allocated: %ld\n", i, ts->tokens[i]->str, ts->tokens[i]->type, ts->tokens[i]->str_len, ts->tokens[i]->str_allocated);
}


const char *operands[] = {
    "|",  /* O_PIPE */
    "||", /* O_OR   */
    "&&", /* O_AND  */
    ">",  /* O_RE   */
    ">>"  /* O_APP  */
};


char *my_strtok(char *str, const char **delims, char **save_ptr)
{
    char *pos;
    char a;
    /* first call to my_strtok should pass NULL as the 
       save_ptr parameter. Resulting calls to my_strtok will use the 
       offset of the save_ptr as the beginning of the token */
    if (*save_ptr == NULL)
        pos = str;
    else
        pos = *save_ptr;

    while (*pos != 0 && *pos != ' ') {
        printf("%c", *(pos++));
        continue;
    }

    if (*pos == 0)
        return NULL;

    *save_ptr = ++pos;
    return str;
}

bool bool_in_list(bool *list, size_t len, bool item)
{
    for (size_t i = 0; i < len; i++) {
        if (list[i] == item)
            return true;
    }
    return false;
}

int occurence_in_list(bool *list, size_t len, bool item)
{
    int s = 0;
    for (size_t i = 0; i < len; i++) {
        if (list[i] == item)
            s++;
    }
    return s;
}


bool possible_delims(char c, size_t pos, bool pd[TOTAL_OPERANDS])
{
    //printf("\n");
    for (int i = 0; i < TOTAL_OPERANDS; i++) {
        if (!bool_in_list(pd, TOTAL_OPERANDS, true))
            return false;

        if (operands[i][pos] != c) {
            //printf("%c - %c\n", operands[i][pos], c);
            pd[i] = false;
        }
    }

    return bool_in_list(pd, TOTAL_OPERANDS, true);
}

void tokenize(struct tokenized_str_t *ts, char *buffer)
{
    char **token_str_ptr;
    char c;
    bool pd[TOTAL_OPERANDS];
    char token[1024];
    bool is_cmd = true;
    size_t token_len = 0;

    while ((c = *buffer++) != 0) {
        memset(pd, true, TOTAL_OPERANDS);

        if (possible_delims(c, 0, pd)) {
            //for (int i = 0; i < TOTAL_OPERANDS; i++) {
            //    printf("%d\n", pd[i]);
            //}

            ts->total_tokens++;

            ts->tokens[ts->total_tokens]->str[0] = c;
            token_len = 1;

            while ((c = *buffer++) != 0) {
                ts->tokens[ts->total_tokens]->str[token_len] = c;
                possible_delims(c, token_len, pd);
                int len = occurence_in_list(pd, TOTAL_OPERANDS, true);

                if (len == 1) {
                    printf("\nA\n");
                    ts->total_tokens++;
                    break;
                }

                if (len == 0) {
                    ts->tokens[ts->total_tokens]->str[token_len] = 0;

                    bool found = false;
                    for (int i = 0; i < TOTAL_OPERANDS; i++) {
                        if (strcmp(operands[i], ts->tokens[ts->total_tokens]->str) == 0) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        buffer--;
                        ts->total_tokens++;
                        break;
                    }

                    if (!found) {
                        printf("SYNTAX ERROR near: '%c' or '%s'\n", c, buffer);
                        return;
                    }
                }
            }

            token_len = 0;

        } else {
            printf("added: %c, to token nr: %ld at position: %ld\n", c, ts->total_tokens, token_len);
            ts->tokens[ts->total_tokens]->str[token_len++] = c;
        }
    }
}


void test_token_t()
{
    struct tokenized_str_t *ts = tokenized_str_t_malloc();
    char *buffer = (char *) malloc(1024 * sizeof(char));
    strcpy(buffer, "ls -la | grep valery || abc");
    tokenize(ts, buffer);
    tokenized_str_t_print(ts);
}


int main()
{
    test_token_t();
    return 0;
    /* ls -la | grep zsh */
    /*
    1. ls -la
    2. |
    3. grep zsh
    */
    char *token;
    char str[] = "ls -la | grep zsh";
    char *context = NULL;

    token = my_strtok(str, operands, &context);
    printf("\n%s\n", token);
    while (token != NULL) {
        printf("\n%s\n", token);
        token = my_strtok(str, operands, &context);
    }



    return 0;
}
