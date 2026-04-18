#include "school.h"

/* ═══════════════════════════════════════════════════════
   INTERNAL HELPERS
═══════════════════════════════════════════════════════ */

/* Remove trailing newline left by fgets */
static void trim_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

/* Discard any characters remaining in stdin up to and including '\n' */
static void clear_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ── Box printing ─────────────────────────────────────── */

static void print_section(const char *title)
{
    /* Produces:  +--- TITLE ----...----+ (inner = 63) */
    int dashes = 58 - (int)strlen(title);
    if (dashes < 1) dashes = 1;
    printf("\n  +--- %s ", title);
    for (int i = 0; i < dashes; i++) putchar('-');
    printf("+\n");
}

static void print_end(void)
{
    printf("  %s\n\n", BOX_BORDER);
}

/* Print one line of a student card:  |  label: value | */
static void card_row_str(const char *label, const char *value)
{
    printf("  |  %-*s: %-*s |\n", CARD_LBL_W, label, CARD_VAL_W, value);
}

static void card_row_int(const char *label, int value)
{
    printf("  |  %-*s: %-*d |\n", CARD_LBL_W, label, CARD_VAL_W, value);
}

/* ── Input validation ─────────────────────────────────── */

/* Name: letters, spaces, hyphens only; 1..NAME_LEN-1 chars */
static bool validate_name(const char *s)
{
    int len = (int)strlen(s);
    if (len == 0 || len >= NAME_LEN) return false;
    for (int i = 0; s[i]; i++)
        if (!isalpha((unsigned char)s[i]) && s[i] != ' ' && s[i] != '-')
            return false;
    return true;
}

/* ID: letters and digits only; 1..ID_LEN-1 chars */
static bool validate_id(const char *s)
{
    int len = (int)strlen(s);
    if (len == 0 || len >= ID_LEN) return false;
    for (int i = 0; s[i]; i++)
        if (!isalnum((unsigned char)s[i]))
            return false;
    return true;
}

/* Date: exactly DD/MM/YYYY, digits at non-slash positions */
static bool validate_date(const char *s)
{
    if (strlen(s) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            if (s[i] != '/') return false;
        } else {
            if (!isdigit((unsigned char)s[i])) return false;
        }
    }
    int day   = (s[0]-'0')*10 + (s[1]-'0');
    int month = (s[3]-'0')*10 + (s[4]-'0');
    return (day >= 1 && day <= 31 && month >= 1 && month <= 12);
}

/* Address: letters, digits, spaces, and . , - / ; 1..ADDRESS_LEN-1 chars */
static bool validate_address(const char *s)
{
    int len = (int)strlen(s);
    if (len == 0 || len >= ADDRESS_LEN) return false;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (!isalnum((unsigned char)c) &&
            c != ' ' && c != ',' && c != '.' && c != '-' && c != '/')
            return false;
    }
    return true;
}

/* Phone: optional leading '+', digits only, total max 13 chars */
static bool validate_phone(const char *s)
{
    int len = (int)strlen(s);
    if (len == 0 || len > 13) return false;
    int start = (s[0] == '+') ? 1 : 0;
    for (int i = start; s[i]; i++)
        if (!isdigit((unsigned char)s[i]))
            return false;
    return true;
}

/* ── Validated input collectors ───────────────────────── */

static void input_name(char *dest, const char *prompt)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Letters, spaces, and hyphens only | max 29 chars ]");
    while (1) {
        printf("  %s", prompt);
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_name(buf))
            printf("  Invalid name. Use letters, spaces, and hyphens only (max 29 chars).\n");
        else {
            strncpy(dest, buf, NAME_LEN - 1);
            dest[NAME_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_id(char *dest, const char *prompt)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Letters and digits only | max 9 chars ]");
    while (1) {
        printf("  %s", prompt);
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_id(buf))
            printf("  Invalid ID. Use letters and digits only (max 9 chars).\n");
        else {
            strncpy(dest, buf, ID_LEN - 1);
            dest[ID_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_date(char *dest, const char *prompt)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Format: DD/MM/YYYY  e.g. 14/03/2005 ]");
    while (1) {
        printf("  %s", prompt);
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_date(buf))
            printf("  Invalid date. Use DD/MM/YYYY format with valid day (1-31) and month (1-12).\n");
        else {
            strncpy(dest, buf, BIRTH_LEN - 1);
            dest[BIRTH_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_address(char *dest, const char *prompt)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Letters, digits, spaces, and . , - /  | max 39 chars ]");
    while (1) {
        printf("  %s", prompt);
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_address(buf))
            printf("  Invalid address. Allowed: letters, digits, spaces, . , - /  (max 39 chars).\n");
        else {
            strncpy(dest, buf, ADDRESS_LEN - 1);
            dest[ADDRESS_LEN - 1] = '\0';
            break;
        }
    }
}

static void input_phone(char *dest, const char *prompt)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Digits only | optional leading + for international | max 13 chars ]");
    while (1) {
        printf("  %s", prompt);
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_phone(buf))
            printf("  Invalid phone. Digits only, optional leading '+', max 13 characters.\n");
        else {
            strncpy(dest, buf, PHONE_LEN - 1);
            dest[PHONE_LEN - 1] = '\0';
            break;
        }
    }
}

