#include "school.h"

/* ═══════════════════════════════════════════════════════════════════
   TESTING MODE
   ─────────────────────────────────────────────────────────────────
   Compile with -DTESTING to replace getche() with a stdin-compatible
   alternative. This allows automated testing via input redirection:
     gcc src/main.c src/school.c -DTESTING -o test_build.exe
     test_build.exe < tests/input/test_01.txt > tests/output/test_01.txt
   In normal mode READ_CHAR() calls getche() as usual.
═══════════════════════════════════════════════════════════════════ */
#ifdef TESTING
/* Read one character from stdin and consume the following newline. */
static char testing_read_char(void)
{
    char c = (char)getchar();
    int  n = getchar();
    if (n != '\n' && n != EOF) ungetc(n, stdin);
    return c;
}
#define READ_CHAR() testing_read_char()
#else
#define READ_CHAR() getche()
#endif

/* ═══════════════════════════════════════════════════════════════════
   TABLE FORMAT MACROS
   ─────────────────────────────────────────────────────────────────
   All table widths are fixed to prevent misaligned output.

   Profile table — 81 chars:
     No(4) | Name(22) | ID(10) | Date of Birth(12) | Guardian Phone(15)

   Score table — 93 chars:
     No(4) | Name(18) | ID(9) | C.Sc(5) | Sci(5) | Eng(5) |
     Math(5) | Hist(5) | Total(7)
     Score cell format : "%3d %c"   → e.g. " 85 A"  (5 chars)
     Total cell format : "%5.1f %c" → e.g. " 86.6 B" (7 chars)

   Score detail card — 38 chars:
     Subject(16) | Score(7) | Grade(7)
═══════════════════════════════════════════════════════════════════ */

/* Profile table */
#define TP_BORDER \
    "  +------+------------------------+------------+--------------+-----------------+"
#define TP_HEAD \
    "  | %-4s | %-22s | %-10s | %-12s | %-15s |\n"
#define TP_ROW \
    "  | %4d | %-22.22s | %-10.10s | %-12s | %-15s |\n"

/* Score table */
#define TS_BORDER \
    "  +------+--------------------+-----------+-------+-------+-------+-------+-------+---------+"
#define TS_HEAD \
    "  | %-4s | %-18s | %-9s | %-5s | %-5s | %-5s | %-5s | %-5s | %-7s |\n"
#define TS_ROW \
    "  | %4d | %-18.18s | %-9.9s | %3d %c | %3d %c | %3d %c | %3d %c | %3d %c | %5.1f %c |\n"

/* Score detail card (single-student lookup)
   Border: "  +" + 18 dashes + "+" + 7 dashes + "+" + 7 dashes + "+" = 38 chars */
#define TD_BORDER  "  +------------------+-------+-------+"
#define TD_SUBJ    "  | %-16s | %5d |   %c   |\n"
#define TD_TOTAL   "  | %-16s | %5.1f |   %c   |\n"

/* ═══════════════════════════════════════════════════════════════════
   INTERNAL HELPERS
═══════════════════════════════════════════════════════════════════ */

