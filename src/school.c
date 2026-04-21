#include "school.h"
#include "validation.h"
#include "input.h"
#include "display.h"

/* ═══════════════════════════════════════════════════════════════════
   TESTING MODE
   ─────────────────────────────────────────────────────────────────
   Compile with -DTESTING to replace getche() with a stdin-compatible
   alternative. This allows automated testing via input redirection:
     gcc src/main.c src/school.c src/validation.c src/input.c \
         src/display.c -DTESTING -o tests/test_build.exe
     test_build.exe < tests/input/test_01.txt > tests/output/test_01.txt
   In normal mode READ_CHAR() calls getche() as usual.
═══════════════════════════════════════════════════════════════════ */
#ifdef TESTING
static char testing_read_char(void)
{
    i32 c = getchar();                      /* read one character from stdin */
    i32 n = getchar();                      /* consume the following newline */
    if (n != '\n' && n != EOF) ungetc(n, stdin);
    if (c == EOF) return 'D';               /* treat EOF as Exit             */
    return (char)c;
}
#define READ_CHAR() testing_read_char()
#else
#include <conio.h>
#define READ_CHAR() getche()
#endif

/* ═══════════════════════════════════════════════════════════════════
   GRADE HELPERS
═══════════════════════════════════════════════════════════════════ */

/*
 * American letter-grade system: A>=90  B>=80  C>=70  D>=60  F<60
 * score  : u8 — valid scores are 0-100, fits in u8 (0-255).
 * return : u8 — the ASCII code of A/B/C/D/F, fits in u8.
 */
static u8 letter_grade(u8 score)
{
    if (score >= 90) return 'A';
    if (score >= 80) return 'B';
    if (score >= 70) return 'C';
    if (score >= 60) return 'D';
    return 'F';
}

/* Recompute and store all derived grade fields.
   Must be called after every score change. */
static void compute_grades(Student *student)
{
    student->comp_science_grade = letter_grade(student->comp_science);
    student->science_grade      = letter_grade(student->science);
    student->english_grade      = letter_grade(student->english);
    student->math_grade         = letter_grade(student->math);
    student->history_grade      = letter_grade(student->history);

    student->total = (student->comp_science + student->science +
                      student->english      + student->math    +
                      student->history) / 5.0f;

    student->total_grade = letter_grade((u8)(student->total + 0.5f));
}

/* ═══════════════════════════════════════════════════════════════════
   INTERNAL LOOKUP  (no output, returns pointer or NULL)
═══════════════════════════════════════════════════════════════════ */

static Student *lookup_by_id(School *school, const char *id)
{
    Student *cur = school->front;
    while (cur != NULL) {
        if (!stricmp(cur->id, id)) return cur;
        cur = cur->next;
    }
    return NULL;
}

