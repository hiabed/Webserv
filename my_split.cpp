#include "webserv.hpp"

char** my_split(char *str, char delim) 
{
    int num_tokens = 1;
    for (char *p = str; *p; ++p) 
    {
        if (*p == delim)
            ++num_tokens;
    }
    char **tokens = new char*[num_tokens + 1];
    if (!tokens)
        return NULL;
    for (int i = 0; i < num_tokens; ++i)
        tokens[i] = NULL;
    int token_index = 0;
    char *token = strtok(str, &delim);
    while (token != NULL) {
        tokens[token_index] = new char[strlen(token) + 1];
        if (!tokens[token_index]) {
            for (int i = 0; i < token_index; ++i) {
                delete[] tokens[i];
            }
            delete[] tokens;
            return NULL;
        }
        strcpy(tokens[token_index], token);
        token = strtok(NULL, &delim);
        ++token_index;
    }
    tokens[num_tokens] = NULL;
    return tokens;
}