#include "display.h"

/* ═══════════════════════════════════════════════════════════════════
   TABLE FORMAT MACROS
   ─────────────────────────────────────────────────────────────────
   Profile table — 83 chars wide:
     No(4) | Name(22) | ID(10) | Date of Birth(13) | Guardian Phone(15)

   Score table — 93 chars wide:
     No(4) | Name(18) | ID(9) | C.Sc(5) | Sci(5) | Eng(5) |
     Math(5) | Hist(5) | Total(7)
     Score cell : "%3d %c"   -> " 85 A"   (5 chars)
     Total cell : "%5.1f %c" -> " 86.6 B" (7 chars)

   Score detail card — 38 chars wide:
     Subject(16) | Score(7) | Grade(7)
═══════════════════════════════════════════════════════════════════ */

/* Profile table */
#define TP_BORDER \
    "  +------+------------------------+------------+---------------+-----------------+"
#define TP_HEAD \
    "  | %-4s | %-22s | %-10s | %-13s | %-15s |\n"
#define TP_ROW \
    "  | %4d | %-22.22s | %-10.10s | %-13s | %-15s |\n"

/* Score table */
#define TS_BORDER \
    "  +------+--------------------+-----------+-------+-------+-------+-------+-------+---------+"
#define TS_HEAD \
    "  | %-4s | %-18s | %-9s | %-5s | %-5s | %-5s | %-5s | %-5s | %-7s |\n"
#define TS_ROW \
    "  | %4d | %-18.18s | %-9.9s | %3d %c | %3d %c | %3d %c | %3d %c | %3d %c | %5.1f %c |\n"

/* Score detail card */
#define TD_BORDER  "  +------------------+-------+-------+"
#define TD_SUBJ    "  | %-16s | %5d |   %c   |\n"
#define TD_TOTAL   "  | %-16s | %5.1f |   %c   |\n"

/* ═══════════════════════════════════════════════════════════════════
   POINTER ARRAY HELPERS
   ─────────────────────────────────────────────────────────────────
   Sorted display is achieved by building a temporary array of
   Student pointers, sorting the array, and printing from it.
   The linked list itself is never reordered by display operations,
   preserving insertion order at all times.
   The caller must call free() on the returned pointer.
═══════════════════════════════════════════════════════════════════ */

/* Allocate and fill a pointer array from the linked list.
 * i : u16 — matches school->count type (u16). */
static Student **build_pointer_array(School *school)
{
    Student **arr = (Student **)malloc(school->count * sizeof(Student *));
    Student  *cur = school->front;
    u16       i;

    if (arr == NULL) return NULL;
    for (i = 0; i < school->count; i++) {
        arr[i] = cur;
        cur    = cur->next;
    }
    return arr;
}

/* Bubble sort: alphabetical ascending by name (case-insensitive).
 * i, j : u16 — iterate up to count-1 where count is u16. */
static void sort_by_name(Student **arr, u16 count)
{
    u16      i, j;
    bool     swapped;
    Student *tmp;

    for (i = 0; i < count - 1; i++) {
        swapped = false;
        for (j = 0; j < count - i - 1; j++) {
            if (stricmp(arr[j]->name, arr[j + 1]->name) > 0) {
                tmp        = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
                swapped    = true;
            }
        }
        if (!swapped) break;
    }
}

/* Bubble sort: total score descending (highest first).
 * i, j : u16 — same reasoning as sort_by_name. */
static void sort_by_score(Student **arr, u16 count)
{
    u16      i, j;
    bool     swapped;
    Student *tmp;

    for (i = 0; i < count - 1; i++) {
        swapped = false;
        for (j = 0; j < count - i - 1; j++) {
            if (arr[j]->total < arr[j + 1]->total) {
                tmp        = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
                swapped    = true;
            }
        }
        if (!swapped) break;
    }
}

/* ── Internal table printers ────────────────────────────────────── */

/* i : u16 — iterates up to count (u16). */
static void print_profile_table(Student **arr, u16 count)
{
    u16 i;

    puts(TP_BORDER);
    printf(TP_HEAD, "No.", "Name", "ID", "Date of Birth", "Guardian Phone");
    puts(TP_BORDER);
    for (i = 0; i < count; i++)
        printf(TP_ROW, (i32)(i + 1),
               arr[i]->name, arr[i]->id, arr[i]->birth, arr[i]->phone);
    puts(TP_BORDER);
    printf("  Note: Address not shown in table view."
           " Use option 2 for the full profile.\n");
}

