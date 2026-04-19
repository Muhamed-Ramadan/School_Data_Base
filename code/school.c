#include "school.h"
#include <time.h>

/* ═══════════════════════════════════════════════════════════
   INTERNAL HELPERS
═══════════════════════════════════════════════════════════ */

/* Strip the trailing newline that fgets stores */
static void trim_newline(char *s)
{
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

/* Flush characters left in stdin after a scanf or partial read */
static void clear_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ── Box / section printing ─────────────────────────────── */

/*
 * Prints a section header that matches BOX_BORDER width (67 chars):
 *   +--- TITLE --...--+
 * Padding = 58 - strlen(title).
 */
static void print_section(const char *title)
{
    int pad = 58 - (int)strlen(title);
    if (pad < 1) pad = 1;
    printf("\n  +--- %s ", title);
    for (int i = 0; i < pad; i++) putchar('-');
    puts("+");
}

/* Matching closing border with blank line */
static void print_end(void)
{
    printf("%s\n\n", BOX_BORDER);
}

/* ── Card row printers (BOX_BORDER width = 67 chars) ────── */
/*
 * Row format (67 chars):
 *   "  |  " (5) + label(14) + ": " (2) + value(39) + "      " (6) + "|" (1) = 67
 */
static void card_row_str(const char *label, const char *value)
{
    char v[40];
    strncpy(v, value, 39);
    v[39] = '\0';
    printf("  |  %-14s: %-39s      |\n", label, v);
}

/* ── Score row printers ─────────────────────────────────── */
/*
 * Integer score row (67 chars):
 *   "  |  " (5) + subj(16) + ": " (2) + score(3) + "   Grade: " (10) + gpa(1) + pad(29) + "|" (1) = 67
 */
static void score_row(const char *subj, int sc, char gpa)
{
    printf("  |  %-16s: %3d   Grade: %c%*s|\n", subj, sc, gpa, 29, "");
}

/*
 * Float total row (67 chars):
 *   "  |  " (5) + "Total Score     " (16) + ": " (2) + total(5) + " Grade: " (8) + gpa(1) + pad(29) + "|" (1) = 67
 */
static void score_row_total(float total, char gpa)
{
    printf("  |  %-16s: %5.1f Grade: %c%*s|\n", "Total Score", total, gpa, 29, "");
}

/* ── GPA calculation ────────────────────────────────────── */

/* American letter-grade system (A/B/C/D/F) */
static char calc_gpa_letter(int score)
{
    if (score >= 90) return 'A';
    if (score >= 80) return 'B';
    if (score >= 70) return 'C';
    if (score >= 60) return 'D';
    return 'F';
}

/*
 * Recompute and store all derived grade fields.
 * Call this after any score change.
 */
static void update_gpa(Student *s)
{
    s->com_gpa   = calc_gpa_letter(s->com_sc);
    s->sci_gpa   = calc_gpa_letter(s->sci_sc);
    s->eng_gpa   = calc_gpa_letter(s->eng_sc);
    s->math_gpa  = calc_gpa_letter(s->math_sc);
    s->hist_gpa  = calc_gpa_letter(s->hist_sc);
    s->total_sc  = (s->com_sc + s->sci_sc + s->eng_sc +
                    s->math_sc + s->hist_sc) / 5.0f;
    s->total_gpa = calc_gpa_letter((int)(s->total_sc + 0.5f));
}

/* ═══════════════════════════════════════════════════════════
   INPUT VALIDATORS
═══════════════════════════════════════════════════════════ */

static bool is_leap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int month, int year)
{
    int d[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap(year)) return 29;
    return d[month];
}

/*
 * Name rules:
 *   - Characters : A-Z  a-z  space  hyphen (-)
 *   - Must start and end with a letter
 *   - No consecutive spaces
 *   - Hyphen must have a letter on each side
 *   - Each word/part >= 2 characters
 *   - Overall length: 2 to NAME_LEN-1
 */
