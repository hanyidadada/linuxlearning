#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct stuInfo{
    char stuName[10];
    int Age;
}ElemType;
typedef struct node{
    ElemType data;
    struct node *next;
}ListNode;

ListNode *ListHead;

void CreateList(void)
{
    ListNode *p;
    if (ListHead) {
        free(ListHead);
    }
    ListHead = NULL;
    ListHead = (ListNode *)malloc(sizeof(ListNode));
    memset(ListHead, 0, sizeof(ListNode));
    printf("Create success\n");
    return;
}

void PrintfList(void) {
    if (ListHead == NULL) {
        printf("Error! please create first!\n");
        return;
    }
    ListNode *p = ListHead->next;
    while(p) {
        printf("student name:%s, age:%d\n", p->data.stuName, p->data.Age);
        p = p->next;
    }
    return;
}

void InsertList(void){
    if (ListHead == NULL) {
        printf("Error! please create first!\n");
        return;
    }
    
    ListNode *p = ListHead;
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    printf("Enter stu name:");
    scanf(" %s", temp->data.stuName);
    printf("Enter age:");
    scanf(" %d", &(temp->data.Age));
    while(p->next != NULL) {
        p = p->next;
    }
    p->next = temp;
    temp->next = NULL;
    return ;
}

int main(void) {
    while(1) {
        printf("1 Create List\n");
        printf("2 Printf List\n");
        printf("3 Insert List\n");
        printf("4 Quit\n\n");
        char command;
        scanf(" %c", &command);
        switch (command) {
        case '1':
            CreateList();
            break;
        case '2':
            PrintfList();
            break;
        case '3':
            InsertList();
            break;
        case '4':
            return 0;
            break;
        default:
            break;
        }
    }
}