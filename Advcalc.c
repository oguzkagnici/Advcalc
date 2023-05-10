#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAXLENGTH  257

int error = 1; //Error tracker to check whether the evaluation is successful.

char *variables[129]; //Array to store defined variables.
long long values[129]; //Array to store values of the corresponding variables.
int currentindex = 0; //Counter to add variables faster to the arrays.
char *cp;


void skipwhitespace(const char **); //Helper function to skip unnecessary whitespaces.

long long eval(const char *expression); //Outer function to pass pointer to the main evaluator function.

char *getvar(const char *expression); //Helper function to get alphabetic variable names.

long long evalor(const char **); //Initial function that evaluates the given expression by calling the sub-functions.

long long evalexpression(const char **); //Sub-function that calculates expressions which include "&" statements.

long long evaloperations(const char **); //Sub-function that calculates expressions which include "+","-" statements.

long long evalterms(const char **); //Sub-function that calculates expressions which include "*" statements.

long long evalfactor(const char **); //Sub-function that calculates the highest precedence elements such as variables.

long long gettoken(const char **, long long *, char *); //Helper function that returns the type of the next token.

void ungettoken(long long, const char **); //Helper function that ungets the current token.

char peeknext(const char **); //Helper function that checks the next character.

int main() {

    char line[MAXLENGTH]; //Initializing the expression line.
    cp = malloc(257 * sizeof(char));

    while (1) {
        printf("> ");
        error = 1;
        if (fgets(line, sizeof line, stdin) == NULL) break;
        char *linepointer = line;
        while (isspace(*linepointer))
            linepointer++;
        if (strlen(linepointer) == 0)
            continue;

        else if (strchr(line, '%') && !strchr(line, '=')) {//Striping the line if it contains comments.
            if (line[0] == '%')
                continue;
            char *newline = strtok(line, "%");
            char *newlinepointer = newline;
            while (isspace(*newlinepointer))
                newlinepointer++;
            if (strlen(newlinepointer) == 0)
                continue;

            long long result = eval(newline); //Calculating the expression.
            if (error == 1)
                printf("%lld\n", result);
            else {
                printf("Error!\n");
                continue;
            }


        } else if (strchr(line, '=')) { // Stripping the line if it is an assignment expression.
            if (line[0] == '=') {
                printf("Error!\n");
                continue;
            }
            cp = strchr(line, '=') + 1;
            if (strchr(cp, '=') && !strchr(cp, '%')) {
                printf("Error!\n");
                continue;
            } else if (strchr(line, '%')) {
                char *nocomment = strtok(line, "%");
                if (!strchr(line, '=')){
                    long long result = eval(line);
                    if (error == 1) {
                        printf("%lld\n", result);
                        continue;
                    }

                    else {
                        printf("Error!\n");
                        continue;
                    }
                }
            }

            int found = 0;
            int valid = 1;
            char *token = strtok(line, "=");
            char *variable = getvar(token);
            char *varpointer = token;
            int isend = 0;
            int counter = 0;
            while (isspace(*varpointer)) {
                varpointer++;
                counter++;
            }
            for (int i = counter; i < strlen(token); ++i) { //Checking if the variable name is valid.
                if (!isalpha(*varpointer)) {
                    if (isspace(*varpointer)) {
                        isend = 1;
                    } else {
                        valid = 0;
                        printf("Error!\n");
                        found = 1;
                        break;
                    }

                } else if (isend == 1) {
                    valid = 0;
                    printf("Error!\n");
                    found = 1;
                    break;
                }
                varpointer++;
            }
            if (valid == 0)
                continue;

            char *newline = strtok(NULL, "=");
            if (strlen(variable) == 0) {
                valid = 0;
                printf("Error!\n");
                found = 1;
                continue;
            }

            //Checking whether the variable is previously defined and calling the eval() function accordingly.
            for (int i = 0; i < sizeof(variables) / sizeof(char *) && variables[i] != NULL && valid == 1; ++i) {
                if (strcmp(variable, variables[i]) == 0) {
                    found = 1;
                    long long result = eval(newline);
                    if (error == 0) {
                        printf("Error!\n");
                    } else
                        values[i] = result;
                    break;
                }
            }
            if (found == 0) {
                long long result = eval(newline);
                if (error == 0) {
                    printf("Error!\n");
                    continue;
                } else {
                    variables[currentindex] = variable;
                    values[currentindex] = result;
                    currentindex++;
                }


            }


        } else {
            long long result = eval(line);
            if (error == 1)
                printf("%lld\n", result);
            else {
                printf("Error!\n");
                continue;
            }

        }
    }


    return (0);
}