static bool validate_name(const char *s)
{
    int len = (int)strlen(s);
    if (len < 2 || len >= NAME_LEN) return false;
    if (!isalpha((unsigned char)s[0]))     return false;
    if (!isalpha((unsigned char)s[len-1])) return false;

    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalpha(c) && c != ' ' && c != '-') return false;
        if (c == ' '  && (unsigned char)s[i + 1] == ' ')  return false;
        if (c == '-') {
            if (i == 0 || !isalpha((unsigned char)s[i - 1])) return false;
            if (i + 1 >= len || !isalpha((unsigned char)s[i + 1])) return false;
        }
    }

    /* each word / hyphen-separated part must be >= 2 chars */
    int part = 0;
    for (int i = 0; i <= len; i++) {
        if (s[i] == ' ' || s[i] == '-' || s[i] == '\0') {
            if (part > 0 && part < 2) return false;
            part = 0;
        } else {
            part++;
        }
    }
    return true;
}

/*
 * ID rules:
 *   - Characters : A-Z  a-z  0-9  (no spaces)
 *   - Length     : 4 to ID_LEN-1
 */
static bool validate_id(const char *s)
{
    int len = (int)strlen(s);
    if (len < 4 || len >= ID_LEN) return false;
    for (int i = 0; s[i]; i++)
        if (!isalnum((unsigned char)s[i])) return false;
    return true;
}

/*
 * Date rules:
 *   - Format  : DD/MM/YYYY
 *   - Valid calendar day/month (leap year aware)
 *   - Student age must be 14 – 21 (inclusive) as of today
 */
static bool validate_date(const char *s)
{
    if ((int)strlen(s) != 10) return false;

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) { if (s[i] != '/')                    return false; }
        else                  { if (!isdigit((unsigned char)s[i]))   return false; }
    }

    int day   = (s[0]-'0')*10 + (s[1]-'0');
    int month = (s[3]-'0')*10 + (s[4]-'0');
    int year  = (s[6]-'0')*1000 + (s[7]-'0')*100 +
                (s[8]-'0')*10   + (s[9]-'0');

    if (month < 1 || month > 12)                return false;
    if (day < 1 || day > days_in_month(month, year)) return false;

    time_t     t   = time(NULL);
    struct tm *now = localtime(&t);
    int cy = now->tm_year + 1900, cm = now->tm_mon + 1, cd = now->tm_mday;

    int age = cy - year;
    if (cm < month || (cm == month && cd < day)) age--;

    return (age >= 14 && age <= 21);
}

/*
 * Address rules:
 *   - Characters : A-Z  a-z  0-9  space  .  ,  -  /
 *   - No consecutive spaces; must not start or end with a space
 *   - Length     : 10 to ADDRESS_LEN-1
 */
static bool validate_address(const char *s)
{
    int len = (int)strlen(s);
    if (len < 10 || len >= ADDRESS_LEN)   return false;
    if (s[0] == ' ' || s[len-1] == ' ')  return false;

    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != ' ' && c != ',' &&
            c != '.'    && c != '-' && c != '/')    return false;
        if (c == ' ' && (unsigned char)s[i + 1] == ' ') return false;
    }
    return true;
}

/*
 * Phone rules:
 *   - Characters : 0-9 only  (use 00 for international prefix)
 *   - Length     : 6 to 15 digits
 */