static Student *lookup_by_name(School *school, const char *name)
{
    Student *cur = school->front;
    while (cur != NULL) {
        if (!stricmp(cur->name, name)) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════
   PROFILE FIELD HELPER
   ─────────────────────────────────────────────────────────────────
   Collect all profile fields for a student that already exists in
   the list. The caller must clear student->name and student->id
   before calling to prevent the uniqueness check from matching the
   student itself (self-match prevention).
═══════════════════════════════════════════════════════════════════ */
static void fill_profile_fields(School *school, Student *student)
{
    char tmp_name[NAME_LEN];   /* staging buffer — checked for uniqueness before copy */
    char tmp_id  [ID_LEN];     /* staging buffer — checked for uniqueness before copy */

    /* Name — must be unique across the database */
    while (1) {
        read_name(tmp_name);
        if (student_exists_by_name(school, tmp_name))
            printf("    A student with this name already exists."
                   " Enter a different name.\n");
        else {
            strncpy(student->name, tmp_name, NAME_LEN - 1);
            break;
        }
    }

    read_date(student->birth);

    /* ID — must be unique across the database */
    while (1) {
        read_id(tmp_id);
        if (student_exists_by_id(school, tmp_id))
            printf("    This ID is already in use."
                   " Enter a different ID.\n");
        else {
            strncpy(student->id, tmp_id, ID_LEN - 1);
            break;
        }
    }

    read_address(student->address);
    read_phone(student->phone);
}

/* ═══════════════════════════════════════════════════════════════════
   SCORE EDIT HELPER
   ─────────────────────────────────────────────────────────────────
   Edit scores for one student.
   mode 'A': all five subjects in sequence.
   mode 'S': user selects one subject from a numbered list.
   Grades are recomputed on exit via compute_grades().
═══════════════════════════════════════════════════════════════════ */
static void edit_one_student_scores(Student *student, char mode)
{
    char subject_choice;   /* '1'-'5': subject selection in mode S           */

    if (mode == 'A') {
        printf("\n  Enter new scores (0 to 100):\n\n");
        student->comp_science = read_score("Comp. Science");
        student->science      = read_score("Science");
        student->english      = read_score("English");
        student->math         = read_score("Math");
        student->history      = read_score("History");
    } else {
        printf("\n  Select subject:\n");
        printf("    1 - Comp. Science   (current: %3d  %c)\n",
               student->comp_science, student->comp_science_grade);
        printf("    2 - Science         (current: %3d  %c)\n",
               student->science,      student->science_grade);
        printf("    3 - English         (current: %3d  %c)\n",
               student->english,      student->english_grade);
        printf("    4 - Math            (current: %3d  %c)\n",
               student->math,         student->math_grade);
        printf("    5 - History         (current: %3d  %c)\n",
               student->history,      student->history_grade);
        printf("    > ");
        subject_choice = READ_CHAR(); printf("\n");
        printf("\n  New score (0 to 100):\n\n");
        switch (subject_choice) {
            case '1': student->comp_science = read_score("Comp. Science"); break;
            case '2': student->science      = read_score("Science");       break;
            case '3': student->english      = read_score("English");       break;
            case '4': student->math         = read_score("Math");          break;
            case '5': student->history      = read_score("History");       break;
            default:
                printf("  Invalid selection. No changes made.\n");
                return;
        }
    }
    compute_grades(student);
}

/* ═══════════════════════════════════════════════════════════════════
   FLOW: STUDENT PROFILE LOOKUP, EDIT & REMOVE  (option 2)
   ─────────────────────────────────────────────────────────────────
   State machine using labeled goto for menu-level navigation.
   This is the standard pattern for console-menu state machines in C.

   State transitions:
     choose_method -> do_search -> [not found] -> prompt_retry
                                -> [found]     -> show_actions
                                                  E -> fill_profile_fields
                                                  R -> confirm_remove
                                                  B -> choose_method
═══════════════════════════════════════════════════════════════════ */
static void flow_profile_lookup(School *school)
{
    char     method;                    /* 'N' by name, 'I' by ID            */
    char     action;                    /* 'E' edit, 'R' remove, 'B' back    */
    char     response;                  /* generic single-key response        */
    char     search_term[INPUT_BUF];    /* validated search input             */
    Student *found = NULL;              /* pointer to the located node        */
    char     saved_id[ID_LEN];          /* ID saved before free() in remove  */

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }
    print_section("STUDENT PROFILE LOOKUP, EDIT & REMOVE");

choose_method:
    printf("\n  Search by:\n");
    printf("    N - Name     I - ID     B - Back to Main Menu\n");
    printf("    > ");
    method = (char)toupper(READ_CHAR()); printf("\n");
    if (method == 'B') { print_end(); return; }
    if (method != 'N' && method != 'I') {
        printf("  Enter N, I, or B.\n");
        goto choose_method;
    }

do_search:
    printf("\n  Enter student %s:\n", method == 'N' ? "name" : "ID");
    if (method == 'N') { if (!read_search_name(search_term)) goto choose_method; }
    else               { if (!read_search_id  (search_term)) goto choose_method; }

    found = (method == 'N') ? lookup_by_name(school, search_term)
                            : lookup_by_id  (school, search_term);

    if (found == NULL) {
        printf("\n  No student found with %s: %s\n",
               method == 'N' ? "name" : "ID", search_term);
        printf("\n  N - by Name     I - by ID     B - Back\n");
        printf("    > ");
        response = (char)toupper(READ_CHAR()); printf("\n");
        if (response == 'B')              { print_end(); return; }
        if (response == 'N' || response == 'I') method = response;
        goto do_search;
    }

    printf("\n  Student found:\n\n");
    print_profile_card(found);

show_actions:
    printf("    E - Edit   R - Remove   B - Back\n");
    printf("    > ");
    action = (char)toupper(READ_CHAR()); printf("\n");

    if (action == 'B') goto choose_method;

    if (action == 'E') {
        printf("\n  Enter new details:\n");
        /* clear current fields so uniqueness check does not match self */
        strcpy(found->name, "");
        strcpy(found->id,   "");
        fill_profile_fields(school, found);

        printf("\n  Updated profile:\n\n");
        print_profile_card(found);

        printf("    %s - Search again   B - Back\n",
               method == 'N' ? "N" : "I");
        printf("    > ");
        response = (char)toupper(READ_CHAR()); printf("\n");
        if (response == 'B')              { print_end(); return; }
        if (response == 'N' || response == 'I') method = response;
        goto do_search;
    }

    if (action == 'R') {
        printf("\n  Confirm removal of: %s\n", found->name);
        printf("    Y - Yes, remove     N - No, go back\n");
        printf("    > ");
        response = (char)toupper(READ_CHAR()); printf("\n");

        if (response == 'N') { printf("\n"); goto show_actions; }

        if (response == 'Y') {
            /* save ID before the node is freed by delete_student() */
            strncpy(saved_id, found->id, ID_LEN - 1);
            saved_id[ID_LEN - 1] = '\0';
            delete_student(school, saved_id);

            printf("  Search for another student?\n");
            printf("    N - by Name     I - by ID     B - Back\n");
            printf("    > ");
            response = (char)toupper(READ_CHAR()); printf("\n");
            if (response == 'B')              { print_end(); return; }
            if (response == 'N' || response == 'I') method = response;
            goto do_search;
        }
        goto show_actions;
    }

    printf("  Enter E, R, or B.\n");
    goto show_actions;
}

/* ═══════════════════════════════════════════════════════════════════
   FLOW: STUDENT SCORES LOOKUP & EDIT  (option 5)
═══════════════════════════════════════════════════════════════════ */
static void flow_scores_lookup(School *school)
{
    char     method;                    /* 'N' by name, 'I' by ID            */
    char     action;                    /* 'E' edit, 'N' search, 'B' back    */
    char     response;                  /* generic single-key response        */
    char     edit_mode;                 /* 'A' all subjects, 'S' specific     */
    char     search_term[INPUT_BUF];    /* validated search input             */
    Student *found = NULL;              /* pointer to the located node        */

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }
    print_section("STUDENT SCORES LOOKUP & EDIT");

sc_method:
    printf("\n  Search by:\n");
    printf("    N - Name     I - ID     B - Back to Main Menu\n");
    printf("    > ");
    method = (char)toupper(READ_CHAR()); printf("\n");
    if (method == 'B') { print_end(); return; }
    if (method != 'N' && method != 'I') {
        printf("  Enter N, I, or B.\n");
        goto sc_method;
    }

sc_search:
    printf("\n  Enter student %s:\n", method == 'N' ? "name" : "ID");
    if (method == 'N') { if (!read_search_name(search_term)) goto sc_method; }
    else               { if (!read_search_id  (search_term)) goto sc_method; }

    found = (method == 'N') ? lookup_by_name(school, search_term)
                            : lookup_by_id  (school, search_term);

    if (found == NULL) {
        printf("\n  No student found with %s: %s\n",
               method == 'N' ? "name" : "ID", search_term);
        printf("\n  N - by Name     I - by ID     B - Back\n");
        printf("    > ");
        response = (char)toupper(READ_CHAR()); printf("\n");
        if (response == 'B')              { print_end(); return; }
        if (response == 'N' || response == 'I') method = response;
        goto sc_search;
    }

    print_score_card(found);

sc_actions:
    printf("    E - Edit Scores     N - Search Again     B - Back\n");
    printf("    > ");
    action = (char)toupper(READ_CHAR()); printf("\n");

    if (action == 'B') goto sc_method;
    if (action == 'N') goto sc_search;

    if (action == 'E') {
        printf("\n  Edit mode:\n");
        printf("    A - All subjects     S - Specific subject\n");
        printf("    > ");
        edit_mode = (char)toupper(READ_CHAR()); printf("\n");
        if (edit_mode != 'A' && edit_mode != 'S') {
            printf("  Enter A or S.\n");
            goto sc_actions;
        }
        edit_one_student_scores(found, edit_mode);
        printf("\n  Updated scores:\n\n");
        print_score_card(found);
        printf("    N - Search Again     B - Back\n");
        printf("    > ");
        response = (char)toupper(READ_CHAR()); printf("\n");
        if (response == 'B') { print_end(); return; }
        if (response == 'N' || response == 'I') method = response;
        goto sc_search;
    }

    printf("  Enter E, N, or B.\n");
    goto sc_actions;
}

