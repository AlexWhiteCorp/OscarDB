#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/oscardb.h"

const char* EXIT = "exit";
const char* INSERT_M = "INSERT-M";
const char* GET_M = "GET-M";
const char* UPDATE_M = "UPDATE-M";
const char* DELETE_M = "DELETE-M";
const char* SHOW_M = "SHOW-M";
char input[128];

void input_user_id(int* id);
void input_user(struct User* u);

int main() {
    setbuf(stdout, NULL);
    db_stotage = "../resources/";

    create_db("juniorhills");
    connect("juniorhills");
    create_table("users", MASTER);
    create_table("user_comments", SLAVE);

    while(1)
    {
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
            input_user_id(&id);

            struct User* u = get_m(id);

            if(u != NULL)printf("User(id= '%i', email = '%s', password = '%s')\n", u->id, u->email, u->pass);
            else printf("User with id = '%i' doesn't exist!\n", id);
            continue;
        }
        if(strcmp(input, UPDATE_M) == 0)
        {
            struct User* u = malloc(sizeof(struct User));

            input_user_id(&u->id);
            input_user(u);

            update_m(u);
            continue;
        }
        if(strcmp(input, DELETE_M) == 0)
        {
            int id;
            input_user_id(&id);
            delete_m(id);
            continue;
        }
        if(strcmp(input, SHOW_M) == 0)
        {
            show_m(0);
            continue;
        }
        if(strcmp(input, EXIT) == 0) break;
    }

    return 0;
}

void input_user_id(int* id)
{
    printf("id: ");
    scanf("%d", id);
    gets(input);
}
void input_user(struct User* u)
{
    printf("email: ");
    gets(input);
    strcpy(u->email, input);

    printf("password: ");
    gets(input);
    strcpy(u->pass, input);
}