#ifndef OSCARDB_OSCARDB_H
#define OSCARDB_OSCARDB_H

#endif //OSCARDB_OSCARDB_H

#include <sys/stat.h>

static char* db_stotage = "";
static const int MASTER = 1;
static const int SLAVE = 2;

struct User{
    int id;
    char email[31];
    char pass[21];
};

struct User_Comment{
    int user_id;
    int comment_id;
    char text[7];
    char date[7];
};

void create_db(char* db_name);
void connect(char* db_name);
void show_m(int real), show_s(int real);

void create_table(char* t_name, int t_type);
void insert_m(struct User* user), insert_s(struct User_Comment* user_comment);
struct User* get_m(int m_pk); struct User_Comment* get_s(int m_pk, int s_pk);
void update_m(struct User* user), update_s(struct User_Comment* user_comment);
void delete_m(int m_pk), delete_s(int m_pk, int s_pk);
int count_m_total(), count_s_total(), count_s(int user_id);

void call_gc();

char* str_cat(char* str1, char* str2);