long long eval(const char *expression) {
    long long result = evalor(&expression);
    skipwhitespace(&expression);
    if (*expression != '\0') {
        error = 0;
        return 0;
    }
    return result;
}

long long evalor(const char **p) {
    char nexttoken; //Used for checking the next token in the expression to ensure that it is valid.
    long long result = evalexpression(p);
    if (error == 0) //Stops if the higher precedence elements are invalid.
        return 0;
    while (1) { //Evaluating until there are no "|" statements.
        long long operator = gettoken(p, NULL, cp); //Checks whether the operator is "|".
        switch (operator) {
            case '|' :
                nexttoken = peeknext(p);
                if (nexttoken != '1' && nexttoken != 'a' && nexttoken != '(' && nexttoken != 'f' && nexttoken != 'n') {
                    error = 0;
                    break;
                }
                result = result | evalexpression(p);
                break;
            default:
                ungettoken(operator, p);
                return result;
        }
    }
}

long long evalexpression(const char **p) {
    char nexttoken;
    long long result = evaloperations(p);
    if (error == 0)
        return 0;
    while (1) { //Evaluating until there are no "&" statements.
        long long operator = gettoken(p, NULL, cp); //Checks whether the operator is "&".
        switch (operator) {
            case '&':
                nexttoken = peeknext(p);
                if (nexttoken != '1' && nexttoken != 'a' && nexttoken != '(' && nexttoken != 'f' && nexttoken != 'n') {
                    error = 0;
                    break;
                }
                result = result & evaloperations(p);
                break;
            default:
                ungettoken(operator, p);
                return result;
        }
    }
}

long long evaloperations(const char **p) {
    char nexttoken;
    long long result = evalterms(p);
    if (error == 0)
        return 0;
    while (1) { //Evaluating until there are no "+" or "-" statements.
        long long operator = gettoken(p, NULL, cp); //Checks whether the operator is "+" or "-".
        switch (operator) {
            case '+' :
                nexttoken = peeknext(p);
                if (nexttoken != '1' && nexttoken != 'a' && nexttoken != '(' && nexttoken != 'f' && nexttoken != 'n') {
                    error = 0;
                    break;
                }
                result += evalterms(p);
                break;
            case '-' :
                nexttoken = peeknext(p);
                if (nexttoken != '1' && nexttoken != 'a' && nexttoken != '(' && nexttoken != 'f' && nexttoken != 'n') {
                    error = 0;
                    break;
                }
                result -= evalterms(p);
                break;
            default:
                ungettoken(operator, p);
                return result;
        }
    }
}

long long evalterms(const char **p) {
    char nexttoken;
    long long result = evalfactor(p);
    if (error == 0)
        return 0;
    while (1) { //Evaluating until there are no "*" statements.
        long long operator = gettoken(p, NULL, cp); //Checks whether the operator is "*".
        switch (operator) {
            case '*' :
                nexttoken = peeknext(p);
                if (nexttoken != '1' && nexttoken != 'a' && nexttoken != '(' && nexttoken != 'f' && nexttoken != 'n') {
                    error = 0;
                    break;
                }
                result *= evalfactor(p);
                break;
            default:
                ungettoken(operator, p);
                return result;
        }
    }
}

