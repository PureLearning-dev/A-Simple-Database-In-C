#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 250

// 硬编码数据库表
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

// 声明
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
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
    Row row_to_insert;
} Statement;

// 封装一个输入缓冲区
typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

// 执行结果enum封装
typedef enum {
    EXECUTE_TABLE_FULL,
    EXECUTE_SUCCESS
} ExecuteResult;

// 得到一些基础数据

// 得到id的大小
const uint32_t ID_SIZE = size_of_attribute(Row, id);
// 得到username的大小
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
// 得到email的大小
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
// 初始化各个变量的偏移量
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
// 得到PAGE相关的数据，一页中有多少行等等
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
// 这里设置的一页是4096B，一个表可以装100个PAGE
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

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

// 为了在硬盘上紧凑的存储和可以跨平台读取，需要进行序列化和反序列化
// 原理：结构体在经过编译器后，会在结构体的不同数据后添加padding，便于CPU处理。如果不将添加的padding处理了，会导致存储在硬盘上浪费很多空间。
// 同时，不同平台添加的padding也有所不同，直接存取会导致不同平台无法兼容使用。
// 因此，使用统一的操作进行处理，就可以避免上述的各种糟糕的情况！
void serialize_row(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

// 确定特定行在内存中的地址
void* row_slot(Table* table, uint32_t row_num) {
    // 确定在table中的哪个页
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    if (page == NULL) {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    // 确定在页中的哪个行
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    // 返回具体的内存地址
    return page + byte_offset;
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table* table) {
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

// 得到输入的命令具体是什么类型
MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table) {
    if (strcmp(input_buffer->buffer, ".quit") == 0) {
        close_input_buffer(input_buffer);
        free(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

// 对插入操作进行预处理，防止字符溢出
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
    statement->type = STATEMENT_INSERT;

    // 硬编码切割命令
    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);

    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

// 预处理输入的命令
// 现在只能识别insert和select
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        return prepare_insert(input_buffer, statement);
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);

    serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

// 获取所有的数据
ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}


// 处理具体语句，该函数是最核心的函数之一
// 后续会发展为一个虚拟机，用于执行输入的命令语句
ExecuteResult execute_statement(Statement* statement, Table *table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
        	return execute_select(statement, table);
    }
}

int main(int argc, char *argv[]) {
    Table* table = new_table();
    InputBuffer *input_buffer = new_input_buffer();
    // 不断循环读取、执行
    while (true) {
        // 输出标准格式
        print_prompt();
        // 读入输入的命令
        read_input(input_buffer);
        // 如果是元命令执行do_meta_command函数
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        // 否则执行对应操作
        Statement statement;

        // 预处理命令，得到命令对应的抽象结构体statement
        switch (prepare_statement(input_buffer, &statement)) {
            // 成功直接跳出则可，只有错误才进行单独处理
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement.\n");
            	continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is too long.\n");
                continue;
            case (PREPARE_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n",
                input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }
}
