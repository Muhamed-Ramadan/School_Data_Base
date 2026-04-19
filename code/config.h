#ifndef CONFIG_H
#define CONFIG_H

/*
 * config.h — UI Configuration
 * ─────────────────────────────────────────────────────────────────
 * All menu labels, section titles, and border strings live here.
 * To update any displayed text, edit this file only.
 */

/* ── Main box  (inner = 63 chars, total line = 67 chars) ─────── */
#define BOX_BORDER \
    "  +---------------------------------------------------------------+"

/* ── Welcome banner  (each line ≤ 61 printable chars) ──────────── */
#define WELCOME_TITLE    "SCHOOL MANAGEMENT SYSTEM"
#define WELCOME_SUBTITLE "Student Profile Administration"
#define WELCOME_L1       "Manage student profiles including:"
#define WELCOME_L2       "  Name | ID | Address | Guardian Phone | Date of Birth"
#define WELCOME_L3       "  Scores: Comp.Sc | Science | English | Math | History"
#define WELCOME_L4       "Search, edit, delete, list, and rank by total score"

/* ── Menu option labels  (each ≤ 60 chars) ──────────────────────
   Options 3 & 4 and 7 & 8 are aligned so their qualifiers start
   at the same horizontal position for visual consistency.         */
#define OPT_1  "1. Add New Student Profile"
#define OPT_2  "2. Student Profile Lookup, Edit & Remove"
#define OPT_3  "3. View All Student Profiles       (Insertion Order)"
#define OPT_4  "4. List All Student Profiles       (A-Z)"
#define OPT_5  "5. Student Scores Lookup & Edit"
#define OPT_6  "6. Edit All Students' Scores"
#define OPT_7  "7. View All Students' Scores       (Insertion Order)"
#define OPT_8  "8. List All Students' Scores       (Ranked by Score)"
#define OPT_D  "D. Exit"

#endif /* CONFIG_H */