/* i : u16 — same reasoning as print_profile_table. */
static void print_score_table(Student **arr, u16 count)
{
    u16      i;
    Student *s;

    puts(TS_BORDER);
    printf(TS_HEAD, "No.", "Name", "ID",
           "C.Sc", "Sci", "Eng", "Math", "Hist", "Total");
    puts(TS_BORDER);
    for (i = 0; i < count; i++) {
        s = arr[i];
        printf(TS_ROW, (i32)(i + 1),
               s->name, s->id,
               s->comp_science, s->comp_science_grade,
               s->science,      s->science_grade,
               s->english,      s->english_grade,
               s->math,         s->math_grade,
               s->history,      s->history_grade,
               s->total,        s->total_grade);
    }
    puts(TS_BORDER);
}

/* ═══════════════════════════════════════════════════════════════════
   PUBLIC DISPLAY FUNCTIONS
═══════════════════════════════════════════════════════════════════ */

/*
 * Print a section header aligned to BOX_BORDER width (67 chars).
 * Format:  +--- TITLE ----...----+
 *
 * padding : i8 — max 58 (when title is empty), min 1 (clamped).
 *                i8 range -128 to 127 covers this comfortably.
 * i       : i8 — loop index up to padding (max 58), fits in i8.
 */
void print_section(const char *title)
{
    i8 padding = (i8)(58 - (i8)strlen(title));
    i8 i;

    if (padding < 1) padding = 1;
    printf("\n  +--- %s ", title);
    for (i = 0; i < padding; i++) putchar('-');
    puts("+");
}

/* Print the closing border with a blank line. */
void print_end(void)
{
    printf("%s\n\n", BOX_BORDER);
}

/* Full detail card — used for single-student display. */
void print_profile_card(const Student *student)
{
    printf("%s\n", BOX_BORDER);
    printf("  |  %-14s: %-45s|\n", "Name",           student->name);
    printf("  |  %-14s: %-45s|\n", "ID",             student->id);
    printf("  |  %-14s: %-45s|\n", "Date of Birth",  student->birth);
    printf("  |  %-14s: %-45s|\n", "Address",        student->address);
    printf("  |  %-14s: %-45s|\n", "Guardian Phone", student->phone);
    printf("%s\n\n", BOX_BORDER);
}

/* Score detail table — used inside score cards and edit flows. */
void print_score_detail_table(const Student *student)
{
    puts(TD_BORDER);
    printf("  | %-16s | Score |  GPA  |\n", "Subject");
    puts(TD_BORDER);
    printf(TD_SUBJ,  "Comp. Science", student->comp_science, student->comp_science_grade);
    printf(TD_SUBJ,  "Science",       student->science,      student->science_grade);
    printf(TD_SUBJ,  "English",       student->english,      student->english_grade);
    printf(TD_SUBJ,  "Math",          student->math,         student->math_grade);
    printf(TD_SUBJ,  "History",       student->history,      student->history_grade);
    puts(TD_BORDER);
    printf(TD_TOTAL, "Total Score",   student->total,        student->total_grade);
    puts(TD_BORDER);
    printf("\n");
}

/* Full score card with header — used in single-student context. */
void print_score_card(const Student *student)
{
    printf("%s\n", BOX_BORDER);
    printf("  |  %-14s: %-45s|\n", "Name", student->name);
    printf("  |  %-14s: %-45s|\n", "ID",   student->id);
    printf("%s\n", BOX_BORDER);
    print_score_detail_table(student);
}

/* Option 3 — all profiles in insertion order. */
void print_all_profiles(School *school)
{
    Student **arr = NULL;

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENT PROFILES  (Insertion Order)");

    arr = build_pointer_array(school);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    printf("\n");
    print_profile_table(arr, school->count);
    free(arr);
    print_end();
}

/* Option 4 — all profiles sorted A-Z. */
void list_profiles_sorted(School *school)
{
    Student **arr = NULL;

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENT PROFILES  (A-Z)");

    arr = build_pointer_array(school);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    sort_by_name(arr, school->count);
    printf("\n");
    print_profile_table(arr, school->count);
    free(arr);
    print_end();
}

/* Option 7 — all scores in insertion order. */
void print_all_scores(School *school)
{
    Student **arr = NULL;

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS' SCORES  (Insertion Order)");

    arr = build_pointer_array(school);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    printf("\n");
    print_score_table(arr, school->count);
    free(arr);
    print_end();
}

/* Option 8 — all scores sorted by total score descending. */
void print_scores_ranked(School *school)
{
    Student **arr = NULL;

    if (is_empty(school)) { printf("  Database is empty.\n\n"); return; }

    print_section("ALL STUDENTS' SCORES  (Ranked by Total Score)");

    arr = build_pointer_array(school);
    if (arr == NULL) { printf("  Error: insufficient memory.\n"); return; }

    sort_by_score(arr, school->count);
    printf("\n");
    print_score_table(arr, school->count);
    free(arr);
    print_end();
}
