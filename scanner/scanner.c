#include "scanner.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 符号表内容
struct Token TokenTab[] = {
    // 命名常数
    {CONST_ID, "PI", 3.1415926, NULL, {0, 0}},
    {CONST_ID, "E", 2.71828, NULL, {0, 0}},
    // 参数
    {T, "T", 0.0, NULL, {0, 0}},

    // 支持的函数名
    {FUNC, "SIN", 0.0, sin, {0, 0}},
    {FUNC, "COS", 0.0, cos, {0, 0}},
    {FUNC, "TAN", 0.0, tan, {0, 0}},
    {FUNC, "LN", 0.0, log, {0, 0}},
    {FUNC, "EXP", 0.0, exp, {0, 0}},
    {FUNC, "SQRT", 0.0, sqrt, {0, 0}},

    // 保留关键字
    {ORIGIN, "ORIGIN", 0.0, NULL, {0, 0}},
    {SCALE, "SCALE", 0.0, NULL, {0, 0}},
    {ROT, "ROT", 0.0, NULL, {0, 0}},
    {IS, "IS", 0.0, NULL, {0, 0}},
    {FOR, "FOR", 0.0, NULL, {0, 0}},
    {FROM, "FROM", 0.0, NULL, {0, 0}},
    {TO, "TO", 0.0, NULL, {0, 0}},
    {STEP, "STEP", 0.0, NULL, {0, 0}},
    {DRAW, "DRAW", 0.0, NULL, {0, 0}},
};

FILE *in_file = NULL;
struct position current_pos = {1, 0};

