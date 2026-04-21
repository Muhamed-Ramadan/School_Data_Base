#ifndef DISPLAY_H
#define DISPLAY_H

/*
 * display.h — Display and Print Interface
 * ─────────────────────────────────────────────────────────────────
 * Internal header. Declares all functions that produce output:
 * section headers, profile tables, score tables, and cards.
 */

#include "school.h"

/* -- Section framing ------------------------------------------ */
void print_section           (const char *title);
void print_end               (void);

/* -- Single-student cards ------------------------------------- */
void print_profile_card      (const Student *student);
void print_score_card        (const Student *student);
void print_score_detail_table(const Student *student);

/* -- Whole-database views ------------------------------------- */
void print_all_profiles      (School *school);
void list_profiles_sorted    (School *school);
void print_all_scores        (School *school);
void print_scores_ranked     (School *school);

#endif /* DISPLAY_H */
