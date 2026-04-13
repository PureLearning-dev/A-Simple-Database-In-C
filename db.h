#ifndef DB_H
#define DB_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 250

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

// 将数据结构进一步抽象，Cursor表示数据库正在操作的行
// 只用知道数据所在的行就可以得到对应的内存位置
typedef struct {
    Table* table;
    uint32_t row_num;
    bool end_of_table;  // 表示Cursor指向位置的后一个行是否存在，方便直接使用Cursor进行判断是否可以执行后续操作
} Cursor;

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

// 在Table中抽象出一个pager，以后都向pager中查询数据
typedef struct {
    uint32_t num_rows;
    Pager* pager;
} Table;

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

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum {
    EXECUTE_TABLE_FULL,
    EXECUTE_SUCCESS
} ExecuteResult;

// 常量声明
extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t PAGE_SIZE;
extern const uint32_t ROW_SIZE;
extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

// 函数声明
InputBuffer *new_input_buffer();
void print_prompt();
void read_input(InputBuffer *input_buffer);
void close_input_buffer(InputBuffer *input_buffer);

void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* cursor_value(Cursor* cursor);
void print_row(Row* row);

Table* db_open(const char* filename);
void* get_page(Pager* pager, uint32_t page_num);
Pager* pager_open(const char* filename);
void db_close(Table* table);
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);

Cursor* table_start(Table* table);
Cursor* table_end(Table* table)
void cursor_advance(Cursor* cursor);

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);
ExecuteResult execute_statement(Statement* statement, Table *table);

#endif