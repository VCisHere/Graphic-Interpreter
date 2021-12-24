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
        "\n本次执行产生两个文件：\n"
        "%s 存储 语法分析器的输出文本\n"
        "%s 存储 输出的错误信息\n",
        LOGFILE_PATH, ERRFILE_PATH);
    Parser(argv[1]);
    return 0;
}