// 初始化词法分析器
int InitScanner(const char *fileName)
{
    current_pos.line = 1;
    current_pos.col = 0;

    in_file = fopen(fileName, "r");
    if (in_file != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// 关闭词法分析器
void CloseScanner()
{
    if (in_file != NULL)
    {
        fclose(in_file);
    }
}

int pre_process(struct Token *pToken);
int scan_move(struct Token *pToken, int first_char);
int post_process(struct Token *pToken, int last_state);

// 识别并返回一个记号
// 遇到非法输入时，type=ERRTOKEN，文件结束时，type=NONTOKEN
struct Token GetToken()
{
    int first_char; // 记号开始的第一个字符
    int last_state = -1;
    struct position where;
    struct Token theToken;
    int to_bo_continue;

    do
    {
        // 第一步，预处理，跳过空白字符
        first_char = pre_process(&theToken);
        if (first_char == -1)
        {
            theToken.type = NONTOKEN;
            return theToken;
        }
        // 当前记号开始的位置
        theToken.where = where = current_pos;

        // 第二步，边扫描边输入，边转移状态
        last_state = scan_move(&theToken, first_char);

        // 第三步，后处理，根据终态所标记的记号种类信息，进行特殊处理
        to_bo_continue = post_process(&theToken, last_state);
    } while (to_bo_continue != 0);

    theToken.where = where;
    return theToken;
}

// 从输入源程序中读入一个字符并返回它
// 若遇到文件结束则返回-1
char GetChar(void)
{
    int next_char = getc(in_file);
    if (EOF == next_char)
    {
        return -1;
    }
    else
    {
        // 遇到换行
        if ('\n' == next_char)
        {
            ++(current_pos.line);
            current_pos.col = 0;
            return next_char;
        }
        else
        {
            ++(current_pos.col);
            return toupper(next_char);
        }
    }
}

// 把预读的字符退回到输入源程序中
void BackChar(char next_char)
{
    if (next_char == EOF || next_char == '\n')
    {
        return;
    }

    ungetc(next_char, in_file);
    --(current_pos.col);
}

// 判断所给的字符串是否在符号表中
struct Token JudgeKeyToken(const char *c_str)
{
    int count;
    struct Token err_token;

    for (count = 0; count < sizeof(TokenTab) / sizeof(TokenTab[0]); ++count)
    {
        if (strcmp(TokenTab[count].lexeme, c_str) == 0)
        {
            return TokenTab[count];
        }
    }

    memset(&err_token, 0, sizeof(err_token));
    err_token.type = ERRTOKEN;
    return err_token;
}

// 将字符c追加到记号文本末尾
// 若超长则不再追加并返回-1，否则返回0
int AppendTokenTxt(struct Token *pToken, char c)
{
    size_t len;
    len = strlen(pToken->lexeme);
    if (len + 1 >= sizeof(pToken->lexeme))
    {
        return -1;
    }
    pToken->lexeme[len] = c;
    pToken->lexeme[len + 1] = '\0';
    return 0;
}

int is_space(char c)
{
    // 处理非ASCII字符
    if (c < 0 || c > 0x7e)
    {
        return 0;
    }
    return isspace(c);
}

// 跳过空白字符，并读取第一个非空白字符
// 返回值：-1 文件结束，其他值表示字符本身
int pre_process(struct Token *pToken)
{
    int current_char;
    memset(pToken, 0, sizeof(struct Token));
    for (;;)
    {
        current_char = GetChar();
        if (current_char == -1)
        {
            return -1;
        }
        if (!is_space(current_char))
        {
            break;
        }
    }
    return current_char;
}

// DFA提供的接口操作，定义在dfa.c
extern int get_start_state();
extern int move(int state_src, char ch);
extern enum Token_Type state_is_final(int state);

int scan_move(struct Token *pToken, int first_char)
{
    int current_state, next_state;
    int current_char;

    current_char = first_char;
    current_state = get_start_state();
    for (;;)
    {
        next_state = move(current_state, current_char);
        if (next_state < 0)
        {
            if (pToken->lexeme[0] == '\0')
            {
                snprintf(pToken->lexeme, sizeof(pToken->lexeme), "\\x%02x",
                         (unsigned char)current_char);
            }
            else
            {
                BackChar(current_char);
            }
            break;
        }

        AppendTokenTxt(pToken, current_char);
        current_state = next_state;
        current_char = GetChar();
        if (current_char == -1)
        {
            break;
        }
    }
    return current_state;
}

// 根据终态所标记的记号种类信息，进行特殊处理
// 若返回非0，则表示当前刚处理完了注释，需要调用者接着获取下一个记号
int post_process(struct Token *pToken, int last_state)
{
    int to_bo_continue = 0;
    enum Token_Type tk = state_is_final(last_state);
    switch (tk)
    {
    case ID:
    {
        struct Token id = JudgeKeyToken(pToken->lexeme);
        if (ERRTOKEN == id.type)
        {
            pToken->type == ERRTOKEN;
        }
        else
        {
            *pToken = id;
        }
    }
    break;
    case CONST_ID:
    {
        pToken->type = tk;
        pToken->value = atof(pToken->lexeme);
    }
    break;
    case COMMENT:
    {
        // 行注释，忽略直到行尾的文本，并读取下一个记号
        int c;
        while (1)
        {
            c = GetChar();
            if (c == '\n' || c == -1)
            {
                break;
            }
        }
        to_bo_continue = 1;
    }
    break;
    default:
        pToken->type = tk;
        break;
    }
    return to_bo_continue;
}

struct tk_print
{
    enum Token_Type tk;
    const char *str;
};

#define MKSTR(x) \
    {            \
        x, #x    \
    }

struct tk_print tk_names[] = {
    MKSTR(ORIGIN), MKSTR(SCALE), MKSTR(ROT), MKSTR(IS),
    MKSTR(TO), MKSTR(STEP), MKSTR(DRAW), MKSTR(FOR),
    MKSTR(FROM), MKSTR(T),

    MKSTR(SEMICO), MKSTR(L_BRACKET), MKSTR(R_BRACKET), MKSTR(COMMA),

    MKSTR(PLUS), MKSTR(MINUS), MKSTR(MUL), MKSTR(DIV),
    MKSTR(POWER), MKSTR(FUNC), MKSTR(CONST_ID), MKSTR(ERRTOKEN),

    MKSTR(NONTOKEN)};

const char *token_type_str(enum Token_Type tk)
{
    struct tk_print *p = tk_names;
    for (; p->tk != NONTOKEN; ++p)
    {
        if (p->tk == tk)
        {
            return p->str;
        }
    }
    return "UNKNOW";
}