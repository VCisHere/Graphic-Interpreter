#ifndef _SCANNER_H
#define _SCANNER_H

enum Token_Type
{
    COMMENT, //注释
    ID,      //用于表驱动型分析器，可识别下述所有单词

    ORIGIN,
    SCALE,
    ROT,
    IS,
    TO,
    STEP,
    DRAW,
    FOR,
    FROM,
    T,

    SEMICO,
    L_BRACKET,
    R_BRACKET,
    COMMA,
    PLUS,
    MINUS,
    MUL,
    DIV,
    POWER,
    FUNC,
    CONST_ID,

    NONTOKEN,
    ERRTOKEN

};

typedef enum Token_Type Token_Type;

#define TOKEN_LEN_MAX 20

struct position
{
    unsigned int line;
    unsigned int col;
};

typedef double (*t_func)(double);

struct Token
{
    Token_Type type;
    char lexeme[TOKEN_LEN_MAX + 1];
    double value;
    t_func FuncPtr;

    struct position where;
};

int InitScanner(const char *fileName);

struct Token GetToken();

void CloseScanner();

const char *token_type_str(enum Token_Type tk);

#endif