#include "db.h"

int main(int argc, char *argv[]) {

    // 从启动命令行中读取需要的文件位置
    if (argc < 2) {
        printf("请输入数据库表的存储文件地址.\n");
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    // 打开数据库数据文件
    Table* table = db_open(filename);

    InputBuffer *input_buffer = new_input_buffer();

    while (true) {
        // 输出数据库系统标识
        print_prompt();
        // 读入用户输入命令
        read_input(input_buffer);

        // 首先进行元命令处理
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table)) {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        // 将命令抽象为Statement数据结构，发生任何错误继续从循环开始执行
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            // 预处理成功才进入excute_statement真正执行
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case PREPARE_STRING_TOO_LONG:
                printf("String is too long.\n");
                continue;
            case PREPARE_NEGATIVE_ID:
                printf("ID must be positive.\n");
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // 执行命令
        switch (execute_statement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Executed.\n");
                break;
            case EXECUTE_TABLE_FULL:
                printf("Error: Table full.\n");
                break;
        }
    }
}