/* Search input: read and validate an ID for lookup */
static bool input_search_id(char *dest)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Letters and digits only | max 9 chars ]");
    printf("  Student ID: ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    if (!validate_id(buf)) {
        printf("  Invalid ID format. Use letters and digits only (max 9 chars).\n\n");
        return false;
    }
    strncpy(dest, buf, ID_LEN - 1);
    dest[ID_LEN - 1] = '\0';
    return true;
}

/* Search input: read and validate a name for lookup */
static bool input_search_name(char *dest)
{
    char buf[INPUT_BUF];
    printf("  %s\n", "  [ Letters, spaces, and hyphens only | max 29 chars ]");
    printf("  Student Name: ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    if (!validate_name(buf)) {
        printf("  Invalid name format. Use letters, spaces, and hyphens only (max 29 chars).\n\n");
        return false;
    }
    strncpy(dest, buf, NAME_LEN - 1);
    dest[NAME_LEN - 1] = '\0';
    return true;
}

/* ── Internal list printer (no section header) ─────────── */
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

/* ═══════════════════════════════════════════════════════
   MAIN MENU
═══════════════════════════════════════════════════════ */
void MAIN_MENU(void)
{
    /* Welcome banner */
    printf("\n");
    printf("  %s\n", BOX_BORDER);
    printf("  |  %-61s|\n", WELCOME_TITLE);
    printf("  |  %-61s|\n", WELCOME_SUBTITLE);
    printf("  %s\n", BOX_BORDER);
    printf("  |  %-61s|\n", WELCOME_L1);
    printf("  |  %-61s|\n", WELCOME_L2);
    printf("  |  %-61s|\n", WELCOME_L3);
    printf("  |  %-61s|\n", "");
    printf("  |  %-61s|\n", WELCOME_L4);
    printf("  %s\n\n", BOX_BORDER);

    School Q;
    char   choice;
    char   str[ID_LEN];

    CreateSchool(&Q);

    enum {
        APPEND         = '1',
        EDIT           = '2',
        SEARCH_ID      = '3',
        SEARCH_NAME    = '4',
        LIST           = '5',
        SIZE           = '6',
        DELETE         = '7',
        PRINT          = '8',
        EDIT_SCORE_    = '9',
        PRINT_SCORE_   = 'A',
        RANK_STUDENT_  = 'B',
        EXIT           = 'C',
        PRINT_SCORE_lower  = 'a',
        RANK_STUDENT_lower = 'b',
        EXIT_lower         = 'c',
    };

    do {
        printf("  %s\n", BOX_BORDER);
        printf("  |  %-61s|\n", "MAIN MENU");
        printf("  %s\n", BOX_BORDER);
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
        printf("  %s\n", BOX_BORDER);
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
                if (input_search_id(str))
                    STUDENT_EDIT(&Q, str);
                break;

            case SEARCH_ID:
                print_section("FIND STUDENT BY ID");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_id(str))
                    STUDENT_SEARCH_ID(&Q, str);
                print_end();
                break;

            case SEARCH_NAME:
                print_section("FIND STUDENT BY NAME");
                if (isEmpty(&Q)) { printf("  Database is empty.\n"); print_end(); break; }
                if (input_search_name(str))
                    STUDENT_SEARCH_NAME(&Q, str);
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
                if (input_search_id(str))
                    DELETE_STUDENT(&Q, str);
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

/* ═══════════════════════════════════════════════════════
   CreateSchool
═══════════════════════════════════════════════════════ */
void CreateSchool(School *pschool)
{
    pschool->front = NULL;
    pschool->rear  = NULL;
    pschool->size  = 0;
}

/* ═══════════════════════════════════════════════════════
   isEmpty
═══════════════════════════════════════════════════════ */
bool isEmpty(School *pschool)
{
    return (pschool->front == NULL);
}

/* ═══════════════════════════════════════════════════════
   NEW_STUDENT
═══════════════════════════════════════════════════════ */
void NEW_STUDENT(School *pschool)
{
    Student *pstudent = (Student *)malloc(sizeof(Student));
    if (pstudent == NULL) {
        printf("  Error: insufficient memory.\n\n");
        return;
    }

    print_section("ADD NEW STUDENT RECORD");

    /* Name — unique */
    while (1) {
        input_name(pstudent->name, "Name          : ");
        if (STUDENT_SEARCH_NAME_SILENT(pschool, pstudent->name))
            printf("  A student with this name already exists. Please enter a different name.\n");
        else
            break;
    }

    /* Date of birth */
    input_date(pstudent->birth, "Date of Birth : ");

    /* ID — unique */
    while (1) {
        input_id(pstudent->ID, "ID            : ");
        if (STUDENT_SEARCH_ID_SILENT(pschool, pstudent->ID))
            printf("  This ID is already in use. Please enter a different ID.\n");
        else
            break;
    }

    /* Address */
    input_address(pstudent->address, "Address       : ");

    /* Guardian's phone */
    input_phone(pstudent->phone, "Guardian Ph.  : ");

    /* Initialise scores and pointer */
    pstudent->com_sc  = 0;
    pstudent->ar_sc   = 0;
    pstudent->eng_sc  = 0;
    pstudent->math_sc = 0;
    pstudent->pNext   = NULL;

    /* Enqueue */
    if (isEmpty(pschool)) {
        pschool->front = pstudent;
        pschool->rear  = pstudent;
    } else {
        pschool->rear->pNext = pstudent;
        pschool->rear        = pstudent;
    }
    (pschool->size)++;

    printf("\n  Student record created successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════
   STUDENT_EDIT
═══════════════════════════════════════════════════════ */
void STUDENT_EDIT(School *pschool, char id[ID_LEN])
{
    char tmp_name[NAME_LEN];
    char tmp_id  [ID_LEN];

    Student *temp = STUDENT_SEARCH_ID(pschool, id);
    if (temp == NULL) { print_end(); return; }

    printf("\n  Enter new details:\n\n");

    /* Name — unique (excluding current record) */
    while (1) {
        input_name(tmp_name, "Name          : ");
        strcpy(temp->name, "");                           /* clear to avoid self-match */
        if (STUDENT_SEARCH_NAME_SILENT(pschool, tmp_name))
            printf("  A student with this name already exists. Please enter a different name.\n");
        else {
            strncpy(temp->name, tmp_name, NAME_LEN - 1);
            break;
        }
    }

    input_date(temp->birth, "Date of Birth : ");

    /* ID — unique (excluding current record) */
    while (1) {
        input_id(tmp_id, "ID            : ");
        strcpy(temp->ID, "");                             /* clear to avoid self-match */
        if (STUDENT_SEARCH_ID_SILENT(pschool, tmp_id))
            printf("  This ID is already in use. Please enter a different ID.\n");
        else {
            strncpy(temp->ID, tmp_id, ID_LEN - 1);
            break;
        }
    }

    input_address(temp->address, "Address       : ");
    input_phone  (temp->phone,   "Guardian Ph.  : ");

    printf("\n  Student record updated successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════
   STUDENT_SEARCH_ID
═══════════════════════════════════════════════════════ */
Student *STUDENT_SEARCH_ID(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return NULL;
    }
    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->ID, id)) {
            printf("  Student found:\n\n");
            PRINT_STUDENT(temp);
            return temp;
        }
        temp = temp->pNext;
    }
    printf("  No student found with ID: %s\n\n", id);
    return NULL;
}

/* ═══════════════════════════════════════════════════════
   STUDENT_SEARCH_NAME
═══════════════════════════════════════════════════════ */
Student *STUDENT_SEARCH_NAME(School *pschool, char name[NAME_LEN])
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return NULL;
    }
    Student *temp = pschool->front;
    while (temp != NULL) {
        if (!stricmp(temp->name, name)) {
            printf("  Student found:\n\n");
            PRINT_STUDENT(temp);
            return temp;
        }
        temp = temp->pNext;
    }
    printf("  No student found with name: %s\n\n", name);
    return NULL;
}