/* ═══════════════════════════════════════════════════════════════════
   MAIN MENU
═══════════════════════════════════════════════════════════════════ */
void run_main_menu(void)
{
    School school;          /* the single school database for this session    */
    char   choice;          /* user's menu selection (single keypress)        */
    char   count_str[32];   /* formatted enrollment count for the header      */
    i8     spaces;          /* padding between "MAIN MENU" and count string   */
                            /* i8: max ~50, min ~29; always positive, i8 fits */

    /* Welcome banner */
    printf("\n");
    printf("%s\n", BOX_BORDER);
    printf("  |  %-61s|\n", WELCOME_TITLE);
    printf("  |  %-61s|\n", WELCOME_SUBTITLE);
    printf("%s\n", BOX_BORDER);
    printf("  |  %-61s|\n", WELCOME_L1);
    printf("  |  %-61s|\n", WELCOME_L2);
    printf("  |  %-61s|\n", WELCOME_L3);
    printf("  |  %-61s|\n", "");
    printf("  |  %-61s|\n", WELCOME_L4);
    printf("%s\n\n", BOX_BORDER);

    init_school(&school);

    do {
        /* Build enrollment count string for the menu header */
        if      (school.count == 0) sprintf(count_str, "No students enrolled");
        else if (school.count == 1) sprintf(count_str, "1 student enrolled");
        else                        sprintf(count_str, "%d students enrolled",
                                            (i32)school.count);

        /* Pad "MAIN MENU" (11 chars) and count to fill 63-char inner width */
        spaces = (i8)(63 - 11 - 1 - (i8)strlen(count_str));
        if (spaces < 1) spaces = 1;

        printf("%s\n", BOX_BORDER);
        printf("  |  MAIN MENU%*s%s |\n", (i32)spaces, "", count_str);
        printf("%s\n", BOX_BORDER);
        printf("  |  %-61s|\n", "  -- Profiles --");
        printf("  |   %-60s|\n", OPT_1);
        printf("  |   %-60s|\n", OPT_2);
        printf("  |   %-60s|\n", OPT_3);
        printf("  |   %-60s|\n", OPT_4);
        printf("  |  %-61s|\n", "  -- Scores --");
        printf("  |   %-60s|\n", OPT_5);
        printf("  |   %-60s|\n", OPT_6);
        printf("  |   %-60s|\n", OPT_7);
        printf("  |   %-60s|\n", OPT_8);
        printf("  |  %-61s|\n", "");
        printf("  |   %-60s|\n", OPT_D);
        printf("%s\n", BOX_BORDER);
        printf("  Your choice: ");
        choice = READ_CHAR();
        printf("\n");

        switch (choice)
        {
            case '1': add_student          (&school); break;
            case '2': flow_profile_lookup  (&school); break;
            case '3': print_all_profiles   (&school); break;
            case '4': list_profiles_sorted (&school); break;
            case '5': flow_scores_lookup   (&school); break;
            case '6': edit_all_scores      (&school); break;
            case '7': print_all_scores     (&school); break;
            case '8': print_scores_ranked  (&school); break;
            case 'D':
            case 'd': printf("\n  Goodbye!\n\n");     break;
            default:  printf("  Invalid choice."
                             " Please select a valid option.\n\n");
        }
    } while (choice != 'D' && choice != 'd');
}

