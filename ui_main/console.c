#include <stdio.h>

extern void Parser(const char *file_name);

#include "../errlog/errlog.h"

int CheckSrcFile(const char *file_name)
{
    FILE *file = NULL;
    if (file_name == NULL || file_name[0] == '\0')
    {
        fprintf(stderr, "未指定源程序文件");
        return 0;
    }
    if ((file = fopen(file_name, "r")) == NULL)
    {
        fprintf(stderr, "打开源程序文件[%s]失败", file_name);
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
        "\n本次执行产生两个文件：\n"
        "%s 存储 语法分析器的输出文本\n"
        "%s 存储 输出的错误信息\n",
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