/* Strip the trailing '\n' that fgets stores. */
static void trim_newline(char *s)
{
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

/* Remove leading and trailing space characters in-place. */
static void trim_spaces(char *s)
{
    int end = (int)strlen(s) - 1;
    while (end >= 0 && s[end] == ' ') s[end--] = '\0';
    int start = 0;
    while (s[start] == ' ') start++;
    if (start > 0) memmove(s, s + start, strlen(s + start) + 1);
}

/* Discard remaining input in stdin up to and including newline. */
static void clear_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * Print a section header that aligns with BOX_BORDER (67 chars):
 *   +--- TITLE -----...-----+
 * Trailing dashes fill the remaining space to position 67.
 */
static void print_section(const char *title)
{
    int pad = 58 - (int)strlen(title);
    if (pad < 1) pad = 1;
    printf("\n  +--- %s ", title);
    for (int i = 0; i < pad; i++) putchar('-');
    puts("+");
}

/* Closing border with blank line. */
static void print_end(void)
{
    printf("%s\n\n", BOX_BORDER);
}

/* ── GPA helpers ─────────────────────────────────────────────────── */

/* American letter-grade system: A ≥ 90, B ≥ 80, C ≥ 70, D ≥ 60, F < 60. */
static char calc_gpa(int score)
{
    if (score >= 90) return 'A';
    if (score >= 80) return 'B';
    if (score >= 70) return 'C';
    if (score >= 60) return 'D';
    return 'F';
}

/* Recompute and store all derived grade fields after any score change. */
static void update_gpa(Student *s)
{
    s->com_gpa   = calc_gpa(s->com_sc);
    s->sci_gpa   = calc_gpa(s->sci_sc);
    s->eng_gpa   = calc_gpa(s->eng_sc);
    s->math_gpa  = calc_gpa(s->math_sc);
    s->hist_gpa  = calc_gpa(s->hist_sc);
    s->total_sc  = (s->com_sc + s->sci_sc + s->eng_sc +
                    s->math_sc + s->hist_sc) / 5.0f;
    s->total_gpa = calc_gpa((int)(s->total_sc + 0.5f));
}

/* ── Calendar helpers ────────────────────────────────────────────── */

static bool is_leap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int days_in_month(int m, int y)
{
    int d[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (m == 2 && is_leap(y)) ? 29 : d[m];
}

/* ═══════════════════════════════════════════════════════════════════
   INPUT VALIDATORS
═══════════════════════════════════════════════════════════════════ */

/*
 * NAME RULES
 * ─────────────────────────────────────────────────────────────────
 * Allowed characters : A-Z  a-z  space  hyphen (-)
 * Must start and end with a letter.
 * Must contain at least one space → minimum two words.
 * Each word or hyphen-separated part must be ≥ 2 characters.
 * A hyphen must be preceded and followed by a letter.
 * No consecutive spaces (double space is rejected).
 * No character (case-insensitive) repeated more than twice in a row,
 *   e.g. "Moohamed" is rejected; "Mohammed" is accepted.
 * Overall length (after trimming): 5 to NAME_LEN-1.
 */
static bool validate_name(const char *s)
{
    int len = (int)strlen(s);
    if (len < 5 || len >= NAME_LEN)               return false;
    if (!isalpha((unsigned char)s[0]))             return false;
    if (!isalpha((unsigned char)s[len - 1]))       return false;

    bool has_space = false;
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalpha(c) && c != ' ' && c != '-')  return false;
        if (c == ' ' && s[i + 1] == ' ')           return false; /* no double space */
        if (c == '-') {
            if (i == 0 || !isalpha((unsigned char)s[i - 1])) return false;
            if (i + 1 >= len || !isalpha((unsigned char)s[i + 1])) return false;
        }
        if (c == ' ') has_space = true;
        /* no three identical consecutive characters (case-insensitive) */
        if (i >= 2 &&
            tolower(c) == tolower((unsigned char)s[i - 1]) &&
            tolower(c) == tolower((unsigned char)s[i - 2])) return false;
    }
    if (!has_space) return false;

    /* each word/part must be ≥ 2 characters */
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
 * ID RULES
 * Allowed  : A-Z  a-z  0-9  (no spaces or symbols)
 * Length   : 4 to ID_LEN-1
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
 * DATE OF BIRTH RULES
 * Format   : DD/MM/YYYY
 * Calendar : leap-year-aware; correct days per month enforced.
 * Age      : student must be 14 to 21 years old as of today.
 */
static bool validate_date(const char *s)
{
    if ((int)strlen(s) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) { if (s[i] != '/')                  return false; }
        else                  { if (!isdigit((unsigned char)s[i])) return false; }
    }
    int day   = (s[0] - '0') * 10 + (s[1] - '0');
    int month = (s[3] - '0') * 10 + (s[4] - '0');
    int year  = (s[6] - '0') * 1000 + (s[7] - '0') * 100 +
                (s[8] - '0') * 10   + (s[9] - '0');

    if (month < 1 || month > 12)                      return false;
    if (day < 1 || day > days_in_month(month, year))  return false;

    time_t     t   = time(NULL);
    struct tm *now = localtime(&t);
    int cy = now->tm_year + 1900, cm = now->tm_mon + 1, cd = now->tm_mday;
    int age = cy - year;
    if (cm < month || (cm == month && cd < day)) age--;
    return (age >= 14 && age <= 21);
}

/*
 * ADDRESS RULES
 * ─────────────────────────────────────────────────────────────────
 * Allowed characters : A-Z  a-z  0-9  space  .  ,  -  /
 * Must not start or end with a space (trimmed by the collector).
 * No two consecutive identical non-alphanumeric characters:
 *   "//"  ".."  ",,"  "--"  and double space are all rejected.
 * Length (after trimming): 10 to ADDRESS_LEN-1.
 */
static bool validate_address(const char *s)
{
    int len = (int)strlen(s);
    if (len < 10 || len >= ADDRESS_LEN)  return false;
    if (s[0] == ' ' || s[len - 1] == ' ') return false;
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != ' ' && c != ',' &&
            c != '.'    && c != '-' && c != '/')  return false;
        /* no two consecutive identical non-alphanumeric characters */
        if (i > 0 && !isalnum(c) && c == (unsigned char)s[i - 1]) return false;
    }
    return true;
}

/*
 * PHONE RULES
 * Allowed  : 0-9 (digits only — use 00 prefix for international)
 * Length   : 6 to 15 digits
 */