static bool validate_phone(const char *s)
{
    int len = (int)strlen(s);
    if (len < 6 || len > 15) return false;
    for (int i = 0; s[i]; i++)
        if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

/* ═══════════════════════════════════════════════════════════
   INPUT COLLECTORS
═══════════════════════════════════════════════════════════ */

static void input_name(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  Name\n");
    printf("    Allowed  :  A-Z   a-z   space   -\n");
    printf("    Note     :  Each word min 2 chars  "
           "|  Hyphen ( - ) must be between two letters\n");
    printf("    Length   :  2 to 29 characters\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_name(buf))
            printf("    Invalid input. Please follow the format above.\n");
        else {
            strncpy(dest, buf, NAME_LEN - 1);
            dest[NAME_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_id(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  ID\n");
    printf("    Allowed  :  A-Z   a-z   0-9   ( no spaces )\n");
    printf("    Length   :  4 to 9 characters\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_id(buf))
            printf("    Invalid input. Please follow the format above.\n");
        else {
            strncpy(dest, buf, ID_LEN - 1);
            dest[ID_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_date(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  Date of Birth\n");
    printf("    Format   :  DD/MM/YYYY   e.g. 14/03/2005\n");
    printf("    Age      :  Student must be 14 to 21 years old\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_date(buf))
            printf("    Invalid date, format, or age out of range (14-21)."
                   " Please try again.\n");
        else {
            strncpy(dest, buf, BIRTH_LEN - 1);
            dest[BIRTH_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_address(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  Address\n");
    printf("    Allowed  :  A-Z   a-z   0-9   space   .   ,   -   /\n");
    printf("    Length   :  10 to 39 characters  |  No consecutive spaces\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_address(buf))
            printf("    Invalid input. Please follow the format above.\n");
        else {
            strncpy(dest, buf, ADDRESS_LEN - 1);
            dest[ADDRESS_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_phone(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  Guardian Phone\n");
    printf("    Allowed  :  0-9\n");
    printf("    Note     :  Use 00 for international prefix"
           "   e.g. 0020100123456\n");
    printf("    Length   :  6 to 15 digits\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_phone(buf))
            printf("    Invalid input. Digits only, 6 to 15 characters.\n");
        else {
            strncpy(dest, buf, PHONE_LEN - 1);
            dest[PHONE_LEN - 1] = '\0';
            break;
        }
    }
}

static int input_score(const char *subject)
{
    int score;
    printf("\n    %s\n", subject);
    printf("    Allowed  :  0-9   |   Range: 0 to 100\n");
    while (1) {
        printf("    > ");
        if (scanf("%d", &score) == 1 && score >= 0 && score <= 100) {
            clear_stdin();
            return score;
        }
        clear_stdin();
        printf("    Invalid. Enter a whole number between 0 and 100.\n");
    }
}

/* For search — show hints, validate, return false on bad input */
static bool input_search_id(char *dest)
{
    char buf[INPUT_BUF];
    printf("    Allowed  :  A-Z   a-z   0-9   |  Length: 4 to 9 chars\n");
    printf("    > ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    if (!validate_id(buf)) {
        printf("  Invalid ID format.\n\n");
        return false;
    }
    strncpy(dest, buf, ID_LEN - 1);
    dest[ID_LEN - 1] = '\0';
    return true;
}

static bool input_search_name(char *dest)
{
    char buf[INPUT_BUF];
    printf("    Allowed  :  A-Z   a-z   space   -   |  Length: 2 to 29 chars\n");
    printf("    > ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    if (!validate_name(buf)) {
        printf("  Invalid name format.\n\n");
        return false;
    }
    strncpy(dest, buf, NAME_LEN - 1);
    dest[NAME_LEN - 1] = '\0';
    return true;
}

/* Print all students in current list order (no section header) */
static void print_students_raw(School *pschool)
{
    Student *temp = pschool->front;
    int i = 0;
    while (temp != NULL) {
        i++;
        printf("  [%d]\n", i);
        PRINT_STUDENT(temp);
        temp = temp->pNext;
    }
}

/* ═══════════════════════════════════════════════════════════
   MAIN MENU
═══════════════════════════════════════════════════════════ */
void MAIN_MENU(void)
{
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

    School Q;
    char   choice;
    char   str[INPUT_BUF];

    CreateSchool(&Q);

    enum {
        APPEND             = '1',
        EDIT               = '2',
        SEARCH_ID          = '3',
        SEARCH_NAME        = '4',
        LIST               = '5',
        SIZE               = '6',
        DELETE             = '7',
        PRINT              = '8',
        EDIT_SCORE_        = '9',
        PRINT_SCORE_       = 'A',
        RANK_STUDENT_      = 'B',
        EXIT               = 'C',
        PRINT_SCORE_lower  = 'a',
        RANK_STUDENT_lower = 'b',
        EXIT_lower         = 'c',
    };

    do {
        printf("%s\n", BOX_BORDER);
        printf("  |  %-61s|\n", "MAIN MENU");
        printf("%s\n", BOX_BORDER);
        printf("  |   %-60s|\n", OPT_1);
        printf("  |   %-60s|\n", OPT_2);
        printf("  |   %-60s|\n", OPT_3);
        printf("  |   %-60s|\n", OPT_4);
        printf("  |   %-60s|\n", OPT_5);
        printf("  |   %-60s|\n", OPT_6);
        printf("  |   %-60s|\n", OPT_7);
        printf("  |   %-60s|\n", OPT_8);
        printf("  |   %-60s|\n", OPT_9);
        printf("  |   %-60s|\n", OPT_A);
        printf("  |   %-60s|\n", OPT_B);
        printf("  |   %-60s|\n", OPT_C);
        printf("%s\n", BOX_BORDER);
        printf("  Your choice: ");
        choice = getche();
        printf("\n");

        switch (choice)
        {
            case APPEND:
                NEW_STUDENT(&Q);
                break;

            case EDIT:
                print_section("EDIT STUDENT RECORD");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_id(str)) STUDENT_EDIT(&Q, str);
                break;

            case SEARCH_ID:
                print_section("FIND STUDENT BY ID");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_id(str)) STUDENT_SEARCH_ID(&Q, str);
                print_end();
                break;

            case SEARCH_NAME:
                print_section("FIND STUDENT BY NAME");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_name(str)) STUDENT_SEARCH_NAME(&Q, str);
                print_end();
                break;

            case LIST:
                STUDENT_LIST(&Q);
                break;

            case SIZE:
                SchoolSize(&Q);
                break;

            case DELETE:
                print_section("REMOVE STUDENT RECORD");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_id(str)) DELETE_STUDENT(&Q, str);
                break;

            case PRINT:
                PRINT_SCHOOL(&Q);
                break;

            case EDIT_SCORE_:
                STUDENT_SCORE(&Q);
                break;

            case PRINT_SCORE_:
            case PRINT_SCORE_lower:
                PRINT_SCHOOL_SCORE(&Q);
                break;

            case RANK_STUDENT_:
            case RANK_STUDENT_lower:
                RANK_STUDENT(&Q);
                PRINT_SCHOOL_SCORE(&Q);
                break;

            case EXIT:
            case EXIT_lower:
                printf("\n  Goodbye!\n\n");
                break;

            default:
                printf("  Invalid choice. Please select a valid option.\n\n");
        }

    } while (choice != EXIT && choice != EXIT_lower);
}

/* ═══════════════════════════════════════════════════════════
   CreateSchool
═══════════════════════════════════════════════════════════ */
void CreateSchool(School *pschool)
{
    pschool->front = NULL;
    pschool->rear  = NULL;
    pschool->size  = 0;
}

/* ═══════════════════════════════════════════════════════════
   isEmpty
═══════════════════════════════════════════════════════════ */
bool isEmpty(School *pschool)
{
    return (pschool->front == NULL);
}

/* ═══════════════════════════════════════════════════════════
   NEW_STUDENT
═══════════════════════════════════════════════════════════ */
void NEW_STUDENT(School *pschool)
{
    Student *p = (Student *)malloc(sizeof(Student));
    if (p == NULL) { printf("  Error: insufficient memory.\n\n"); return; }

    print_section("ADD NEW STUDENT RECORD");

    /* Name — must be unique */
    while (1) {
        input_name(p->name);
        if (STUDENT_SEARCH_NAME_SILENT(pschool, p->name))
            printf("    A student with this name already exists."
                   " Please enter a different name.\n");
        else break;
    }

    /* Date of birth */
    input_date(p->birth);

    /* ID — must be unique */
    while (1) {
        input_id(p->ID);
        if (STUDENT_SEARCH_ID_SILENT(pschool, p->ID))
            printf("    This ID is already in use."
                   " Please enter a different ID.\n");
        else break;
    }

    /* Address */
    input_address(p->address);

    /* Guardian phone */
    input_phone(p->phone);

    /* Initialise scores and derive GPA */
    p->com_sc = p->sci_sc = p->eng_sc = p->math_sc = p->hist_sc = 0;
    update_gpa(p);
    p->pNext = NULL;

    /* Enqueue */
    if (isEmpty(pschool)) {
        pschool->front = p;
        pschool->rear  = p;
    } else {
        pschool->rear->pNext = p;
        pschool->rear        = p;
    }
    (pschool->size)++;

    printf("\n  Student record created successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_EDIT
═══════════════════════════════════════════════════════════ */
void STUDENT_EDIT(School *pschool, char id[ID_LEN])
{
    char tmp_name[NAME_LEN];
    char tmp_id  [ID_LEN];

    Student *t = STUDENT_SEARCH_ID(pschool, id);
    if (t == NULL) { print_end(); return; }

    printf("\n  Enter new details:\n");

    /* Name — unique excluding current record */
    while (1) {
        input_name(tmp_name);
        strcpy(t->name, "");
        if (STUDENT_SEARCH_NAME_SILENT(pschool, tmp_name))
            printf("    A student with this name already exists."
                   " Please enter a different name.\n");
        else {
            strncpy(t->name, tmp_name, NAME_LEN - 1);
            break;
        }
    }

    input_date(t->birth);

    /* ID — unique excluding current record */
    while (1) {
        input_id(tmp_id);
        strcpy(t->ID, "");
        if (STUDENT_SEARCH_ID_SILENT(pschool, tmp_id))
            printf("    This ID is already in use."
                   " Please enter a different ID.\n");
        else {
            strncpy(t->ID, tmp_id, ID_LEN - 1);
            break;
        }
    }

    input_address(t->address);
    input_phone  (t->phone);

    printf("\n  Student record updated successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_SEARCH_ID
═══════════════════════════════════════════════════════════ */
Student *STUDENT_SEARCH_ID(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return NULL; }

    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->ID, id)) {
            printf("\n  Student found:\n\n");
            PRINT_STUDENT(temp);
            return temp;
        }
        temp = temp->pNext;
    }
    printf("  No student found with ID: %s\n\n", id);
    return NULL;
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_SEARCH_NAME
═══════════════════════════════════════════════════════════ */
Student *STUDENT_SEARCH_NAME(School *pschool, char name[NAME_LEN])
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return NULL; }

    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->name, name)) {
            printf("\n  Student found:\n\n");
            PRINT_STUDENT(temp);
            return temp;
        }
        temp = temp->pNext;
    }
    printf("  No student found with name: %s\n\n", name);
    return NULL;
}

/* ═══════════════════════════════════════════════════════════
   DELETE_STUDENT
═══════════════════════════════════════════════════════════ */
void DELETE_STUDENT(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    /* Single-student case */
    if (pschool->front == pschool->rear &&
        !stricmp(pschool->front->ID, id))
    {
        printf("\n  Removing:\n\n");
        PRINT_STUDENT(pschool->front);
        free(pschool->front);
        pschool->front = pschool->rear = NULL;
        (pschool->size)--;
        printf("  Student record removed successfully.\n");
        print_end();
        return;
    }

    Student *cur = pschool->front, *prev = NULL;
    while (cur != NULL && stricmp(cur->ID, id)) {
        prev = cur;
        cur  = cur->pNext;
    }

    if (cur == NULL) {
        printf("  No student found with ID: %s\n", id);
        print_end();
        return;
    }

    printf("\n  Removing:\n\n");
    PRINT_STUDENT(cur);

    if      (pschool->front == cur) { pschool->front = cur->pNext; }
    else if (pschool->rear  == cur) { prev->pNext = NULL; pschool->rear = prev; }
    else                            { prev->pNext = cur->pNext; }

    free(cur);
    (pschool->size)--;
    printf("  Student record removed successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_LIST  (sort A-Z then display)
═══════════════════════════════════════════════════════════ */
void STUDENT_LIST(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS (A-Z)");

    if (pschool->front == pschool->rear) {
        printf("  Only one student in the database:\n\n");
        PRINT_STUDENT(pschool->front);
        print_end();
        return;
    }

    Student *holder = (Student *)malloc(sizeof(Student));
    if (holder == NULL) { printf("  Error: insufficient memory.\n"); return; }

    Student *cur, *prev;
    int flag;
    for (int i = 0; i < pschool->size; i++) {
        cur = pschool->front->pNext; prev = pschool->front; flag = 0;
        while (cur != NULL) {
            if (stricmp(prev->name, cur->name) > 0) {
                flag = 1;
                prev->pNext = cur->pNext; cur->pNext = cur;
                *holder = *prev; *prev = *cur; *cur = *holder;
            }
            prev = cur; cur = cur->pNext;
        }
        if (flag == 0) break;
    }
    free(holder);

    print_students_raw(pschool);
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   PRINT_STUDENT
═══════════════════════════════════════════════════════════ */
void PRINT_STUDENT(Student *pstudent)
{
    printf("%s\n", BOX_BORDER);
    card_row_str("Name",           pstudent->name);
    card_row_str("ID",             pstudent->ID);
    card_row_str("Date of Birth",  pstudent->birth);
    card_row_str("Address",        pstudent->address);
    card_row_str("Guardian Phone", pstudent->phone);
    printf("%s\n\n", BOX_BORDER);
}

/* ═══════════════════════════════════════════════════════════
   PRINT_SCHOOL
═══════════════════════════════════════════════════════════ */
void PRINT_SCHOOL(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENT RECORDS");
    print_students_raw(pschool);
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_SCORE
═══════════════════════════════════════════════════════════ */
void STUDENT_SCORE(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("EDIT ALL STUDENTS' SCORES");
    printf("  Scores must be whole numbers between 0 and 100.\n");

    Student *temp = pschool->front;
    while (temp != NULL) {
        printf("\n  %s\n", BOX_BORDER);
        printf("  |  Student : %-49s|\n", temp->name);
        printf("  |  ID      : %-49s|\n", temp->ID);
        printf("  %s\n", BOX_BORDER);

        temp->com_sc  = input_score("Comp. Science");
        temp->sci_sc  = input_score("Science");
        temp->eng_sc  = input_score("English");
        temp->math_sc = input_score("Math");
        temp->hist_sc = input_score("History");
        update_gpa(temp);

        /* Show results immediately after each student */
        printf("\n  Results:\n");
        PRINT_STUDENT_SCORE(temp);

        temp = temp->pNext;
    }

    printf("  All scores updated successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   PRINT_STUDENT_SCORE
═══════════════════════════════════════════════════════════ */
void PRINT_STUDENT_SCORE(Student *pstudent)
{
    printf("%s\n", BOX_BORDER);
    card_row_str("Name", pstudent->name);
    card_row_str("ID",   pstudent->ID);
    printf("%s\n", BOX_BORDER);
    score_row("Comp. Science", pstudent->com_sc,  pstudent->com_gpa);
    score_row("Science",       pstudent->sci_sc,  pstudent->sci_gpa);
    score_row("English",       pstudent->eng_sc,  pstudent->eng_gpa);
    score_row("Math",          pstudent->math_sc, pstudent->math_gpa);
    score_row("History",       pstudent->hist_sc, pstudent->hist_gpa);
    printf("%s\n", BOX_BORDER);
    score_row_total(pstudent->total_sc, pstudent->total_gpa);
    printf("%s\n\n", BOX_BORDER);
}

/* ═══════════════════════════════════════════════════════════
   PRINT_SCHOOL_SCORE
═══════════════════════════════════════════════════════════ */
void PRINT_SCHOOL_SCORE(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS' SCORES");
    Student *temp = pschool->front;
    int i = 0;
    while (temp != NULL) {
        i++;
        printf("  [%d]\n", i);
        PRINT_STUDENT_SCORE(temp);
        temp = temp->pNext;
    }
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   RANK_STUDENT  (sort by total_sc descending)
═══════════════════════════════════════════════════════════ */
void RANK_STUDENT(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("RANKING BY TOTAL SCORE");

    if (pschool->front == pschool->rear) {
        printf("  Only one student in the database.\n");
        print_end();
        return;
    }

    Student *holder = (Student *)malloc(sizeof(Student));
    if (holder == NULL) { printf("  Error: insufficient memory.\n"); return; }

    Student *cur, *prev;
    int flag;
    for (int i = 0; i < pschool->size; i++) {
        cur = pschool->front->pNext; prev = pschool->front; flag = 0;
        while (cur != NULL) {
            if (prev->total_sc < cur->total_sc) {      /* descending */
                flag = 1;
                prev->pNext = cur->pNext; cur->pNext = cur;
                *holder = *prev; *prev = *cur; *cur = *holder;
            }
            prev = cur; cur = cur->pNext;
        }
        if (flag == 0) break;
    }
    free(holder);

    printf("  Students ranked by total score (highest first).\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════
   SchoolSize
═══════════════════════════════════════════════════════════ */
void SchoolSize(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }
    printf("  Total enrolled students: %d\n\n", pschool->size);
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_SEARCH_ID_SILENT
═══════════════════════════════════════════════════════════ */
bool STUDENT_SEARCH_ID_SILENT(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) return false;
    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->ID, id)) return true;
        temp = temp->pNext;
    }
    return false;
}

/* ═══════════════════════════════════════════════════════════
   STUDENT_SEARCH_NAME_SILENT
═══════════════════════════════════════════════════════════ */
bool STUDENT_SEARCH_NAME_SILENT(School *pschool, char name[NAME_LEN])
{
    if (isEmpty(pschool)) return false;
    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->name, name)) return true;
        temp = temp->pNext;
    }
    return false;
}
