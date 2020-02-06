#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/oscardb.h"
#include "../headers/gc.h"

struct stat st = {0};

struct DataBase{
    struct Master* master;
    struct Slave* slave;
};

struct Record{};

struct T_Header{
    int records_count;
    int next_new_pk;
};

struct Master{
    char* ind;
    char* fl;
    size_t record_s;
};

struct Slave{
    char* ind;
    char* fl;
    size_t record_s;
};

struct Index{
    int pk;
    int ind;
    int exist;
};

struct S_Index{
    int m_pk;
    int s_pk;
    int ind;
    int exist;
};

struct DataBase* db;
char* db_path;

int create_file(char* name, char* type);

void create_db(char* db_name)
{
    db_path = str_cat(db_stotage, db_name);
    db_path = str_cat(db_path, "/");
    if (stat(db_path, &st) == -1)
    {
        mkdir(db_path);
        printf("Data base '%s' created successful!\n", db_name);
    }
    else
    {
        printf("Data base '%s' already exist!\n", db_name);
    }
}

void connect(char* db_name)
{
    db_path = str_cat(db_stotage, db_name);
    db_path = str_cat(db_path, "/");
    db = malloc(sizeof(struct DataBase));
    db->master = malloc(sizeof(struct Master));
    db->slave = malloc(sizeof(struct Slave));

    db->master->ind = str_cat("users", ".ind");
    db->master->fl = str_cat("users", ".fl");
    db->master->record_s = sizeof(struct User);

    db->slave->ind = str_cat("user_comments", ".ind");
    db->slave->fl = str_cat("user_comments", ".fl");
    db->slave->record_s = sizeof(struct User_Comment);
}

void create_table(char* t_name, int t_type)
{
    if(t_type == MASTER)
    {
        if(create_file(str_cat(db_path, t_name), ".ind") == 0 && create_file(str_cat(db_path, t_name), ".fl") == 0)
        {
            db->master->ind = str_cat(t_name, ".ind");
            db->master->fl = str_cat(t_name, ".fl");
            db->master->record_s = sizeof(struct User);

            FILE *index_table = fopen(str_cat(db_path, db->master->ind), "r+b");
            if(index_table == NULL)
            {
                printf("Cannot open file '%s'.\n", db->master->ind);
                return;
            }
            struct T_Header* header = malloc(sizeof(header));
            header->records_count = 0;
            header->next_new_pk = 1;
            fwrite(header, sizeof(struct Index), 1, index_table);
            fclose(index_table);

            printf("Table '%s' created successful\n", t_name);
        }
        else
        {
            printf("Cannot create Table '%s'\n", t_name);
        }
        return;
    }
    if(t_type == SLAVE)
    {
        if(create_file(str_cat(db_path, t_name), ".ind") == 0 && create_file(str_cat(db_path, t_name), ".fl") == 0)
        {
            db->slave->ind = str_cat(t_name, ".ind");
            db->slave->fl = str_cat(t_name, ".fl");
            db->slave->record_s = sizeof(struct User_Comment);

            FILE *index_table = fopen(str_cat(db_path, db->slave->ind), "r+b");
            if(index_table == NULL)
            {
                printf("Cannot open file '%s'.\n", db->slave->fl);
                return;
            }
            struct T_Header* header = malloc(sizeof(header));
            header->records_count = 0;
            fwrite(header, sizeof(struct Index), 1, index_table);
            fclose(index_table);
            printf("Table '%s' created successful\n", t_name);
        }
        else
        {
            printf("Cannot create Table '%s'\n", t_name);
        }
        return;
    }
}

//   <M A S T E R>

