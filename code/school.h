#ifndef SCHOOL_H
#define SCHOOL_H

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "config.h"

/* ── Field storage sizes (includes null terminator) ───── */
#define NAME_LEN      30   /* visible max: 29 chars */
#define BIRTH_LEN     11   /* DD/MM/YYYY + null      */
#define ID_LEN        10   /* visible max:  9 chars  */
#define ADDRESS_LEN   40   /* visible max: 39 chars  */
#define PHONE_LEN     16   /* visible max: 15 digits */
#define INPUT_BUF     64   /* general-purpose read buffer */

/* ── Student node ───────────────────────────────────────── */
typedef struct node {
    char  name    [NAME_LEN];
    char  address [ADDRESS_LEN];
    char  birth   [BIRTH_LEN];
    char  phone   [PHONE_LEN];
    char  ID      [ID_LEN];

    /* Scores (0 – 100) */
    int   com_sc;    /* Computer Science */
    int   sci_sc;    /* Science          */
    int   eng_sc;    /* English          */
    int   math_sc;   /* Math             */
    int   hist_sc;   /* History          */

    /* Derived: computed and stored after every score update */
    float total_sc;  /* average of all five subjects       */
    char  com_gpa;   /* letter grade for Computer Science  */
    char  sci_gpa;   /* letter grade for Science           */
    char  eng_gpa;   /* letter grade for English           */
    char  math_gpa;  /* letter grade for Math              */
    char  hist_gpa;  /* letter grade for History           */
    char  total_gpa; /* overall letter grade               */

    struct node *pNext;
} Student;

/* ── School queue ───────────────────────────────────────── */
typedef struct queue {
    int      size;
    Student *front;
    Student *rear;
} School;

/* ── Public interface ───────────────────────────────────── */
void     MAIN_MENU                  (void);
void     CreateSchool               (School *pschool);
void     NEW_STUDENT                (School *pschool);
bool     isEmpty                    (School *pschool);
void     STUDENT_EDIT               (School *pschool, char id[ID_LEN]);
Student *STUDENT_SEARCH_ID          (School *pschool, char id[ID_LEN]);
Student *STUDENT_SEARCH_NAME        (School *pschool, char name[NAME_LEN]);
void     DELETE_STUDENT             (School *pschool, char id[]);
void     STUDENT_LIST               (School *pschool);
void     PRINT_STUDENT              (Student *pstudent);
void     PRINT_SCHOOL               (School *pschool);
void     STUDENT_SCORE              (School *pschool);
void     RANK_STUDENT               (School *pschool);
void     PRINT_STUDENT_SCORE        (Student *pstudent);
void     PRINT_SCHOOL_SCORE         (School *pschool);
void     SchoolSize                 (School *pschool);
bool     STUDENT_SEARCH_ID_SILENT   (School *pschool, char id[ID_LEN]);
bool     STUDENT_SEARCH_NAME_SILENT (School *pschool, char name[NAME_LEN]);

#endif /* SCHOOL_H */
