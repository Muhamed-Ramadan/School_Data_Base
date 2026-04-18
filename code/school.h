#ifndef SCHOOL_H
#define SCHOOL_H

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "config.h"

/* ── Field storage sizes ──────────────────────────────── */
#define NAME_LEN     30   /* max 29 visible chars */
#define BIRTH_LEN    11   /* DD/MM/YYYY + null     */
#define ID_LEN       10   /* max  9 visible chars  */
#define ADDRESS_LEN  40   /* max 39 visible chars  */
#define PHONE_LEN    15   /* +XXXXXXXXXXXX + null  */

/* ── Read buffer (used locally in input functions) ────── */
#define INPUT_BUF    64

/* ── Student card box (inner width = 48 chars) ────────── */
#define CARD_BORDER  "  +------------------------------------------------+"
#define CARD_LBL_W   14
#define CARD_VAL_W   29

/* ── Data structures ──────────────────────────────────── */
typedef struct node {
    char name   [NAME_LEN];
    char address[ADDRESS_LEN];
    char birth  [BIRTH_LEN];
    char phone  [PHONE_LEN];
    char ID     [ID_LEN];
    int  com_sc;
    int  ar_sc;
    int  eng_sc;
    int  math_sc;
    struct node *pNext;
} Student;

typedef struct queue {
    int      size;
    Student *front;
    Student *rear;
} School;

/* ── Public interface ─────────────────────────────────── */
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

#endif // SCHOOL_H