void insert_m(struct User* user)
{
    if(strlen(user->email) == 0)
    {
        printf("\x1b[31mEmail cannot be empty!\x1b[0m\n");
        return;
    }
    if(strlen(user->pass) == 0)
    {
        printf("\x1b[31mPassword cannot be empty!\x1b[0m\n");
        return;;
    }

    FILE *index_table = fopen(str_cat(db_path, db->master->ind), "r+b");
    if(index_table == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return;
    }
    struct T_Header* header = malloc(sizeof(header));
    fread(header, sizeof(struct Index), 1, index_table);
    user->id = header->next_new_pk;
    header->records_count = header->records_count + 1;
    header->next_new_pk = header->next_new_pk + 1;
    rewind(index_table);
    fwrite(header, sizeof(struct Index), 1, index_table);

    struct Index* index = malloc(sizeof(struct Index));
    index->pk = user->id;
    index->exist = 0;

    FILE *users = fopen(str_cat(db_path, db->master->fl), "ab");
    if(users == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->fl);
        return;
    }
    fseek(users, 0, SEEK_END);
    index->ind = ftell(users) / sizeof(struct User);
    rewind(users);

    fwrite(user, sizeof(struct User), 1, users);
    fclose(users);

    fseek(index_table, 0, SEEK_END);
    fwrite(index, sizeof(struct Index), 1, index_table);
    fclose(index_table);

    // дескриптор файла
    //handle = fileno(tf);

    // размер файла
    //fs = filelength(handle);

    // обрезаем последнюю запись
    //chsize(handle,fs-s);

    //printf("INSERT User(id = '%i', email = '%s', password = '%s') - Successful!\n", user->id, user->email, user->pass);
}

