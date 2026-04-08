#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 声明
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// 定义可以执行的语句操作
typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

// 将语句操作抽象成更高的语句结构体
typedef struct {
    StatementType type;
} Statement;

// 封装一个输入缓冲区
typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

// 创建一个InputBuffer结构体
InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db ~> ");
}

void read_input(InputBuffer *input_buffer) {
    if (input_buffer == NULL) {
        return;
    }
    // ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    // getline可以自动扩容，适合一行不确定具体输入长度的情况
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

// 得到输入的命令具体是什么类型
MetaCommandResult do_meta_command(InputBuffer *input_buffer) {
    if (strcmp(input_buffer->buffer, ".quit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

// 预处理输入的命令
// 现在只能识别insert和select
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

// 处理具体语句，该函数是最核心的函数之一
// 后续会发展为一个虚拟机，用于执行输入的命令语句
void execute_statement(Statement *statement) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            printf("This is where we would do an insert.\n");
            break;
        case (STATEMENT_SELECT):
            printf("This is where we would do a select.\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    InputBuffer *input_buffer = new_input_buffer();
    // 不断循环读取、执行
    while (true) {
        // 输出标准格式
        print_prompt();
        // 读入输入的命令
        read_input(input_buffer);
        // 如果是元命令执行do_meta_command函数
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                // 预留其他命令的执行
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n",
                input_buffer->buffer);
                continue;
        }

        execute_statement(&statement);
        printf("Executed.\n");
    }
}
