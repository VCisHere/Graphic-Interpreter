#include <stdio.h>

#include "../errlog/errlog.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please input source file\n");
        return 1;
    }
    InitError();

    printf(
        "\n����ִ�в��������ļ���\n"
        "%s �洢 �﷨������������ı�\n"
        "%s �洢 ����Ĵ�����Ϣ\n",
        LOGFILE_PATH, ERRFILE_PATH);
    Parser(argv[1]);
    return 0;
}