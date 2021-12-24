#include <stdio.h>

#include "scanner.h"

const char *token_type_str(enum Token_Type tk);

int main(int argc, char *argv[])
{
    struct Token token;
    if (argc < 2)
    {
        printf("Please input source file!\n");
        return 1;
    }

    // ��ʼ���˷�������
    if (0 == InitScanner(argv[1]))
    {
        printf("Open source file error!\n");
        return 2;
    }

    printf("�Ǻ����     �ַ���       ����ֵ       ����ָ��     λ��\n");

    while (1)
    {
        // ��üǺ�
        token = GetToken();

        if (token.type != NONTOKEN)
        {
            printf("%-12s %-12s %-12f %-12p (%d,%d)\n", token_type_str(token.type),
                   token.lexeme, token.value, token.FuncPtr, token.where.line,
                   token.where.col);
        }
        else
        {
            break;
        }
    }
    CloseScanner();
    return 0;
}