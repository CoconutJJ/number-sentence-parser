/*
 * Number Sentence Parser
 *
 * Let G be the set of valid number sentences.
 *
 * A number sentence, P, is a member of G, if P satisfies:
 *
 * Base Case: P = c where c is in R
 * Inductive Step:
 * P = (A k B) where k is a member of {+,-,*,/} and A,B are members of G
 * OR
 * P = -A where A is a member of G
 *
 *
 * Author: David Yue
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __astNode {
    struct __astNode *left;
    struct __astNode *right;
    char symbol[];

} ASTNODE;

ASTNODE *newAST(char *symbol) {
    
    ASTNODE *new = malloc(sizeof(ASTNODE) + strlen(symbol) + 1);
    strcpy(new->symbol, symbol);
    new->left = NULL;
    new->right = NULL;

    return new;
}

int isNumeralSymbol(char symbol) { return (('0' <= symbol) && (symbol <= '9')) || symbol == '.'; }

int isNumeral(char *expression) {
    int hasDecimal = 0;

    for (int i = 0; i < strlen(expression); i++) {
        if (isNumeralSymbol(expression[i])) {
            continue;
        }
        if (expression[i] == '.') {
            if (!hasDecimal) {
                hasDecimal = 1;
            } else {
                return 0;
            }
            continue;
        }

        return 0;
    }

    return 1;
}

int isOperatorSymbol(char symbol) {
    switch (symbol) {
        case '+':
        case '-':
        case '*':
        case '/':
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

int isBracketSymbol(char symbol) {
    switch (symbol) {
        case '(':
            return 1;
            break;
        case ')':
            return -1;
            break;
        default:
            return 0;
            break;
    }
}

char *removeOuterMostParen(char *expression) {
    int last_chr_index = strlen(expression) - 1;

    if (expression[0] != '(' || expression[last_chr_index] != ')')
        return expression;

    int bracket_count = -1;
    int curr_index = 0;

    while (bracket_count != 0 && curr_index <= last_chr_index) {
        if (bracket_count == -1) bracket_count = 0;

        bracket_count += isBracketSymbol(expression[curr_index]);

        curr_index++;
    }

    if (curr_index > last_chr_index) {
        expression[0] = '\0';
        expression[last_chr_index] = '\0';
        return removeOuterMostParen(expression + 1);
    } else {
        return expression;
    }
}

ASTNODE *extractFirstNumeral(char *expression) {
    char *start = expression;

    int digitCount = 0, hasDecimal = 0;

    for (;;) {
        if (isNumeralSymbol(*expression)) {
            expression++;
            digitCount++;
            continue;
        }
        if (*expression == '.') {
            expression++;
            if (!hasDecimal) {
                hasDecimal = 1;
            } else {
                fprintf(stderr,
                        "Expected floating point value, but numeral has 2 "
                        "decimal points: %s",
                        expression);
                exit(EXIT_FAILURE);
            }
            continue;
        }

        break;
    }

    int index = 0;
    char numeral[digitCount + 1];
    expression = start;
    for (;;) {
        if (isNumeralSymbol(*expression) || *expression == '.') {
            numeral[index] = *expression;
            *expression = '\0';
            expression++;
            index++;
            continue;
        }
        break;
    }
    numeral[index] = '\0';

    return newAST(numeral);
}



ASTNODE *parse(char *expression) {
    
    int last_chr_index = strlen(expression) - 1;

    ASTNODE *symbol;

    // base case: we have a numeral 0 through 9
    if (isNumeralSymbol(expression[0])) {
        return extractFirstNumeral(expression);
    }

    // case: negative sign. we convert -(AkB) to the normal form: 0 - (AkB)
    if (expression[0] == '-') {
        ASTNODE *neg = newAST("-");
        ASTNODE *zero = newAST("0");

        neg->left = zero;

        neg->right = parse(&(expression[1]));

        return neg;
    }

    // remove all outer parentheses
    expression = removeOuterMostParen(expression);

    // PDA, loop until we reach the "middle operator"
    int bracket_count = -1;
    int curr_index = 0;

    while (!isOperatorSymbol(expression[curr_index]) ||
           (bracket_count != 0 && curr_index <= (last_chr_index - 1))) {
        if (bracket_count == -1) bracket_count = 0;
        bracket_count += isBracketSymbol(expression[curr_index]);
        curr_index++;
    }

    // in the case we get no operator i.e (-123) should be a valid expression.
    if (curr_index > last_chr_index - 1) {
        return parse(expression);
    }

    // create ASTNODE for "middle operator"
    char op = expression[curr_index];
    ASTNODE *head = newAST(&op);
    expression[curr_index] = '\0';

    // parse the left side and right side.
    head->left = parse(&(expression[0]));
    head->right = parse(&(expression[curr_index + 1]));

    return head;
}

double evaluate(ASTNODE *head) {
    switch (head->symbol[0]) {
        case '+':
            return evaluate(head->left) + evaluate(head->right);
            break;
        case '-':
            return evaluate(head->left) - evaluate(head->right);
            break;
        case '*':
            return evaluate(head->left) * evaluate(head->right);
            break;
        case '/':
            return evaluate(head->left) / evaluate(head->right);
            break;
        default: ;
            return strtod(head->symbol, NULL); 
            
            break;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *src_fd = fopen(argv[1], "r");

    char c;
    int char_count = 0;

    while ((c = fgetc(src_fd)) != EOF) {
        if (isOperatorSymbol(c) || isBracketSymbol(c) || isNumeralSymbol(c)) {
            char_count++;
        } else if (c != ' ') {
            fprintf(stderr, "Unrecognized symbol: %c", c);
            exit(EXIT_FAILURE);
        }
    }

    char expression[char_count + 1];

    fseek(src_fd, 0, SEEK_SET);

    int index = 0;

    while ((c = fgetc(src_fd)) != EOF) {
        if (isOperatorSymbol(c) || isBracketSymbol(c) || isNumeralSymbol(c)) {
            expression[index] = c;
            index++;
        }
    }

    expression[index] = '\0';

    ASTNODE *head = parse(expression);

    printf("%f", evaluate(head));
}