/* ═══════════════════════════════════════════════════════
   DELETE_STUDENT
═══════════════════════════════════════════════════════ */
void DELETE_STUDENT(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }

    /* Single-element case */
    if (pschool->front == pschool->rear && !stricmp(pschool->front->ID, id)) {
        printf("  Removing:\n\n");
        PRINT_STUDENT(pschool->front);
        free(pschool->front);
        pschool->front = NULL;
        pschool->rear  = NULL;
        (pschool->size)--;
        printf("  Student record removed successfully.\n");
        print_end();
        return;
    }

    Student *cur  = pschool->front;
    Student *prev = NULL;
    while (cur != NULL && stricmp(cur->ID, id))
    {
        prev = cur;
        cur  = cur->pNext;
    }

    if (cur == NULL) {
        printf("  No student found with ID: %s\n", id);
        print_end();
        return;
    }

    printf("  Removing:\n\n");
    PRINT_STUDENT(cur);

    if      (pschool->front == cur) { pschool->front = cur->pNext; }
    else if (pschool->rear  == cur) { prev->pNext = NULL; pschool->rear = prev; }
    else                            { prev->pNext = cur->pNext; }

    free(cur);
    (pschool->size)--;
    printf("  Student record removed successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════
   STUDENT_LIST  (sort A-Z then display)
═══════════════════════════════════════════════════════ */
void STUDENT_LIST(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }

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
        cur  = pschool->front->pNext;
        prev = pschool->front;
        flag = 0;
        while (cur != NULL) {
            if (stricmp(prev->name, cur->name) > 0) {
                flag = 1;
                prev->pNext = cur->pNext;
                cur->pNext  = cur;
                *holder = *prev;
                *prev   = *cur;
                *cur    = *holder;
            }
            prev = cur;
            cur  = cur->pNext;
        }
        if (flag == 0) break;
    }
    free(holder);

    print_students_raw(pschool);
    print_end();
}