static bool validate_phone(const char *s)
{
    int len = (int)strlen(s);
    if (len < 6 || len > 15) return false;
    for (int i = 0; s[i]; i++)
        if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

/* ═══════════════════════════════════════════════════════════════════
   INPUT COLLECTORS
═══════════════════════════════════════════════════════════════════ */

static void input_name(char *dest)
{
    char buf[INPUT_BUF];
    printf("\n  Name\n");
    printf("    Allowed : A-Z   a-z   space   -\n");
    printf("    Rules   : Minimum two words  |  Each word min 2 chars\n");
    printf("              Hyphen ( - ) must be between two letters\n");
    printf("              No character repeated more than twice in a row\n");
    printf("    Length  : 5 to 29 characters\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        trim_spaces(buf);
        if (!validate_name(buf))
            printf("    Invalid. Please follow the rules above.\n");
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
    printf("    Allowed : A-Z   a-z   0-9   ( no spaces or symbols )\n");
    printf("    Length  : 4 to 9 characters\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_id(buf))
            printf("    Invalid. Please follow the rules above.\n");
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
    printf("    Format  : DD/MM/YYYY   e.g. 14/03/2005\n");
    printf("    Note    : Student must be 14 to 21 years old\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_date(buf))
            printf("    Invalid. Check the format, calendar date,"
                   " and age range (14-21).\n");
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
    printf("    Allowed : A-Z   a-z   0-9   space   .   ,   -   /\n");
    printf("    Rules   : No consecutive identical symbols"
           "   e.g. // or ..\n");
    printf("    Length  : 10 to 39 characters\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        trim_spaces(buf);
        if (!validate_address(buf))
            printf("    Invalid. Please follow the rules above.\n");
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
    printf("    Allowed : 0-9   ( digits only )\n");
    printf("    Note    : Use 00 prefix for international"
           "   e.g. 0020100123456\n");
    printf("    Length  : 6 to 15 digits\n");
    while (1) {
        printf("    > ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (!validate_phone(buf))
            printf("    Invalid. Digits only, 6 to 15 characters.\n");
        else {
            strncpy(dest, buf, PHONE_LEN - 1);
            dest[PHONE_LEN - 1] = '\0';
            break;
        }
    }
}

/* Read and validate one score (0-100); retries on bad input. */
static int input_score(const char *subject)
{
    int score;
    printf("    %-16s: ", subject);
    while (1) {
        if (scanf("%d", &score) == 1 && score >= 0 && score <= 100) {
            clear_stdin();
            return score;
        }
        clear_stdin();
        printf("      Invalid. Enter a whole number between 0 and 100.\n");
        printf("    %-16s: ", subject);
    }
}

/*
 * Search-mode input helpers: validate format then copy to dest.
 * Return false if format is wrong so the caller can prompt again.
 */
static bool input_search_id(char *dest)
{
    char buf[INPUT_BUF];
    printf("    Allowed : A-Z   a-z   0-9   |  Length: 4 to 9 chars\n");
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
    printf("    Allowed : A-Z   a-z   space   -   |  Min two words\n");
    printf("    > ");
    fgets(buf, sizeof(buf), stdin);
    trim_newline(buf);
    trim_spaces(buf);
    if (!validate_name(buf)) {
        printf("  Invalid name format.\n\n");
        return false;
    }
    strncpy(dest, buf, NAME_LEN - 1);
    dest[NAME_LEN - 1] = '\0';
    return true;
}

/* ═══════════════════════════════════════════════════════════════════
   POINTER ARRAY HELPERS
   ─────────────────────────────────────────────────────────────────
   A temporary array of Student pointers is built for sorted display.
   The linked list itself is never reordered by display operations,
   preserving insertion order at all times.
   The caller is responsible for calling free() on the returned array.
═══════════════════════════════════════════════════════════════════ */

static Student **build_ptr_array(School *pschool)
{
    Student **arr = (Student **)malloc(pschool->size * sizeof(Student *));
    if (arr == NULL) return NULL;
    Student *t = pschool->front;
    for (int i = 0; i < pschool->size; i++) { arr[i] = t; t = t->pNext; }
    return arr;
}

/* Bubble sort pointer array alphabetically by name (A-Z). */
static void sort_arr_by_name(Student **arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (stricmp(arr[j]->name, arr[j + 1]->name) > 0) {
                Student *tmp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* Bubble sort pointer array by total_sc descending (highest first). */
static void sort_arr_by_score(Student **arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j]->total_sc < arr[j + 1]->total_sc) {
                Student *tmp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

/* ═══════════════════════════════════════════════════════════════════
   TABLE PRINTERS
═══════════════════════════════════════════════════════════════════ */

static void print_profile_table(Student **arr, int n)
{
    puts(TP_BORDER);
    printf(TP_HEAD, "No.", "Name", "ID", "Date of Birth", "Guardian Phone");
    puts(TP_BORDER);
    for (int i = 0; i < n; i++)
        printf(TP_ROW, i + 1,
               arr[i]->name, arr[i]->ID, arr[i]->birth, arr[i]->phone);
    puts(TP_BORDER);
    printf("  Note: Address not shown in table view."
           " Use option 2 for the full profile.\n");
}

static void print_score_table(Student **arr, int n)
{
    puts(TS_BORDER);
    printf(TS_HEAD, "No.", "Name", "ID",
           "C.Sc", "Sci", "Eng", "Math", "Hist", "Total");
    puts(TS_BORDER);
    for (int i = 0; i < n; i++) {
        Student *s = arr[i];
        printf(TS_ROW, i + 1,
               s->name, s->ID,
               s->com_sc,  s->com_gpa,
               s->sci_sc,  s->sci_gpa,
               s->eng_sc,  s->eng_gpa,
               s->math_sc, s->math_gpa,
               s->hist_sc, s->hist_gpa,
               s->total_sc, s->total_gpa);
    }
    puts(TS_BORDER);
}

/* Print the score detail card for a single student. */
static void print_score_detail(Student *s)
{
    printf("%s\n", BOX_BORDER);
    printf("  |  %-14s: %-45s|\n", "Name", s->name);
    printf("  |  %-14s: %-45s|\n", "ID",   s->ID);
    printf("%s\n", BOX_BORDER);
    puts(TD_BORDER);
    printf("  | %-16s | Score |  GPA  |\n", "Subject");
    puts(TD_BORDER);
    printf(TD_SUBJ,  "Comp. Science", s->com_sc,  s->com_gpa);
    printf(TD_SUBJ,  "Science",       s->sci_sc,  s->sci_gpa);
    printf(TD_SUBJ,  "English",       s->eng_sc,  s->eng_gpa);
    printf(TD_SUBJ,  "Math",          s->math_sc, s->math_gpa);
    printf(TD_SUBJ,  "History",       s->hist_sc, s->hist_gpa);
    puts(TD_BORDER);
    printf(TD_TOTAL, "Total Score",   s->total_sc, s->total_gpa);
    puts(TD_BORDER);
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
   INTERNAL FLOW HELPERS
═══════════════════════════════════════════════════════════════════ */

/* Edit all profile fields of an already-located student. */
static void do_edit_profile(School *pschool, Student *t)
{
    char tmp_name[NAME_LEN], tmp_id[ID_LEN];
    printf("\n  Enter new details:\n");

    while (1) {
        input_name(tmp_name);
        strcpy(t->name, "");            /* clear to avoid self-match in search */
        if (STUDENT_SEARCH_NAME_SILENT(pschool, tmp_name))
            printf("    A student with this name already exists."
                   " Enter a different name.\n");
        else { strncpy(t->name, tmp_name, NAME_LEN - 1); break; }
    }

    input_date(t->birth);

    while (1) {
        input_id(tmp_id);
        strcpy(t->ID, "");              /* clear to avoid self-match in search */
        if (STUDENT_SEARCH_ID_SILENT(pschool, tmp_id))
            printf("    This ID is already in use."
                   " Enter a different ID.\n");
        else { strncpy(t->ID, tmp_id, ID_LEN - 1); break; }
    }

    input_address(t->address);
    input_phone(t->phone);
}

/*
 * Edit scores for a single student.
 * mode 'A' — all five subjects in sequence.
 * mode 'S' — user selects one subject from a numbered list.
 * GPA is recomputed on exit.
 */
static void do_edit_scores(Student *t, char mode)
{
    if (mode == 'A') {
        printf("\n  Enter new scores (0 to 100):\n\n");
        t->com_sc  = input_score("Comp. Science");
        t->sci_sc  = input_score("Science");
        t->eng_sc  = input_score("English");
        t->math_sc = input_score("Math");
        t->hist_sc = input_score("History");
    } else {
        printf("\n  Select subject:\n");
        printf("    1 — Comp. Science   (current: %3d  %c)\n",
               t->com_sc,  t->com_gpa);
        printf("    2 — Science         (current: %3d  %c)\n",
               t->sci_sc,  t->sci_gpa);
        printf("    3 — English         (current: %3d  %c)\n",
               t->eng_sc,  t->eng_gpa);
        printf("    4 — Math            (current: %3d  %c)\n",
               t->math_sc, t->math_gpa);
        printf("    5 — History         (current: %3d  %c)\n",
               t->hist_sc, t->hist_gpa);
        printf("    > ");
        char sc = READ_CHAR(); printf("\n");
        printf("\n  New score (0 to 100):\n\n");
        switch (sc) {
            case '1': t->com_sc  = input_score("Comp. Science"); break;
            case '2': t->sci_sc  = input_score("Science");       break;
            case '3': t->eng_sc  = input_score("English");       break;
            case '4': t->math_sc = input_score("Math");          break;
            case '5': t->hist_sc = input_score("History");       break;
            default:  printf("  Invalid selection. No changes made.\n"); return;
        }
    }
    update_gpa(t);
}

/* Find a student in the list by method ('N'=name, 'I'=ID). No output. */
static Student *find_by_method(School *pschool, char method, const char *term)
{
    Student *t = pschool->front;
    while (t != NULL) {
        if (method == 'N') { if (!stricmp(t->name, term)) return t; }
        else               { if (!stricmp(t->ID,   term)) return t; }
        t = t->pNext;
    }
    return NULL;
}

/* ─────────────────────────────────────────────────────────────────
   OPTION 2: Student Profile Lookup, Edit & Remove
   ─────────────────────────────────────────────────────────────────
   Flow:
     choose_method → do_search → [not found] → next_method
                               → [found]     → show_actions
                                               E → do_edit_profile → after_edit
                                               R → confirm_remove
                                               B → choose_method
   goto is used for menu-state navigation — a standard pattern in
   console C programs.
──────────────────────────────────────────────────────────────────*/
static void flow_lookup_edit_remove(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }
    print_section("STUDENT PROFILE LOOKUP, EDIT & REMOVE");

    char method = '\0';
    char term[INPUT_BUF];
    Student *found = NULL;

choose_method:
    printf("\n  Search by:\n");
    printf("    N — Name     I — ID     B — Back to Main Menu\n");
    printf("    > ");
    method = toupper(READ_CHAR()); printf("\n");
    if (method == 'B') { print_end(); return; }
    if (method != 'N' && method != 'I') {
        printf("  Enter N, I, or B.\n");
        goto choose_method;
    }

do_search:
    printf("\n  Enter student %s:\n", method == 'N' ? "name" : "ID");
    if (method == 'N') { if (!input_search_name(term)) goto choose_method; }
    else               { if (!input_search_id(term))   goto choose_method; }

    found = find_by_method(pschool, method, term);

    if (found == NULL) {
        printf("\n  No student found with %s: %s\n",
               method == 'N' ? "name" : "ID", term);
        printf("\n  N — by Name     I — by ID     B — Back\n");
        printf("    > ");
        char nx = toupper(READ_CHAR()); printf("\n");
        if (nx == 'B')                  { print_end(); return; }
        if (nx == 'N' || nx == 'I') method = nx;
        goto do_search;
    }

    printf("\n  Student found:\n\n");
    PRINT_STUDENT(found);

show_actions:
    printf("    E — Edit   R — Remove   B — Back\n");
    printf("    > ");
    char act = toupper(READ_CHAR()); printf("\n");

    if (act == 'B') goto choose_method;

    if (act == 'E') {
        do_edit_profile(pschool, found);
        printf("\n  Updated profile:\n\n");
        PRINT_STUDENT(found);
        printf("    %s — Search again   B — Back\n",
               method == 'N' ? "N" : "I");
        printf("    > ");
        char ae = toupper(READ_CHAR()); printf("\n");
        if (ae == 'B')              { print_end(); return; }
        if (ae == 'N' || ae == 'I') method = ae;
        goto do_search;
    }

    if (act == 'R') {
        printf("\n  Confirm removal of: %s\n", found->name);
        printf("    Y — Yes, remove     N — No, go back\n");
        printf("    > ");
        char conf = toupper(READ_CHAR()); printf("\n");
        if (conf == 'N') { printf("\n"); goto show_actions; }
        if (conf == 'Y') {
            char saved_id[ID_LEN];
            strncpy(saved_id, found->ID, ID_LEN - 1);
            saved_id[ID_LEN - 1] = '\0';
            DELETE_STUDENT(pschool, saved_id);
            printf("  Search for another student?\n");
            printf("    N — by Name     I — by ID     B — Back\n");
            printf("    > ");
            char ns = toupper(READ_CHAR()); printf("\n");
            if (ns == 'B')              { print_end(); return; }
            if (ns == 'N' || ns == 'I') method = ns;
            goto do_search;
        }
        goto show_actions;
    }
    printf("  Enter E, R, or B.\n");
    goto show_actions;
}

/* ─────────────────────────────────────────────────────────────────
   OPTION 5: Student Scores Lookup & Edit
──────────────────────────────────────────────────────────────────*/
static void flow_scores_lookup_edit(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }
    print_section("STUDENT SCORES LOOKUP & EDIT");

    char method = '\0';
    char term[INPUT_BUF];
    Student *found = NULL;

sc_method:
    printf("\n  Search by:\n");
    printf("    N — Name     I — ID     B — Back to Main Menu\n");
    printf("    > ");
    method = toupper(READ_CHAR()); printf("\n");
    if (method == 'B') { print_end(); return; }
    if (method != 'N' && method != 'I') {
        printf("  Enter N, I, or B.\n");
        goto sc_method;
    }

sc_search:
    printf("\n  Enter student %s:\n", method == 'N' ? "name" : "ID");
    if (method == 'N') { if (!input_search_name(term)) goto sc_method; }
    else               { if (!input_search_id(term))   goto sc_method; }

    found = find_by_method(pschool, method, term);

    if (found == NULL) {
        printf("\n  No student found with %s: %s\n",
               method == 'N' ? "name" : "ID", term);
        printf("\n  N — by Name     I — by ID     B — Back\n");
        printf("    > ");
        char nx = toupper(READ_CHAR()); printf("\n");
        if (nx == 'B')              { print_end(); return; }
        if (nx == 'N' || nx == 'I') method = nx;
        goto sc_search;
    }

    print_score_detail(found);

sc_actions:
    printf("    E — Edit Scores     N — Search Again     B — Back\n");
    printf("    > ");
    char act = toupper(READ_CHAR()); printf("\n");

    if (act == 'B') goto sc_method;
    if (act == 'N') goto sc_search;

    if (act == 'E') {
        printf("\n  Edit mode:\n");
        printf("    A — All subjects     S — Specific subject\n");
        printf("    > ");
        char mode = toupper(READ_CHAR()); printf("\n");
        if (mode != 'A' && mode != 'S') {
            printf("  Enter A or S.\n");
            goto sc_actions;
        }
        do_edit_scores(found, mode);
        printf("\n  Updated scores:\n\n");
        print_score_detail(found);
        printf("    N — Search Again     B — Back\n");
        printf("    > ");
        char ae = toupper(READ_CHAR()); printf("\n");
        if (ae == 'B') { print_end(); return; }
        if (ae == 'N' || ae == 'I') method = ae;
        goto sc_search;
    }
    printf("  Enter E, N, or B.\n");
    goto sc_actions;
}

/* ═══════════════════════════════════════════════════════════════════
   MAIN MENU
═══════════════════════════════════════════════════════════════════ */
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
    CreateSchool(&Q);

    enum {
        APPEND          = '1',
        LOOKUP_EDIT_REM = '2',
        VIEW_PROFILES   = '3',
        LIST_PROFILES   = '4',
        SCORES_LOOKUP   = '5',
        EDIT_ALL_SCORES = '6',
        VIEW_SCORES     = '7',
        LIST_BY_SCORE   = '8',
        EXIT            = 'D',
        EXIT_lower      = 'd',
    };

    do {
        /* ── Menu header: MAIN MENU on left, enrolled count on right ── */
        char count_str[32];
        if      (Q.size == 0) sprintf(count_str, "No students enrolled");
        else if (Q.size == 1) sprintf(count_str, "1 student enrolled");
        else                  sprintf(count_str, "%d students enrolled", Q.size);

        /* Inner width = 63: "  MAIN MENU" = 11 chars, 1 space gap, count, = 63 */
        int spaces = 63 - 11 - 1 - (int)strlen(count_str);
        if (spaces < 1) spaces = 1;

        printf("%s\n", BOX_BORDER);
        printf("  |  MAIN MENU%*s%s |\n", spaces, "", count_str);
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
            case APPEND:
                NEW_STUDENT(&Q);
                break;
            case LOOKUP_EDIT_REM:
                flow_lookup_edit_remove(&Q);
                break;
            case VIEW_PROFILES:
                PRINT_SCHOOL(&Q);
                break;
            case LIST_PROFILES:
                STUDENT_LIST(&Q);
                break;
            case SCORES_LOOKUP:
                flow_scores_lookup_edit(&Q);
                break;
            case EDIT_ALL_SCORES:
                STUDENT_SCORE(&Q);
                break;
            case VIEW_SCORES:
                PRINT_SCHOOL_SCORE(&Q);
                break;
            case LIST_BY_SCORE:
                RANK_STUDENT(&Q);
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

/* ═══════════════════════════════════════════════════════════════════
   CreateSchool
═══════════════════════════════════════════════════════════════════ */
void CreateSchool(School *pschool)
{
    pschool->front = NULL;
    pschool->rear  = NULL;
    pschool->size  = 0;
}

/* ═══════════════════════════════════════════════════════════════════
   isEmpty
═══════════════════════════════════════════════════════════════════ */
bool isEmpty(School *pschool)
{
    return (pschool->front == NULL);
}

/* ═══════════════════════════════════════════════════════════════════
   NEW_STUDENT
═══════════════════════════════════════════════════════════════════ */
void NEW_STUDENT(School *pschool)
{
    Student *p = (Student *)malloc(sizeof(Student));
    if (p == NULL) { printf("  Error: insufficient memory.\n\n"); return; }

    print_section("ADD NEW STUDENT PROFILE");

    while (1) {
        input_name(p->name);
        if (STUDENT_SEARCH_NAME_SILENT(pschool, p->name))
            printf("    A student with this name already exists."
                   " Enter a different name.\n");
        else break;
    }

    input_date(p->birth);

    while (1) {
        input_id(p->ID);
        if (STUDENT_SEARCH_ID_SILENT(pschool, p->ID))
            printf("    This ID is already in use. Enter a different ID.\n");
        else break;
    }

    input_address(p->address);
    input_phone(p->phone);

    p->com_sc = p->sci_sc = p->eng_sc = p->math_sc = p->hist_sc = 0;
    update_gpa(p);
    p->pNext = NULL;

    if (isEmpty(pschool)) { pschool->front = pschool->rear = p; }
    else { pschool->rear->pNext = p; pschool->rear = p; }
    (pschool->size)++;

    printf("\n  Student profile created successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   DELETE_STUDENT
═══════════════════════════════════════════════════════════════════ */
void DELETE_STUDENT(School *pschool, char id[])
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    /* Single-student case */
    if (pschool->front == pschool->rear &&
        !stricmp(pschool->front->ID, id))
    {
        free(pschool->front);
        pschool->front = pschool->rear = NULL;
        (pschool->size)--;
        printf("  Student profile removed successfully.\n");
        print_end();
        return;
    }

    Student *cur = pschool->front, *prev = NULL;
    while (cur != NULL && stricmp(cur->ID, id)) { prev = cur; cur = cur->pNext; }

    if (cur == NULL) { printf("  No student found with ID: %s\n", id); return; }

    if      (pschool->front == cur) { pschool->front = cur->pNext; }
    else if (pschool->rear  == cur) { prev->pNext = NULL; pschool->rear = prev; }
    else                            { prev->pNext = cur->pNext; }

    free(cur);
    (pschool->size)--;
    printf("  Student profile removed successfully.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   PRINT_STUDENT  (full detail card — used in single-student context)
═══════════════════════════════════════════════════════════════════ */
void PRINT_STUDENT(Student *pstudent)
{
    printf("%s\n", BOX_BORDER);
    printf("  |  %-14s: %-45s|\n", "Name",           pstudent->name);
    printf("  |  %-14s: %-45s|\n", "ID",             pstudent->ID);
    printf("  |  %-14s: %-45s|\n", "Date of Birth",  pstudent->birth);
    printf("  |  %-14s: %-45s|\n", "Address",        pstudent->address);
    printf("  |  %-14s: %-45s|\n", "Guardian Phone", pstudent->phone);
    printf("%s\n\n", BOX_BORDER);
}

/* ═══════════════════════════════════════════════════════════════════
   PRINT_SCHOOL  (option 3 — insertion order, profile table)
═══════════════════════════════════════════════════════════════════ */
void PRINT_SCHOOL(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENT PROFILES  (Insertion Order)");

    Student **arr = build_ptr_array(pschool);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    printf("\n");
    print_profile_table(arr, pschool->size);
    free(arr);
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   STUDENT_LIST  (option 4 — A-Z, profile table)
═══════════════════════════════════════════════════════════════════ */
void STUDENT_LIST(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENT PROFILES  (A-Z)");

    Student **arr = build_ptr_array(pschool);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    sort_arr_by_name(arr, pschool->size);
    printf("\n");
    print_profile_table(arr, pschool->size);
    free(arr);
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   STUDENT_SCORE  (option 6 — edit all students' scores)
   ─────────────────────────────────────────────────────────────────
   User selects edit mode once (A = all subjects, S = one subject).
   For mode S, the subject is also chosen once and applied to every
   student in sequence. Per student: K skips, B stops and saves.
═══════════════════════════════════════════════════════════════════ */
void STUDENT_SCORE(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }
    print_section("EDIT ALL STUDENTS' SCORES");

    /* ── Choose mode ─────────────────────────────────────────── */
    char mode;
    char subj_choice = '\0';
    const char *subj_name = NULL;

choose_mode:
    printf("\n  Edit mode:\n");
    printf("    A — All subjects     S — Specific subject     B — Back\n");
    printf("    > ");
    mode = toupper(READ_CHAR()); printf("\n");
    if (mode == 'B') { print_end(); return; }
    if (mode != 'A' && mode != 'S') { printf("  Enter A, S, or B.\n"); goto choose_mode; }

    if (mode == 'S') {
        printf("\n  Select subject:\n");
        printf("    1 — Comp. Science\n");
        printf("    2 — Science\n");
        printf("    3 — English\n");
        printf("    4 — Math\n");
        printf("    5 — History\n");
        printf("    > ");
        subj_choice = READ_CHAR(); printf("\n");
        switch (subj_choice) {
            case '1': subj_name = "Comp. Science"; break;
            case '2': subj_name = "Science";       break;
            case '3': subj_name = "English";       break;
            case '4': subj_name = "Math";          break;
            case '5': subj_name = "History";       break;
            default:
                printf("  Invalid selection.\n");
                goto choose_mode;
        }
        printf("  Editing: %s   for all students.\n", subj_name);
    }

    printf("\n  Scores must be whole numbers between 0 and 100.\n");
    printf("  K — Skip student     B — Stop and save progress\n");

    /* ── Iterate students ────────────────────────────────────── */
    Student *t = pschool->front;
    int count = 0;

    while (t != NULL) {
        count++;
        char hdr[64];
        snprintf(hdr, sizeof(hdr), "Student %d of %d: %s",
                 count, pschool->size, t->name);
        printf("\n%s\n", BOX_BORDER);
        printf("  |  %-61s|\n", hdr);
        printf("  |  %-14s: %-45s|\n", "ID", t->ID);
        printf("%s\n", BOX_BORDER);

        /* Show current scores */
        puts(TD_BORDER);
        printf("  | %-16s | Score |  GPA  |\n", "Subject");
        puts(TD_BORDER);
        printf(TD_SUBJ,  "Comp. Science", t->com_sc,  t->com_gpa);
        printf(TD_SUBJ,  "Science",       t->sci_sc,  t->sci_gpa);
        printf(TD_SUBJ,  "English",       t->eng_sc,  t->eng_gpa);
        printf(TD_SUBJ,  "Math",          t->math_sc, t->math_gpa);
        printf(TD_SUBJ,  "History",       t->hist_sc, t->hist_gpa);
        puts(TD_BORDER);
        printf(TD_TOTAL, "Total Score",   t->total_sc, t->total_gpa);
        puts(TD_BORDER);

        printf("\n    K — Skip     B — Stop and save     [any other key] — Edit\n");
        printf("    > ");
        char kb = toupper(READ_CHAR()); printf("\n");

        if (kb == 'B') break;
        if (kb == 'K') { t = t->pNext; continue; }

        /* Edit scores */
        if (mode == 'A') {
            printf("\n  Enter new scores:\n\n");
            t->com_sc  = input_score("Comp. Science");
            t->sci_sc  = input_score("Science");
            t->eng_sc  = input_score("English");
            t->math_sc = input_score("Math");
            t->hist_sc = input_score("History");
        } else {
            printf("\n  Enter new score for %s:\n\n", subj_name);
            switch (subj_choice) {
                case '1': t->com_sc  = input_score("Comp. Science"); break;
                case '2': t->sci_sc  = input_score("Science");       break;
                case '3': t->eng_sc  = input_score("English");       break;
                case '4': t->math_sc = input_score("Math");          break;
                case '5': t->hist_sc = input_score("History");       break;
            }
        }
        update_gpa(t);

        /* Show updated scores */
        printf("\n  Updated:\n");
        puts(TD_BORDER);
        printf("  | %-16s | Score |  GPA  |\n", "Subject");
        puts(TD_BORDER);
        printf(TD_SUBJ,  "Comp. Science", t->com_sc,  t->com_gpa);
        printf(TD_SUBJ,  "Science",       t->sci_sc,  t->sci_gpa);
        printf(TD_SUBJ,  "English",       t->eng_sc,  t->eng_gpa);
        printf(TD_SUBJ,  "Math",          t->math_sc, t->math_gpa);
        printf(TD_SUBJ,  "History",       t->hist_sc, t->hist_gpa);
        puts(TD_BORDER);
        printf(TD_TOTAL, "Total Score",   t->total_sc, t->total_gpa);
        puts(TD_BORDER);

        t = t->pNext;
    }

    printf("\n  Score editing complete.\n");
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   PRINT_SCHOOL_SCORE  (option 7 — insertion order, score table)
═══════════════════════════════════════════════════════════════════ */
void PRINT_SCHOOL_SCORE(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS' SCORES  (Insertion Order)");

    Student **arr = build_ptr_array(pschool);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    printf("\n");
    print_score_table(arr, pschool->size);
    free(arr);
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   RANK_STUDENT  (option 8 — ranked by total score, score table)
═══════════════════════════════════════════════════════════════════ */
void RANK_STUDENT(School *pschool)
{
    if (isEmpty(pschool)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS' SCORES  (Ranked by Total Score)");

    Student **arr = build_ptr_array(pschool);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    sort_arr_by_score(arr, pschool->size);
    printf("\n");
    print_score_table(arr, pschool->size);
    free(arr);
    print_end();
}

/* ═══════════════════════════════════════════════════════════════════
   STUDENT_SEARCH_ID_SILENT
═══════════════════════════════════════════════════════════════════ */
bool STUDENT_SEARCH_ID_SILENT(School *pschool, char id[ID_LEN])
{
    if (isEmpty(pschool)) return false;
    Student *t = pschool->front;
    while (t != NULL) { if (!stricmp(t->ID, id)) return true; t = t->pNext; }
    return false;
}

/* ═══════════════════════════════════════════════════════════════════
   STUDENT_SEARCH_NAME_SILENT
═══════════════════════════════════════════════════════════════════ */
bool STUDENT_SEARCH_NAME_SILENT(School *pschool, char name[NAME_LEN])
{
    if (isEmpty(pschool)) return false;
    Student *t = pschool->front;
    while (t != NULL) { if (!stricmp(t->name, name)) return true; t = t->pNext; }
    return false;
}
