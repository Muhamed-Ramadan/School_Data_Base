#ifndef Q_H
#define Q_H
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define NAME_LEN    30
#define BIRTH_LEN   15
#define ID_LEN      10
#define ADDRESS_LEN 30
#define PHONE_LEN   10


typedef struct node{
    char name[NAME_LEN];
    char address[ADDRESS_LEN];
    char birth[BIRTH_LEN];
    char phone[PHONE_LEN];
    char ID[ID_LEN];
    int  com_sc;
    int  ar_sc;
    int  eng_sc;
    int  math_sc;
    struct node *pNext;
}Student; // student is a node

typedef struct queue{ //Queue
int size;
Student* front;
Student* rear;
}School; //school is a queue using single linked list

void MAIN_MENU();                      // prints welcome screen + print MENU until exiting
void CreateSchool(School *pschool);// creating queue of single linked list and assigning size,front,rear to 0,NULL,NULL//
void NEW_STUDENT(School *pschool); //add new student next to the previous student
bool isEmpty(School *pschool); // check if the database is empty
void STUDENT_EDIT(School *pschool,char id[ID_LEN]); // edit specific student(using STUDENT_SEARCH_ID( ,ID) then edit the found student)
Student* STUDENT_SEARCH_ID(School *pschool,char id[ID_LEN]); // searching specific student with ID then PRINT_STUDENT() function
Student* STUDENT_SEARCH_NAME(School *pschool,char NAME[NAME_LEN]); // searching specific student with Name then PRINT_STUDENT() function
void DELETE_STUDENT(School *pschool,char id[]); // search and delete specific student
void STUDENT_LIST(School *pschool); // LISTING Students A-Z then PRINT_SCHOOL() function
void PRINT_STUDENT(Student *pstudent); // print single student info
void PRINT_SCHOOL(School *pschool); // print all the students info with the current arrangement(using PRINT_STUDENT() function multi time)
void STUDENT_SCORE(School *pschool);   // editing all students scores (ARABIC MATH ENGLISH COMPUTER SC.)
void RANK_STUDENT(School *pschool);        // Ranking the Database according to the Computer Sc. Score of Every Student
void PRINT_STUDENT_SCORE(Student *pstudent);// print single student name , id and scores (ARABIC MATH ENGLISH COMPUTER SC.)
void PRINT_SCHOOL_SCORE(School *pschool); // prints all students name id scores using PRINT_STUDENT_SCORE() function multi time


void SchoolSize(School *pschool); // # of Students
//void ClearSchool(School *pschool); //clear data base - could be used later -
bool STUDENT_SEARCH_ID_SILENT(School *pschool,char id[ID_LEN]); // exactly like STUDENT_SEARCH_ID() but with no printf
bool STUDENT_SEARCH_NAME_SILENT(School *pschool,char NAME[NAME_LEN]);// exactly like STUDENT_SEARCH_NAME() but with no printf
#endif // Q_H
