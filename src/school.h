#ifndef SCHOOL_H
#define SCHOOL_H

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "config.h"

/* ── Field storage sizes (include null terminator) ──────────────
   Minimum lengths are enforced by the input validators.          */
#define NAME_LEN     30   /* visible max 29 chars; min: 2 words, each ≥ 2  */
#define BIRTH_LEN    11   /* DD/MM/YYYY\0                                   */
#define ID_LEN       10   /* visible max  9 chars; min: 4                   */
#define ADDRESS_LEN  40   /* visible max 39 chars; min: 10                  */
#define PHONE_LEN    16   /* visible max 15 digits; min:  6                 */
#define INPUT_BUF    64   /* general-purpose read buffer                    */

/* ── Student node ───────────────────────────────────────────────── */
typedef struct node {
    char  name    [NAME_LEN];
    char  address [ADDRESS_LEN];
    char  birth   [BIRTH_LEN];
    char  phone   [PHONE_LEN];
    char  ID      [ID_LEN];

    /* Scores  0 – 100 */
    int   com_sc;    /* Computer Science */
    int   sci_sc;    /* Science          */
    int   eng_sc;    /* English          */
    int   math_sc;   /* Math             */
    int   hist_sc;   /* History          */

    /* Derived — recomputed by update_gpa() after every score change */
    float total_sc;   /* arithmetic average of all five subjects (0.0–100.0) */
    char  com_gpa;    /* letter grade — Computer Science  (A/B/C/D/F)        */
    char  sci_gpa;    /* letter grade — Science                               */
    char  eng_gpa;    /* letter grade — English                               */
    char  math_gpa;   /* letter grade — Math                                  */
    char  hist_gpa;   /* letter grade — History                               */
    char  total_gpa;  /* overall letter grade                                 */

    struct node *pNext;
} Student;

/* ── School queue ───────────────────────────────────────────────── */
typedef struct queue {
    int      size;
    Student *front;
    Student *rear;
} School;

/* ── Public interface ───────────────────────────────────────────── */
void     MAIN_MENU                   (void);
void     CreateSchool                (School *pschool);
void     NEW_STUDENT                 (School *pschool);
bool     isEmpty                     (School *pschool);
void     DELETE_STUDENT              (School *pschool, char id[]);
void     PRINT_STUDENT               (Student *pstudent);
void     PRINT_SCHOOL                (School *pschool);
void     STUDENT_LIST                (School *pschool);
void     STUDENT_SCORE               (School *pschool);
void     RANK_STUDENT                (School *pschool);
void     PRINT_SCHOOL_SCORE          (School *pschool);
bool     STUDENT_SEARCH_ID_SILENT    (School *pschool, char id[ID_LEN]);
bool     STUDENT_SEARCH_NAME_SILENT  (School *pschool, char name[NAME_LEN]);

#endif /* SCHOOL_H */