long long evalfactor(const char **p) {
    long long v; //Holds the value of the highest precedence elements.
    long long v2; //Holds the value of the second expression if there is a function.
    char *cp = malloc(257 * sizeof(char));
    char nexttoken;
    long long operator = gettoken(p, &v,
                                  cp); //Checks whether the element is constant, variable, function or parenthesis.
    switch (operator) {
        case '1' :
            nexttoken = peeknext(p);
            if (nexttoken == '1' || nexttoken == 'a' || nexttoken == '(') {
                error = 0;
                break;
            }
            return v;
        case '(' :
            v = evalor(p);
            operator = gettoken(p, NULL, cp);
            if (operator != ')') {
                //printf("Error!") ;
                ungettoken(operator, p);
                error = 0;
                break;
            }
            return v;
        case 'n' :
            operator = gettoken(p, NULL, cp);
            if (operator != '(') {
                ungettoken(operator, p);
                error = 0;
                break;
            }
            v = evalor(p);
            operator = gettoken(p, NULL, cp);
            if (operator != ')') {
                ungettoken(operator, p);
                error = 0;
                break;
            }
            return ~v;

        case 'a' :
            nexttoken = peeknext(p);
            if (nexttoken == '1' || nexttoken == 'a' || nexttoken == '(') {
                error = 0;
                break;
            }
            for (int i = 0; i < sizeof(variables) / sizeof(char *) && variables[i] != NULL; ++i) {
                if (strcmp(cp, variables[i]) == 0) {
                    return values[i];
                }

            }
            return 0;
        case 'f' :
            operator = gettoken(p, NULL, cp);
            if (operator != '(') {
                ungettoken(operator, p);
                error = 0;
                break;
            }
            v = evalor(p);
            operator = gettoken(p, NULL, cp);
            if (operator != ',') {
                //printf("Error!");
                ungettoken(operator, p);
                error = 0;
                break;
            }
            v2 = evalor(p);
            operator = gettoken(p, NULL, cp);
            if (operator != ')') {
                //printf("Error!") ;
                ungettoken(operator, p);
                error = 0;
                break;
            }
            if (strcmp(cp, "xor") == 0)
                return v ^ v2;
            else if (strcmp(cp, "ls") == 0)
                return v << v2;
            else if (strcmp(cp, "rs") == 0)
                return v >> v2;
            else if (strcmp(cp, "lr") == 0){
                //v = (unsigned) v;
                return (long long) (((unsigned long long)v << v2) | ((unsigned long long) v >> (64 - v2)));
            }
            else if (strcmp(cp, "rr") == 0) {
                //v = (unsigned) v;
                return (long long) (((unsigned long long)v >> v2) | ((unsigned long long) v << (64 - v2)));
            }




        default:
            error = 0;
            return 0;
    }
}

long long gettoken(const char **p, long long *vp, char *cp) {
    skipwhitespace(p);
    char c = **p;
    if (isdigit(c)) { //Goes until the end of the number if the current character is a digit.
        char *p2;
        long long v = strtoll(*p, &p2, 0);
        *p = p2;
        if (vp) *vp = v;
        return '1';
    } else if (isalpha(
            c)) { //Goes until the end of the variable or function if the current character is a alphabetic character.
        int i = 0;
        int len = 0;
        while (isalpha(**p)) {
            (*p)++;
            len++;
        }
        for (int j = 0; j < len; ++j) {
            (*p)--;
        }
        char var[len + 1];
        var[len] = '\0';
        for (int k = 0; k < len; ++k) {
            var[k] = **p;
            (*p)++;
        }
        strcpy(cp, var);
        //Checks whether the token is a function name.
        if (strcmp(var, "xor") == 0 || strcmp(cp, "ls") == 0 || strcmp(cp, "rs") == 0 || strcmp(cp, "lr") == 0 ||
            strcmp(cp, "rr") == 0) {
            return 'f';
        } else if (strcmp(var, "not") == 0)
            return 'n';
        return 'a';
    } else {
        (*p)++;
        return c;
    }

}

char peeknext(const char **p) {
    skipwhitespace(p);
    char c;
    int counter = 0;
    if (isdigit(**p)) {
        while (isdigit(**p)) {
            (*p)++;
            counter++;
        }
        for (int i = 0; i < counter; ++i) {
            (*p)--;
        }
        return '1';
    } else if (isalpha(**p)) {
        int i = 0;
        int len = 0;
        while (isalpha(**p)) {
            (*p)++;
            len++;
        }
        for (int j = 0; j < len; ++j) {
            (*p)--;
        }
        char var[len + 1];
        var[len] = '\0';
        for (int k = 0; k < len; ++k) {
            var[k] = **p;
        }
        if (strcmp(var, "xor") == 0 || strcmp(cp, "ls") == 0 || strcmp(cp, "rs") == 0 || strcmp(cp, "lr") == 0 ||
            strcmp(cp, "rr") == 0) {
            return 'f';
        } else if (strcmp(var, "not") == 0)
            return 'n';
        return 'a';

    } else {
        c = **p;
        return c;
    }
}

void skipwhitespace(const char **p) {
    while (isspace(**p)) { //Moves the pointer forward until there is no whitespace.
        (*p)++;
    }
}


void ungettoken(long long operator, const char **p) {
    (*p)--; //Moves the pointer backwards.
}

char *getvar(const char *expression) {
    char *variable = malloc(257 * sizeof(char));
    gettoken(&expression, NULL, variable);
    return variable;
}