/* ═══════════════════════════════════════════════════════════════════
   init_school
═══════════════════════════════════════════════════════════════════ */
void init_school(School *school)
{
    school->front = NULL;   /* no students yet                               */
    school->rear  = NULL;   /* no students yet                               */
    school->count = 0;      /* enrollment starts at zero                     */
}

/* ═══════════════════════════════════════════════════════════════════
   is_empty
═══════════════════════════════════════════════════════════════════ */
bool is_empty(School *school)
{
    return (school->front == NULL);
}

/* ═══════════════════════════════════════════════════════════════════
   add_student
═══════════════════════════════════════════════════════════════════ */
void add_student(School *school)
{
    Student *new_node = (Student *)malloc(sizeof(Student));

    if (new_node == NULL) {
        printf("  Error: insufficient memory.\n\n");
        return;
    }

    print_section("ADD NEW STUDENT PROFILE");

    /* Zero all fields before filling to avoid garbage values */
    memset(new_node, 0, sizeof(Student));

    fill_profile_fields(school, new_node);

    /* All scores start at 0; derive initial grades */
    compute_grades(new_node);
    new_node->next = NULL;

    /* Enqueue at the rear */
    if (is_empty(school)) {
        school->front = new_node;
        school->rear  = new_node;
    } else {
        school->rear->next = new_node;
        school->rear       = new_node;
    }
    school->count++;

    printf("\n  Student profile created successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   delete_student
═══════════════════════════════════════════════════════════════════ */
void delete_student(School *school, const char *id)
{
    Student *cur  = NULL;   /* node currently examined during traversal       */
    Student *prev = NULL;   /* node preceding cur (needed to relink the list) */

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }

    /* Single-student case */
    if (school->front == school->rear &&
        !stricmp(school->front->id, id))
    {
        free(school->front);
        school->front = school->rear = NULL;
        school->count--;
        printf("  Student profile removed successfully.\n");
        print_end();
        return;
    }

    /* Traverse to find the target node */
    cur = school->front;
    while (cur != NULL && stricmp(cur->id, id)) {
        prev = cur;
        cur  = cur->next;
    }

    if (cur == NULL) {
        printf("  No student found with ID: %s\n", id);
        return;
    }

    /* Relink based on position: front, rear, or middle */
    if      (school->front == cur) { school->front = cur->next; }
    else if (school->rear  == cur) { prev->next = NULL; school->rear = prev; }
    else                           { prev->next = cur->next; }

    free(cur);
    school->count--;
    printf("  Student profile removed successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   edit_all_scores  (option 6)
   User selects a mode once (A = all subjects, S = one subject).
   The chosen mode is applied to every student in sequence.
   Per student: any key to edit, K to skip, B to stop and save.
═══════════════════════════════════════════════════════════════════ */
void edit_all_scores(School *school)
{
    char     mode;              /* 'A' all subjects, 'S' specific subject     */
    char     subject_choice;    /* '1'-'5' for mode S, selected once upfront  */
    char     key;               /* per-student action: edit / K skip / B stop */
    u16      student_index;     /* 1-based progress counter (u16 matches count)*/
    char     header[64];        /* "Student N of M: Name" formatted string     */
    Student *cur = NULL;        /* current student being processed             */

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }
    print_section("EDIT ALL STUDENTS' SCORES");

choose_mode:
    printf("\n  Edit mode:\n");
    printf("    A - All subjects     S - Specific subject     B - Back\n");
    printf("    > ");
    mode = (char)toupper(READ_CHAR()); printf("\n");
    if (mode == 'B') { print_end(); return; }
    if (mode != 'A' && mode != 'S') {
        printf("  Enter A, S, or B.\n");
        goto choose_mode;
    }

    if (mode == 'S') {
        printf("\n  Select subject:\n");
        printf("    1 - Comp. Science\n");
        printf("    2 - Science\n");
        printf("    3 - English\n");
        printf("    4 - Math\n");
        printf("    5 - History\n");
        printf("    > ");
        subject_choice = READ_CHAR(); printf("\n");
        if (subject_choice < '1' || subject_choice > '5') {
            printf("  Invalid selection.\n");
            goto choose_mode;
        }
    }

    printf("\n  Scores must be whole numbers between 0 and 100.\n");
    printf("  K - Skip student     B - Stop and save progress\n");

    cur           = school->front;
    student_index = 0;

    while (cur != NULL) {
        student_index++;
        snprintf(header, sizeof(header), "Student %d of %d: %s",
                 (i32)student_index, (i32)school->count, cur->name);

        printf("\n%s\n", BOX_BORDER);
        printf("  |  %-61s|\n", header);
        printf("  |  %-14s: %-45s|\n", "ID", cur->id);
        printf("%s\n", BOX_BORDER);
        print_score_detail_table(cur);

        printf("    K - Skip     B - Stop and save     [any other key] - Edit\n");
        printf("    > ");
        key = (char)toupper(READ_CHAR()); printf("\n");

        if (key == 'B') break;
        if (key == 'K') { cur = cur->next; continue; }

        /* Edit scores in the chosen mode */
        if (mode == 'A') {
            printf("\n  Enter new scores:\n\n");
            cur->comp_science = read_score("Comp. Science");
            cur->science      = read_score("Science");
            cur->english      = read_score("English");
            cur->math         = read_score("Math");
            cur->history      = read_score("History");
        } else {
            printf("\n  Enter new score:\n\n");
            switch (subject_choice) {
                case '1': cur->comp_science = read_score("Comp. Science"); break;
                case '2': cur->science      = read_score("Science");       break;
                case '3': cur->english      = read_score("English");       break;
                case '4': cur->math         = read_score("Math");          break;
                case '5': cur->history      = read_score("History");       break;
            }
        }
        compute_grades(cur);

        printf("\n  Updated:\n");
        print_score_detail_table(cur);

        cur = cur->next;
    }

    printf("  Score editing complete.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   student_exists_by_id  (silent search — returns bool)
═══════════════════════════════════════════════════════════════════ */
bool student_exists_by_id(School *school, const char *id)
{
    return (lookup_by_id(school, id) != NULL);
}

/* ═══════════════════════════════════════════════════════════════════
   student_exists_by_name  (silent search — returns bool)
═══════════════════════════════════════════════════════════════════ */
bool student_exists_by_name(School *school, const char *name)
{
    return (lookup_by_name(school, name) != NULL);
}