struct User* get_m(int m_pk)
{
    FILE *index_t = fopen(str_cat(db_path, db->master->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return NULL;
    }
    struct Index* index = malloc(sizeof(struct Index));
    for(;;)
    {
        fread(index, sizeof(struct Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->pk == m_pk && index->exist == 0)
        {
            struct User* user = malloc(sizeof(struct User));
            FILE *users = fopen(str_cat(db_path, db->master->fl), "rb");
            fseek(users, index->ind * sizeof(struct User), SEEK_SET);
            fread(user, sizeof(struct User), 1 , users);
            return user;
        }
    }
    //printf("User with id = '%i' doesn't exist!\n", m_pk);
    return NULL;
}

void update_m(struct User* user)
{
    if(user->id < 1)
    {
        printf("\x1b[31mIncorrect user_id!\x1b[0m\n");
        return;
    }
    if(strlen(user->email) == 0)
    {
        printf("\x1b[31mEmail cannot be empty!\x1b[0m\n");
        return;
    }
    if(strlen(user->pass) == 0)
    {
        printf("\x1b[31mPassword cannot be empty!\x1b[0m\n");
        return;
    }

    if(get_m(user->id) == NULL)
    {
        printf("User with id = '%i' doesn't exist!\n", user->id);
        return;
    }

    FILE *index_t = fopen(str_cat(db_path, db->master->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return;
    }
    struct Index* index = malloc(sizeof(struct Index));
    for(;;)
    {
        fread(index, sizeof(struct Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->pk == user->id && index->exist == 0)
        {
            FILE *users = fopen(str_cat(db_path, db->master->fl), "r+b");
            fseek(users, index->ind * sizeof(struct User), SEEK_SET);
            fwrite(user, sizeof(struct User), 1, users);
            fclose(users);
            printf("UPDATE User(id = '%i', email = '%s', password = '%s') - Successful!\n", user->id, user->email, user->pass);
            return;
        }
    }
    printf("User with id = '%i' doesn't exist!\n", user->id);
}

void delete_m(int m_pk)
{
    if(m_pk < 1)
    {
        printf("\x1b[31mIncorrect ID!\x1b[0m\n");
        return;
    }

    if(get_m(m_pk) == NULL)
    {
        printf("User with id = '%i' doesn't exist!\n", m_pk);
        return;
    }

    FILE *index_t = fopen(str_cat(db_path, db->master->ind), "r+b");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return;
    }

    struct T_Header* header = malloc(sizeof(header));
    fread(header, sizeof(struct Index), 1, index_t);
    header->records_count = header->records_count - 1;
    rewind(index_t);
    fwrite(header, sizeof(struct Index), 1, index_t);
    //skip header
    fseek(index_t, sizeof(struct Index), SEEK_SET);

    struct Index* index = malloc(sizeof(struct Index));
    for(int i = 1; i > 0; i++)
    {
        fread(index, sizeof(struct Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->pk == m_pk)
        {
            index->exist = 1;
            //skip header
            fseek(index_t, sizeof(struct Index) * i, SEEK_SET);
            fwrite(index, sizeof(struct Index), 1, index_t);

            //delete s-entities
            FILE* s_index_t = fopen(str_cat(db_path, db->slave->ind), "r+b");
            if(s_index_t == NULL)
            {
                printf("Cannot open file '%s'.\n", db->slave->ind);
                return;
            }

            struct T_Header* s_header = malloc(sizeof(struct T_Header));
            fread(s_header, sizeof(struct S_Index), 1, s_index_t);

            //skip header
            //fseek(s_index_t, sizeof(struct S_Index), SEEK_SET);

            struct S_Index* s_index = malloc(sizeof(struct S_Index));
            for(int j = 1; j > 0; j++)
            {
                fread(s_index, sizeof(struct S_Index), 1 , s_index_t);
                if(feof(s_index_t)) break;
                if(s_index->m_pk == m_pk)
                {
                    s_index->exist = 1;
                    fseek(s_index_t, sizeof(struct S_Index) * j, SEEK_SET);
                    fwrite(s_index, sizeof(struct S_Index), 1, s_index_t);
                    fseek(s_index_t, sizeof(struct S_Index) * (j + 1), SEEK_SET);
                    s_header->records_count = s_header->records_count - 1;
                }
            }
            rewind(s_index_t);
            fwrite(s_header, sizeof(struct S_Index), 1, s_index_t);
            fclose(s_index_t);
            printf("DELETE User(id = '%i') - Successful!\n", m_pk);
            break;
        }
    }
    fclose(index_t);
}

void show_m(int real)
{
    FILE *index_t = fopen(str_cat(db_path, db->master->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return;
    }

    FILE *users = fopen(str_cat(db_path, db->master->fl), "rb");
    if(users == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->fl);
        return;
    }

    struct Index* index = malloc(sizeof(struct Index));
    struct User* user = malloc(sizeof(struct User));

    printf("\n%4s + %7s + %7s + %7s", "", "- - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %31s + %21s +\n", "", "- - - -", "- - - - - - - - - - - - - - - -", "- - - - - - - - - - -");

    printf("%4s | %7s | %7s | %7s", "", "user_id", "index", real == 0 ? "exist |" : "");
    printf("%15s | %7s | %31s | %21s |\n", "", "id", "email", "password");

    printf("%4s + %7s + %7s + %7s", " + -", "- - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %31s + %21s +\n", " + -", "- - - -", "- - - - - - - - - - - - - - - -", "- - - - - - - - - - -");

    //skip header
    fseek(index_t, sizeof(struct Index), SEEK_SET);
    for(int i = 1; i > 0; i++)
    {
        fread(index, sizeof(struct Index), 1 , index_t);
        if(feof(index_t)) break;
        if(real == 0 || index->exist == 0)
        {
            printf("%4i | %7i | %7i | %7s", i, index->pk, index->ind, real == 0 ? (index->exist == 0 ? "+   |" : "-   |") : "");

            fseek(users, index->ind * sizeof(struct User), SEEK_SET);
            fread(user, sizeof(struct User), 1, users);
            printf("%15i | %7i | %31s | %21s |\n", i, user->id, user->email, user->pass);
        }
        else i--;

    }
    printf("%4s + %7s + %7s + %7s", " + -", "- - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %31s + %21s +\n\n", " + -", "- - - -", "- - - - - - - - - - - - - - - -", "- - - - - - - - - - -");

    fclose(index_t);
    fclose(users);
}

//  <M A S T E R/>

//   <S L A V E>

void insert_s(struct User_Comment* user_comment)
{
    if(strlen(user_comment->text) == 0)
    {
        printf("\x1b[31mText cannot be empty!\x1b[0m\n");
        return;
    }
    if(strlen(user_comment->date) == 0)
    {
        printf("\x1b[31mDate cannot be empty!\x1b[0m\n");
        return;
    }

    if(get_m(user_comment->user_id) == NULL)
    {
        printf("User with id = '%i' doesn't exist!\n", user_comment->user_id);
        return;
    }
    if(get_s(user_comment->user_id, user_comment->comment_id) != NULL)
    {
        printf("User's Comment with user.id = '%i' and comment.id = '%i' already exist!\n", user_comment->user_id, user_comment->comment_id);
        return;
    }

    FILE *index_table = fopen(str_cat(db_path, db->slave->ind), "r+b");
    if(index_table == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return;
    }
    struct T_Header* header = malloc(sizeof(struct T_Header));
    fread(header, sizeof(struct S_Index), 1, index_table);
    header->records_count = header->records_count + 1;
    header->next_new_pk = header->next_new_pk + 1;
    rewind(index_table);
    fwrite(header, sizeof(struct S_Index), 1, index_table);

    struct S_Index* index = malloc(sizeof(struct S_Index));
    index->m_pk = user_comment->user_id;
    index->s_pk = user_comment->comment_id;
    index->exist = 0;

    FILE *user_comments = fopen(str_cat(db_path, db->slave->fl), "ab");
    if(user_comments == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->fl);
        return;
    }
    fseek(user_comments, 0, SEEK_END);
    index->ind = ftell(user_comments) / sizeof(struct User_Comment);

    fwrite(user_comment, sizeof(struct User_Comment), 1, user_comments);
    fclose(user_comments);

    fseek(index_table, 0, SEEK_END);
    fwrite(index, sizeof(struct S_Index), 1, index_table);
    fclose(index_table);
    printf("INSERT User's Comment(user_id = '%i', comment_id = '%i', text = '%s', date = '%s') - Successful!\n",
            user_comment->user_id, user_comment->comment_id, user_comment->text, user_comment->date);
}

struct User_Comment* get_s(int m_pk, int s_pk)
{
    if(m_pk < 1)
    {
        printf("\x1b[31mIncorrect user_id!\x1b[0m\n");
        return NULL;
    }
    if(s_pk < 1)
    {
        printf("\x1b[31mIncorrect commend_id!\x1b[0m\n");
        return NULL;
    }
    FILE *index_t = fopen(str_cat(db_path, db->slave->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return NULL;
    }
    struct S_Index* index = malloc(sizeof(struct S_Index));
    for(;;)
    {
        fread(index, sizeof(struct S_Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->m_pk == m_pk && index->s_pk == s_pk && index->exist == 0)
        {
            struct User_Comment* user_comment = malloc(sizeof(struct User_Comment));
            FILE *user_comments = fopen(str_cat(db_path, db->slave->fl), "rb");
            fseek(user_comments, index->ind * sizeof(struct User_Comment), SEEK_SET);
            fread(user_comment, sizeof(struct User_Comment), 1 , user_comments);
            return user_comment;
        }
    }
    //printf("User's Comment with user.id = '%i' and comment.id = '%i' doesn't exist!\n", m_pk, s_pk);
    return NULL;
}

void update_s(struct User_Comment* user_comment)
{
    if(user_comment->user_id < 1)
    {
        printf("\x1b[31mIncorrect user_id!\x1b[0m\n");
        return;
    }
    if(user_comment->user_id < 1)
    {
        printf("\x1b[31mIncorrect commend_id!\x1b[0m\n");
        return;
    }
    if(strlen(user_comment->text) == 0)
    {
        printf("\x1b[31mText cannot be empty!\x1b[0m\n");
        return;
    }
    if(strlen(user_comment->date) == 0)
    {
        printf("\x1b[31mDate cannot be empty!\x1b[0m\n");
        return;
    }
    if(get_s(user_comment->user_id, user_comment->comment_id) == NULL)
    {
        printf("User's Comment with user.id = '%i' and comment.id = '%i' doesn't exist!\n", user_comment->user_id, user_comment->comment_id);
        return;
    }

    FILE *index_t = fopen(str_cat(db_path, db->slave->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return;
    }
    struct S_Index* index = malloc(sizeof(struct S_Index));
    for(;;)
    {
        fread(index, sizeof(struct S_Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->m_pk == user_comment->user_id && index->s_pk == user_comment->comment_id && index->exist == 0)
        {
            FILE *user_comments = fopen(str_cat(db_path, db->slave->fl), "r+b");
            fseek(user_comments, index->ind * sizeof(struct User_Comment), SEEK_SET);
            fwrite(user_comment, sizeof(struct User_Comment), 1, user_comments);
            fclose(user_comments);

            printf("UPDATE User's Comment(user_id = '%i', comment_id = '%i', text = '%s', date = '%s') - Successful!\n",
                   user_comment->user_id, user_comment->comment_id, user_comment->text, user_comment->date);
            return;
        }
    }
    printf("User's Comment with user.id = '%i' and comment.id = '%i' doesn't exist!\n", user_comment->user_id, user_comment->comment_id);
}

void delete_s(int m_pk, int s_pk)
{
    if(m_pk < 1)
    {
        printf("\x1b[31mIncorrect user_id!\x1b[0m\n");
        return;
    }
    if(s_pk < 1)
    {
        printf("\x1b[31mIncorrect commend_id!\x1b[0m\n");
        return;
    }

    if(get_s(m_pk, s_pk) == NULL)
    {
        printf("User's Comment with user.id = '%i' and comment.id = '%i' doesn't exist!\n", m_pk, s_pk);
        return;
    }

    FILE *index_t = fopen(str_cat(db_path, db->slave->ind), "r+b");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return;
    }

    struct T_Header* header = malloc(sizeof(struct S_Index));
    fread(header, sizeof(struct S_Index), 1, index_t);
    header->records_count = header->records_count - 1;
    rewind(index_t);
    fwrite(header, sizeof(struct S_Index), 1, index_t);
    //skip header
    fseek(index_t, sizeof(struct S_Index), SEEK_SET);

    struct S_Index* index = malloc(sizeof(struct S_Index));
    for(;;)
    {
        fread(index, sizeof(struct S_Index), 1 , index_t);
        if(feof(index_t)) break;
        if(index->m_pk == m_pk && index->s_pk == s_pk)
        {
            index->exist = 1;
            //skip header
            fseek(index_t, -sizeof(struct S_Index), SEEK_CUR);
            fwrite(index, sizeof(struct S_Index), 1, index_t);

            printf("DELETE User's Comment(user_id = '%i', comment_id = '%i') - Successful!\n", m_pk, s_pk);
            break;
        }
    }
    fclose(index_t);
}

void show_s(int real)
{
    FILE *index_t = fopen(str_cat(db_path, db->slave->ind), "rb");
    if(index_t == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return;
    }

    FILE *user_comments = fopen(str_cat(db_path, db->slave->fl), "rb");
    if(user_comments == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->fl);
        return;
    }

    struct S_Index* index = malloc(sizeof(struct S_Index));
    struct User_Comment* user_comment = malloc(sizeof(struct User_Comment));

    printf("\n%4s + %7s + %11s + %7s + %7s", "", "- - - -", "- - - - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %11s + %7s + %7s +\n", "", "- - - -", "- - - - - -", "- - - -", "- - - -");

    printf("%4s | %7s | %11s | %7s | %7s", "", "user_id", "comment_id", "index", real == 0 ? "exist |" : "");
    printf("%15s | %7s | %11s | %7s | %7s |\n", "", "user_id", "comment_id", "text", "date");

    printf("%4s + %7s + %11s + %7s + %7s", " + -", "- - - -", "- - - - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %11s + %7s + %7s +\n", " + -", "- - - -", "- - - - - -", "- - - -", "- - - -");

    //skip header
    fseek(index_t, sizeof(struct S_Index), SEEK_SET);
    for(int i = 1; i > 0; i++)
    {
        fread(index, sizeof(struct S_Index), 1 , index_t);
        if(feof(index_t)) break;
        if(real == 0 || index->exist == 0)
        {
            printf("%4i | %7i | %11i | %7i | %7s", i, index->m_pk, index->s_pk, index->ind, real == 0 ? (index->exist == 0 ? "+   |" : "-   |") : "");

            fseek(user_comments, index->ind * sizeof(struct User_Comment), SEEK_SET);
            fread(user_comment, sizeof(struct User_Comment), 1, user_comments);
            printf("%15i | %7i | %11i | %7s | %7s |\n", i, user_comment->user_id, user_comment->comment_id, user_comment->text, user_comment->date);
        }
        else i--;

    }
    printf("%4s + %7s + %11s + %7s + %7s", " + -", "- - - -", "- - - - - -", "- - - -", real == 0 ? "- - - +" : "");
    printf("%15s + %7s + %11s + %7s + %7s +\n\n", " + -", "- - - -", "- - - - - -", "- - - -", "- - - -");

    fclose(index_t);
    fclose(user_comments);
}

//  <S L A V E/>

void clean_master()
{
    FILE *index_table = fopen(str_cat(db_path, db->master->ind), "r+b");
    if(index_table == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->ind);
        return;
    }

    struct T_Header* header = malloc(sizeof(struct Index));
    fread(header, sizeof(struct Index), 1, index_table);

    struct Index* index = malloc(sizeof(struct Index));
    FILE *users = fopen(str_cat(db_path, db->master->fl), "r+b");
    if(users == NULL)
    {
        printf("Cannot open file '%s'.\n", db->master->fl);
        return;
    }
    struct User* user = malloc(sizeof(struct User));

    int read_index = 1, rewrite_index = -1;
    for(read_index; read_index > 0; read_index++)
    {
        fread(index, sizeof(struct Index), 1, index_table);
        if(feof(index_table)) break;
        if(index->exist != 0)
        {
            if(rewrite_index == -1)rewrite_index = read_index;
        }
        else{
            if(rewrite_index != -1)
            {
                fseek(users, index->ind * sizeof(struct User), SEEK_SET);
                fread(user, sizeof(struct User), 1, users);
                printf("%i\n", user->id);
                fseek(users, (rewrite_index - 1) * sizeof(struct User), SEEK_SET);
                fwrite(user, sizeof(struct User), 1, users);

                index->ind = rewrite_index - 1;
                fseek(index_table, rewrite_index * sizeof(struct Index), SEEK_SET);
                fwrite(index, sizeof(struct Index), 1, index_table);
                fseek(index_table, (read_index + 1) * sizeof(struct Index), SEEK_SET);

                rewrite_index++;
            }
        }
    }
    fseek(index_table, 0, SEEK_SET);
    fwrite(header, sizeof(struct Index), 1, index_table);

    int handle = fileno(index_table);
    chsize(handle,(header->records_count + 1) * sizeof(struct Index));

    handle = fileno(users);
    chsize(handle,(header->records_count) * sizeof(struct User));

    fclose(index_table);
    fclose(users);
}

void clean_slave()
{
    FILE *index_table = fopen(str_cat(db_path, db->slave->ind), "r+b");
    if(index_table == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->ind);
        return;
    }

    struct T_Header* header = malloc(sizeof(struct S_Index));
    fread(header, sizeof(struct S_Index), 1, index_table);

    struct S_Index* index = malloc(sizeof(struct S_Index));
    FILE *user_comments = fopen(str_cat(db_path, db->slave->fl), "r+b");
    if(user_comments == NULL)
    {
        printf("Cannot open file '%s'.\n", db->slave->fl);
        return;
    }
    struct User_Comment* user_comment = malloc(sizeof(struct User_Comment));

    int read_index = 1, rewrite_index = -1;
    for(read_index; read_index > 0; read_index++)
    {
        fread(index, sizeof(struct S_Index), 1, index_table);
        if(feof(index_table)) break;
        if(index->exist != 0)
        {
            if(rewrite_index == -1)rewrite_index = read_index;
        }
        else{
            if(rewrite_index != -1)
            {
                fseek(user_comments, index->ind * sizeof(struct User_Comment), SEEK_SET);
                fread(user_comment, sizeof(struct User_Comment), 1, user_comments);
                fseek(user_comments, (rewrite_index - 1) * sizeof(struct User_Comment), SEEK_SET);
                fwrite(user_comment, sizeof(struct User_Comment), 1, user_comments);

                index->ind = rewrite_index - 1;
                fseek(index_table, rewrite_index * sizeof(struct S_Index), SEEK_SET);
                fwrite(index, sizeof(struct S_Index), 1, index_table);
                fseek(index_table, (read_index + 1) * sizeof(struct S_Index), SEEK_SET);

                rewrite_index++;
            }
        }
    }
    fseek(index_table, 0, SEEK_SET);
    fwrite(header, sizeof(struct S_Index), 1, index_table);

    int handle = fileno(index_table);
    chsize(handle,(header->records_count + 1) * sizeof(struct S_Index));

    handle = fileno(user_comments);
    chsize(handle,(header->records_count) * sizeof(struct User_Comment));

    fclose(index_table);
    fclose(user_comments);
}

void call_gc()
{
    clean_slave();
    clean_master();
}

char* str_cat(char* str1, char* str2)
{
    char* newStr = malloc((strlen(str1) + strlen(str2)) * sizeof(char));
    strcpy(newStr, str1);
    strcat(newStr, str2);

    return newStr;
}

int create_file(char* name, char* type)
{
    FILE* file = fopen(str_cat(name, type), "w");
    if(file == NULL)
    {
        return 1;
    }
    fclose(file);
    return 0;
}