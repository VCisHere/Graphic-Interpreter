#define NULL 0
#include "scanner.h"

#define CK_CHAR (0 << 16)
#define CK_LETTER (1U << 16)
#define CK_DIGIT (2U << 16)
#define CK_NULL (0x80U << 16)

typedef unsigned int t_key;
struct t_state_trans
{
    t_key idx;
    int state_to;
};

#define MK_IDX(from, c) ((t_key)(from << 24) | (c))
#define MOVE(from, c, to)   \
    {                       \
        MK_IDX(from, c), to \
    }
#define TRANS_END MK_IDX(255, CK_NULL)

// 状态转移矩阵定义
struct t_state_trans myTransfers[] = {
    MOVE(0, CK_LETTER, 1), MOVE(0, CK_DIGIT, 2), MOVE(0, '*', 4), MOVE(0, '/', 6), MOVE(0, '-', 7), MOVE(0, '+', 8), MOVE(0, ',', 9), MOVE(0, ';', 10), MOVE(0, '(', 11), MOVE(0, ')', 12), MOVE(1, CK_LETTER, 1), MOVE(1, CK_DIGIT, 1), MOVE(2, CK_DIGIT, 2), MOVE(2, '.', 3), MOVE(3, CK_DIGIT, 3), MOVE(4, '*', 5), MOVE(6, '/', 13), MOVE(7, '-', 13), {TRANS_END, 255}};

// 终态及定义
struct t_final_state
{
    int state;
    Token_Type kink;
};

struct t_final_state myFinalStates[] = {
    {1, ID}, {2, CONST_ID}, {3, CONST_ID},

    {4, MUL},
    {5, POWER},
    {6, DIV},
    {7, MINUS},

    {8, PLUS},
    {9, COMMA},
    {10, SEMICO},
    {11, L_BRACKET},
    {12, R_BRACKET},

    {13, COMMENT},

    {-1, ERRTOKEN}};

// DFA定义
struct DFA_definition
{
    int start_state;                   // 初态，唯一
    struct t_final_state *final_state; // 终态，不唯一
    struct t_state_trans *transfers;   // 所有的状态转移，即状态转移矩阵
};

struct DFA_definition myDFA = {0, myFinalStates, myTransfers};

// DFA为记号识别提供的接口

// 查询初态是哪个
int get_start_state() { return myDFA.start_state; }

// 判断指定状态是否为DFA的终态
// 若是，则返回该终态对应的记号类别，否则范围ERRTOKEN
enum Token_Type state_is_final(int state)
{
    struct t_final_state *p = myDFA.final_state;
    for (; p->state > 0; ++p)
    {
        if (p->state == state)
        {
            return p->kink;
        }
    }
    return ERRTOKEN;
}

// 根据当前状态，当前遇到的字符，进行状态转移
// 返回值：-1，此时没有状态转移；>=0，新的状态
int move(int state_src, char ch)
{
    int ck_of_ch = CK_CHAR;
    if ('0' <= ch && ch <= '9')
    {
        ck_of_ch = CK_DIGIT;
        ch = 0;
    }
    else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    {
        ck_of_ch = CK_LETTER;
        ch = 0;
    }
    else
    {
        ck_of_ch = CK_CHAR;
    }

    // 生成查询的关键字
    t_key key = MK_IDX(state_src, ck_of_ch | ch);

    // 查找转移
    struct t_state_trans *pTransfer = myDFA.transfers;
    for (; pTransfer->idx != TRANS_END; ++pTransfer)
    {
        if (pTransfer->idx == key)
        {
            return pTransfer->state_to;
        }
    }
    return -1;
}