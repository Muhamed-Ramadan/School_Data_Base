#ifndef CONFIG_H
#define CONFIG_H

/*
 * config.h — UI Configuration
 * ─────────────────────────────────────────────────────────
 * All menu labels, section titles, and border strings are
 * defined here. To update any displayed text, edit this
 * file only — no changes to school.c are needed.
 */

/* ── Main box border (inner width = 63 chars, total = 67) ─ */
#define BOX_BORDER \
    "  +---------------------------------------------------------------+"

/* ── Welcome screen lines ──────────────────────────────── */
#define WELCOME_TITLE    "SCHOOL MANAGEMENT SYSTEM"
#define WELCOME_SUBTITLE "Student Record Administration"
#define WELCOME_L1       "Manage student records including:"
#define WELCOME_L2       "  Name  |  ID  |  Address  |  Guardian Phone  |  Date of Birth"
#define WELCOME_L3       "  Scores: Comp. Science  |  Science  |  English  |  Math  |  History"
#define WELCOME_L4       "Search, edit, delete, list (A-Z), and rank by total score"

/* ── Menu option labels ─────────────────────────────────── */
#define OPT_1   "1. Add New Student Record"
#define OPT_2   "2. Edit Student Record"
#define OPT_3   "3. Find Student by ID"
#define OPT_4   "4. Find Student by Name"
#define OPT_5   "5. List All Students (A-Z)"
#define OPT_6   "6. Show Total Number of Enrolled Students"
#define OPT_7   "7. Remove Student Record"
#define OPT_8   "8. View All Student Records"
#define OPT_9   "9. Edit All Students' Scores"
#define OPT_A   "A. View All Students' Scores"
#define OPT_B   "B. Print All Students Ranked by Total Score"
#define OPT_C   "C. Exit"

#endif /* CONFIG_H */
