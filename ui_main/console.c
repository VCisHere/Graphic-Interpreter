#include <stdio.h>

extern void Parser(const char *file_name);

#include "../errlog/errlog.h"

int CheckSrcFile(const char *file_name)
{
    FILE *file = NULL;
    if (file_name == NULL || file_name[0] == '\0')
    {
        fprintf(stderr, "δָ��Դ�����ļ�");
        return 0;
    }
    if ((file = fopen(file_name, "r")) == NULL)
    {
        fprintf(stderr, "��Դ�����ļ�[%s]ʧ��", file_name);
        return 0;
    }
    else
    {
        fclose(file);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Give me a source file!\n");
        return 1;
    }
    if (CheckSrcFile(argv[1]) == 0)
    {
        return 1;
    }

    InitError();

    printf(
        "\n����ִ�в��������ļ���\n"
        "%s �洢 �﷨������������ı�\n"
        "%s �洢 ����Ĵ�����Ϣ\n",
        LOGFILE_PATH, ERRFILE_PATH);
    Parser(argv[1]);
    CloseError();

    return 0;
}

#include "ui.h"

void ShowMessage(int flag, const char *msg)
{
    if (flag == 0)
    {
        fprintf(stdout, msg);
    }
    else
    {
        fprintf(stderr, msg);
    }
}

void DrawPixel(unsigned long x, unsigned long y, unsigned int color)
{
    fprintf(stdout, "DrawPixel(%ld, %ld)\n", x, y);
}