# School Management System

A console-based student profile management system written in C. Built as the capstone project for the C module of the **NTI Embedded Systems diploma**, it demonstrates dynamic memory management, pointer manipulation, and queue-based data structure design applied to a real-world problem.

**Author:** Mohamed Ramadan  
**LinkedIn:** [muhamed-ramadan](https://www.linkedin.com/in/muhamed-ramadan)  
**GitHub:** [Muhamed-Ramadan](https://github.com/Muhamed-Ramadan)

---

## Data Structures

The system models a school database as a **queue of singly linked nodes**.

```
School (struct queue)
┌─────────────────┐
│ int   size      │
│ Student* front  │──► [ Student 1 ] ──► [ Student 2 ] ──► · · · ──► NULL
│ Student* rear   │─────────────────────────────────────► [ Student n ]
└─────────────────┘

Student (struct node)
┌──────────────────────┐
│ char  name[30]       │
│ char  ID[10]         │
│ char  birth[11]      │  ← DD/MM/YYYY
│ char  address[40]    │
│ char  phone[16]      │  ← Guardian phone
│ int   com_sc         │  ← Computer Science score (0–100)
│ int   sci_sc         │  ← Science
│ int   eng_sc         │  ← English
│ int   math_sc        │  ← Math
│ int   hist_sc        │  ← History
│ float total_sc       │  ← Average of all five subjects
│ char  com_gpa        │  ← Letter grade per subject (A/B/C/D/F)
│ char  sci_gpa        │
│ char  eng_gpa        │
│ char  math_gpa       │
│ char  hist_gpa       │
│ char  total_gpa      │  ← Overall letter grade
│ Student* pNext  ─────┼──► next node
└──────────────────────┘
```

Nodes are allocated dynamically on the heap with `malloc` and linked through `pNext` pointers. The linked list is **never reordered** by display operations — sorted views are produced by building a temporary pointer array, sorting it, and discarding it after printing.

---

## Menu

```
+---------------------------------------------------------------+
|  MAIN MENU                              12 students enrolled  |
+---------------------------------------------------------------+
|  -- Profiles --                                               |
|   1. Add New Student Profile                                  |
|   2. Student Profile Lookup, Edit & Remove                    |
|   3. View All Student Profiles       (Insertion Order)        |
|   4. List All Student Profiles       (A-Z)                    |
|  -- Scores --                                                 |
|   5. Student Scores Lookup & Edit                             |
|   6. Edit All Students' Scores                                |
|   7. View All Students' Scores       (Insertion Order)        |
|   8. List All Students' Scores       (Ranked by Score)        |
|                                                               |
|   D. Exit                                                     |
+---------------------------------------------------------------+
```

The enrolled-student count updates automatically in the header after every add or remove operation. Options 3 and 7 display data in insertion order. Options 4 and 8 display data sorted (A-Z and by total score respectively).

---

## Features

### Option 1 — Add New Student Profile
Collects name, date of birth, ID, address, and guardian phone. Validates each field before accepting it. Enforces uniqueness of both name and ID across the entire database.

### Option 2 — Student Profile Lookup, Edit & Remove
Unified lookup flow: search by name or ID, view the full profile card, then choose to edit all fields, remove the profile (with Y/N confirmation), or search again — all within one continuous session without returning to the main menu.

### Option 3 — View All Student Profiles (Insertion Order)
Displays all profiles in a formatted table in the order they were added.

### Option 4 — List All Student Profiles (A-Z)
Builds a temporary pointer array, sorts it alphabetically by name using bubble sort, and displays the table. The linked list is unchanged.

### Option 5 — Student Scores Lookup & Edit
Search by name or ID, view the full score card with per-subject GPA and total score, then optionally edit all subjects or one specific subject. Displays updated results immediately after editing.

### Option 6 — Edit All Students' Scores
Choose a mode once (all subjects or one specific subject), then step through every student sequentially. Per student: any key to edit, K to skip, B to stop and save progress.

### Option 7 — View All Students' Scores (Insertion Order)
Displays all scores in a formatted table in insertion order.

### Option 8 — List All Students' Scores (Ranked by Score)
Builds a temporary pointer array, sorts by total score descending, and displays the score table. The linked list is unchanged.

---

## GPA System

Scores follow the American grading scale (0–100). Letter grades are computed and stored after every score change.

| Score  | Grade |
|--------|-------|
| 90–100 | A     |
| 80–89  | B     |
| 70–79  | C     |
| 60–69  | D     |
| 0–59   | F     |

The total score is the arithmetic average of all five subjects. The overall GPA letter is derived from the rounded total score.

---

## Input Validation

All fields are validated before acceptance. The program retries until input is valid and displays the constraints before each prompt.

| Field | Rules |
|-------|-------|
| **Name** | Letters, spaces, hyphens only. Minimum two words, each word ≥ 2 chars. Hyphen must be between two letters. No character repeated more than twice in a row. Length: 5–29. Must be unique. |
| **ID** | Letters and digits only (no spaces or symbols). Length: 4–9. Must be unique. |
| **Date of Birth** | Format `DD/MM/YYYY`. Leap-year-aware calendar validation. Student age must be 14–21. |
| **Address** | Letters, digits, spaces, `.` `,` `-` `/`. No consecutive identical non-alphanumeric characters. Length: 10–39. |
| **Guardian Phone** | Digits only. Use `00` prefix for international numbers (e.g. `0020100123456`). Length: 6–15. |
| **Scores** | Integers 0–100 only. |

---

## Design Notes

**Insertion-order preservation** — The linked list always maintains insertion order. Sorted views (A-Z, ranked) are produced by building a temporary `Student **` pointer array, running bubble sort on the pointers, printing the result, and freeing the array. The database is never reordered.

**Duplicate prevention** — Silent searches on both name and ID are performed before inserting or editing a record. During edit, the current field is temporarily cleared before the uniqueness check to correctly handle the case where the new value matches the existing one.

**Deletion edge cases** — `DELETE_STUDENT` handles all three structural positions: front (advance `front`), rear (retract `rear`), and middle (bypass with `prev->pNext = cur->pNext`). Memory is freed in every case.

**Unified lookup flows** — Options 2 and 5 share the same N/I/B navigation pattern for consistency. State is managed with labeled `goto` statements, a standard pattern for console-menu state machines in C.

**GPA caching** — Letter grades and total score are stored in the node itself and recomputed by `update_gpa()` after every score update, avoiding repeated calculation during display.

**config.h** — All menu labels and border strings are defined in `config.h`. To update any displayed text, edit that file only — no changes to `school.c` are needed.

---

## Testing

The project includes an automated test suite that validates key behaviors without manual interaction.

### How it works

The test runner compiles the project with `-DTESTING`, which replaces `getche()` with a `stdin`-compatible alternative. This allows input to be piped from text files:

```bash
gcc src/main.c src/school.c -DTESTING -o test_build.exe
test_build.exe < tests/input/test_01.txt > tests/output/test_01.txt
```

The runner then compares each output against a pre-approved expected file. If they match, the test passes. If the output changes after a code modification — even unintentionally — the test fails, making the suite useful for catching regressions.

### Running the tests

```bash
# From the repository root
python tests/run_tests.py
```

Output:
```
Compiling with -DTESTING...
Compilation successful.

  [PASS]  test_01_add_and_view
  [PASS]  test_02_duplicate_rejection
  ...

--------------------------------------------------
  Results:  10 passed  |  0 failed  |  0 saved
--------------------------------------------------
```

### Test coverage summary

| # | Test | What it verifies |
|---|------|-----------------|
| 01 | Add and view | Insertion order preserved after adding two students |
| 02 | Duplicate rejection | Name and ID uniqueness enforced on add |
| 03 | Name validation | Six invalid name formats rejected before valid input accepted |
| 04 | Date validation | Five invalid dates rejected; leap year accepted |
| 05 | Delete positions | Correct removal of middle, front, and rear nodes |
| 06 | Sort preserves order | A-Z list does not affect insertion-order view |
| 07 | GPA boundaries | Correct letter grades at score boundaries 90/80/70/60/59 |
| 08 | Empty database | All operations return "Database is empty" gracefully |
| 09 | Edit scores skip/stop | K skips a student, B stops mid-session; edits saved |
| 10 | Rank preserves order | Ranked view does not affect insertion-order view |

Full test case details, including inputs and expected behaviors, are documented in [TEST_CASES.md](TEST_CASES.md).

---

## Build

The project uses `<conio.h>` and `stricmp()`, which are part of the MinGW/MSVC runtime on Windows.

```bash
# MinGW on Windows
gcc src/main.c src/school.c -o school_management_system.exe
./school_management_system.exe
```

> **Windows users:** A pre-built executable is available in [Releases](https://github.com/Muhamed-Ramadan/School_Data_Base/releases) — no build required.

> **Linux / macOS:** Replace `#include <conio.h>` with a `getchar()`-based alternative and `stricmp()` with `strcasecmp()` from `<strings.h>`.

---

## Project Structure

```
/
├── src/
│   ├── main.c        # Entry point
│   ├── school.c      # All function implementations
│   ├── school.h      # Structs, constants, and public interface
│   └── config.h      # UI labels and border strings
├── tests/
│   ├── run_tests.py  # Automated test runner
│   ├── input/        # Input files for each test case
│   └── expected/     # Pre-approved expected outputs
├── TEST_CASES.md
└── README.md
```

---

## Planned Features

- **Partial name search** — Allow searching by a part of a name (e.g. first name only) and display all matching results, with a numbered selection when multiple matches are found.

---

## Known Limitations

- Uses `<conio.h>` (`getche`, `stricmp`) — Windows/MinGW only. Portable replacements are straightforward but not included.
- Sorting uses bubble sort on a pointer array — suitable for school-sized datasets; not intended for large-scale use.
- Data is in-memory only — no file persistence between sessions.

---

## Context

Built as the capstone project for the C module of the **NTI Embedded Systems diploma**. The goal was to implement a working data management system using dynamic memory and linked-list data structures from scratch, with no external libraries beyond the C standard library.
