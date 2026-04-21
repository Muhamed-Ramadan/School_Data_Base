#ifndef SCHOOL_H
#define SCHOOL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "types.h"
#include "config.h"

/* ═══════════════════════════════════════════════════════════════════
   FIELD SIZE CONSTANTS
   Minimum lengths are enforced by the input validators.
═══════════════════════════════════════════════════════════════════ */
#define NAME_LEN     30   /* visible max 29 chars | min: 2 words each >= 2  */
#define BIRTH_LEN    11   /* DD/MM/YYYY\0                                   */
#define ID_LEN       10   /* visible max  9 chars | min: 4                  */
#define ADDRESS_LEN  40   /* visible max 39 chars | min: 10                 */
#define PHONE_LEN    16   /* visible max 15 digits | min: 6                 */
#define INPUT_BUF    64   /* general-purpose read buffer                    */

/* ═══════════════════════════════════════════════════════════════════
   STUDENT  (linked-list node)
═══════════════════════════════════════════════════════════════════ */
typedef struct node {

    /* -- Personal information ------------------------------------ */
    char name    [NAME_LEN];     /* full name: letters, spaces, hyphens      */
    char address [ADDRESS_LEN];  /* street address                           */
    char birth   [BIRTH_LEN];    /* date of birth in DD/MM/YYYY format       */
    char phone   [PHONE_LEN];    /* guardian phone number (digits only)      */
    char id      [ID_LEN];       /* unique student identifier                */

    /* -- Academic scores (0-100) --------------------------------- */
    u8   comp_science;   /* Computer Science score                           */
    u8   science;        /* Science score                                    */
    u8   english;        /* English score                                    */
    u8   math;           /* Math score                                       */
    u8   history;        /* History score                                    */

    /* -- Derived grades ------------------------------------------ */
    /* Recomputed by compute_grades() after every score change.     */
    f32  total;                  /* arithmetic average of all five subjects  */
    u8   comp_science_grade;     /* letter grade: A / B / C / D / F         */
    u8   science_grade;          /* letter grade for Science                 */
    u8   english_grade;          /* letter grade for English                 */
    u8   math_grade;             /* letter grade for Math                    */
    u8   history_grade;          /* letter grade for History                 */
    u8   total_grade;            /* overall letter grade                     */

    struct node *next;           /* pointer to next student in the queue     */

} Student;

/* ═══════════════════════════════════════════════════════════════════
   SCHOOL  (queue over the student linked list)
═══════════════════════════════════════════════════════════════════ */
typedef struct queue {
    u16      count;   /* total number of students currently enrolled         */
    Student *front;   /* pointer to the first student (oldest entry)         */
    Student *rear;    /* pointer to the last student  (most recent entry)    */
} School;

/* ═══════════════════════════════════════════════════════════════════
   PUBLIC INTERFACE
═══════════════════════════════════════════════════════════════════ */

/* -- Core lifecycle -------------------------------------------- */
void  run_main_menu          (void);
void  init_school            (School *school);
bool  is_empty               (School *school);

/* -- Profile operations --------------------------------------- */
void  add_student            (School *school);
void  delete_student         (School *school, const char *id);

/* -- Display: profiles --------------------------------------- */
void  print_profile_card     (const Student *student);
void  print_all_profiles     (School *school);
void  list_profiles_sorted   (School *school);

/* -- Display: scores ----------------------------------------- */
void  print_score_card       (const Student *student);
void  print_all_scores       (School *school);
void  print_scores_ranked    (School *school);

/* -- Score editing ------------------------------------------- */
void  edit_all_scores        (School *school);

/* -- Silent search (no output) ------------------------------ */
bool  student_exists_by_id   (School *school, const char *id);
bool  student_exists_by_name (School *school, const char *name);

#endif /* SCHOOL_H */
