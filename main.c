#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/oscardb.h"

const char* EXIT = "EXIT";

const char* INSERT_M = "INSERT-M";
const char* GET_M = "GET-M";
const char* UPDATE_M = "UPDATE-M";
const char* DELETE_M = "DELETE-M";
const char* SHOW_M = "SHOW-M";
const char* SHOW_M_REAL = "SHOW-M-REAL";

const char* INSERT_S = "INSERT-S";
const char* GET_S = "GET-S";
const char* UPDATE_S = "UPDATE-S";
const char* DELETE_S = "DELETE-S";
const char* SHOW_S = "SHOW-S";
const char* SHOW_S_REAL = "SHOW-S-REAL";

const char* CALL_GC = "CALL-GC";

char input[128];

void input_user_pk(int* id);
void input_user(struct User* u);
void input_comment_pk(int* user_id, int* comment_id, int handle_comment_id);
void input_user_comment(struct User_Comment* user_comment);
void get_input(char* arr, int size);

int main() {
    setbuf(stdout, NULL);
    db_stotage = "/e/Study/Labs/DB/OscarDB/resources/";

    //create_db("juniorhills");
    connect("juniorhills");
    create_table("users", MASTER);
    create_table("user_comments", SLAVE);

    while(1)
    {
        printf("\n\x1b[32mMaster: INSERT-M, GET-M, UPDATE-M, DELETE-M SHOW-M\n");
        printf("Slave: INSERT-S, GET-S, UPDATE-S, DELETE-S SHOW-S\x1b[0m\n\n");
        printf("Enter your command: ");

        gets(input);
        if(strcmp(input, INSERT_M) == 0)
        {
            struct User* u = malloc(sizeof(struct User));
            input_user(u);

            insert_m(u);
            continue;
        }
        if(strcmp(input, GET_M) == 0)
        {
            int id;
            input_user_pk(&id);

            if(id < 1)
            {
                printf("\x1b[31mIncorrect ID!\x1b[0m\n");
                continue;
            }

            struct User* u = get_m(id);

            if(u != NULL)printf("User(id= '%i', email = '%s', password = '%s')\n", u->id, u->email, u->pass);
            else printf("\x1b[31mUser with id = '%i' doesn't exist!\x1b[0m\n", id);
            continue;
        }
        if(strcmp(input, UPDATE_M) == 0)
        {
            struct User* u = malloc(sizeof(struct User));

            input_user_pk(&u->id);
            input_user(u);

            update_m(u);
            continue;
        }
        if(strcmp(input, DELETE_M) == 0)
        {
            int id;
            input_user_pk(&id);
            delete_m(id);
            continue;
        }
        if(strcmp(input, SHOW_M) == 0)
        {
            show_m(1);
            continue;
        }
        if(strcmp(input, SHOW_M_REAL) == 0)
        {
            show_m(0);
            continue;
        }
        if(strcmp(input, INSERT_S) == 0)
        {
            struct User_Comment* u_c = malloc(sizeof(struct User_Comment));
            input_comment_pk(&u_c->user_id, &u_c->comment_id, 0);
            input_user_comment(u_c);

            insert_s(u_c);
            continue;
        }
        if(strcmp(input, GET_S) == 0)
        {
            int user_id, comment_id;
            input_comment_pk(&user_id, &comment_id, 0);

            if(user_id < 1)
            {
                printf("\x1b[31mIncorrect user_id!\x1b[0m\n");
                continue;
            }
            if(comment_id < 1)
            {
                printf("\x1b[31mIncorrect comment_id!\x1b[0m\n");
                continue;
            }

            struct User_Comment* u_c = get_s(user_id, comment_id);

            if(u_c != NULL)printf("User's Comment(user_id= '%i', comment_id= '%i', text = '%s', date = '%s')\n",
                    u_c->user_id, u_c->comment_id, u_c->text, u_c->date);
            else printf("\x1b[31mUser's Comment with user_id= '%i', comment_id= '%i' doesn't exist!\x1b[0m\n", user_id, comment_id);
            continue;
        }
        if(strcmp(input, UPDATE_S) == 0)
        {
            struct User_Comment* u_c = malloc(sizeof(struct User_Comment));

            input_comment_pk(&u_c->user_id, &u_c->comment_id, 0);
            input_user_comment(u_c);

            update_s(u_c);
            continue;
        }
        if(strcmp(input, DELETE_S) == 0)
        {
            int user_id, comment_id;
            input_comment_pk(&user_id, &comment_id, 0);
            delete_s(user_id, comment_id);
            continue;
        }
        if(strcmp(input, SHOW_S) == 0)
        {
            show_s(1);
            continue;
        }
        if(strcmp(input, SHOW_S_REAL) == 0)
        {
            show_s(0);
            continue;
        }
        if(strcmp(input, CALL_GC) == 0)
        {
            call_gc();
            continue;
        }
        if(strcmp(input, EXIT) == 0) break;
        printf("Wrong Command!\n");
    }

    return 0;
}

void input_user_pk(int* id)
{
    printf("id(PK): ");
    scanf("%d", id);
    gets(input);
}
void input_user(struct User* u)
{
    printf("email: ");
    get_input(u->email, sizeof(u->email));

    printf("password: ");
    get_input(u->pass, sizeof(u->pass));
}
void input_comment_pk(int* user_id, int* comment_id, int handle_comment_id)
{
    printf("user_id(PK): ");
    scanf("%d", user_id);
    gets(input);

    if(handle_comment_id == 0)
    {
        printf("comment_id(PK): ");
        scanf("%d", comment_id);
        gets(input);
    }
}
void input_user_comment(struct User_Comment* u_c)
{
    printf("text: ");
    get_input(u_c->text, sizeof(u_c->text));

    printf("date: ");
    get_input(u_c->date, sizeof(u_c->date));

}

void get_input(char* arr, int size)
{
    gets(input);
    memcpy( arr, input, (size_t)(size - 1));
    arr[size - 1] = '\0';
    /*int ch;
    strcpy(arr, "");
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if(size > 0)
        {
            printf("%c\n", (char)ch);
            printf("%s\n", arr);
            arr[31 - size] = 'Q';
            size--;
        }
    }*/
}