/* ═══════════════════════════════════════════════════════
   PRINT_STUDENT
═══════════════════════════════════════════════════════ */
void PRINT_STUDENT(Student *pstudent)
{
    printf("%s\n", CARD_BORDER);
    card_row_str("Name",        pstudent->name);
    card_row_str("ID",          pstudent->ID);
    card_row_str("Date of Birth", pstudent->birth);
    card_row_str("Address",     pstudent->address);
    card_row_str("Guardian Ph.", pstudent->phone);
    printf("%s\n\n", CARD_BORDER);
}

/* ═══════════════════════════════════════════════════════
   PRINT_SCHOOL
═══════════════════════════════════════════════════════ */
void PRINT_SCHOOL(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }
    print_section("ALL STUDENT RECORDS");
    print_students_raw(pschool);
    print_end();
}

/* ═══════════════════════════════════════════════════════
   STUDENT_SCORE
═══════════════════════════════════════════════════════ */
void STUDENT_SCORE(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }
    print_section("EDIT ALL STUDENTS' SCORES");

    Student *temp = pschool->front;
    while (temp != NULL) {
        printf("  Student : %s  (ID: %s)\n", temp->name, temp->ID);
        printf("    Comp. Science : "); scanf("%d", &temp->com_sc);  clear_stdin();
        printf("    Arabic        : "); scanf("%d", &temp->ar_sc);   clear_stdin();
        printf("    English       : "); scanf("%d", &temp->eng_sc);  clear_stdin();
        printf("    Math          : "); scanf("%d", &temp->math_sc); clear_stdin();
        printf("\n");
        temp = temp->pNext;
    }
    printf("  All scores updated successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════
   PRINT_STUDENT_SCORE
═══════════════════════════════════════════════════════ */
void PRINT_STUDENT_SCORE(Student *pstudent)
{
    printf("%s\n", CARD_BORDER);
    card_row_str("Name",          pstudent->name);
    card_row_str("ID",            pstudent->ID);
    card_row_int("Comp. Science", pstudent->com_sc);
    card_row_int("Arabic",        pstudent->ar_sc);
    card_row_int("English",       pstudent->eng_sc);
    card_row_int("Math",          pstudent->math_sc);
    printf("%s\n\n", CARD_BORDER);
}

/* ═══════════════════════════════════════════════════════
   PRINT_SCHOOL_SCORE
═══════════════════════════════════════════════════════ */
void PRINT_SCHOOL_SCORE(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }
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

/* ═══════════════════════════════════════════════════════
   RANK_STUDENT  (sort by Comp. Sc. descending)
═══════════════════════════════════════════════════════ */
void RANK_STUDENT(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }

    print_section("RANK BY COMP. SC. SCORE");

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
        cur  = pschool->front->pNext;
        prev = pschool->front;
        flag = 0;
        while (cur != NULL) {
            if (prev->com_sc < cur->com_sc) {
                flag = 1;
                prev->pNext = cur->pNext;
                cur->pNext  = cur;
                *holder = *prev;
                *prev   = *cur;
                *cur    = *holder;
            }
            prev = cur;
            cur  = cur->pNext;
        }
        if (flag == 0) break;
    }
    free(holder);
    printf("  Students ranked by Computer Science score (highest first).\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════
   SchoolSize
═══════════════════════════════════════════════════════ */
void SchoolSize(School *pschool)
{
    if (isEmpty(pschool)) {
        printf("  Database is empty.\n\n");
        return;
    }
    printf("  Total enrolled students: %d\n\n", pschool->size);
}

/* ═══════════════════════════════════════════════════════
   STUDENT_SEARCH_ID_SILENT
═══════════════════════════════════════════════════════ */
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

/* ═══════════════════════════════════════════════════════
   STUDENT_SEARCH_NAME_SILENT
═══════════════════════════════════════════════════════ */
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
