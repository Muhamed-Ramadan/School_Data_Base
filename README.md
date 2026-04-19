# School Management System

A console-based student profile management system written in C. Built as the capstone project for the C module of the **NTI Embedded Systems diploma**, it demonstrates dynamic memory management, pointer manipulation, and queue-based data structure design applied to a real-world problem.

---

## Data Structures

The system models a school database as a **queue of singly linked nodes**.

```
School (struct queue)
┌─────────────────┐
│ int   size      │
│ Student* front  │──────────► [ Student 1 ] ──► [ Student 2 ] ──► · · · ──► NULL
│ Student* rear   │──────────────────────────────────────────────► [ Student n ]
└─────────────────┘

Student (struct node)
┌──────────────────────┐
│ char  name[30]       │
│ char  ID[10]         │
│ char  birth[11]      │   ← DD/MM/YYYY
│ char  address[40]    │
│ char  phone[16]      │
│ int   com_sc         │   ← Computer Science score (0–100)
│ int   sci_sc         │   ← Science
│ int   eng_sc         │   ← English
│ int   math_sc        │   ← Math
│ int   hist_sc        │   ← History
│ float total_sc       │   ← average of all five subjects
│ char  com_gpa        │   ← letter grade per subject (A/B/C/D/F)
│ char  sci_gpa        │
│ char  eng_gpa        │
│ char  math_gpa       │
│ char  hist_gpa       │
│ char  total_gpa      │   ← overall letter grade
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

Options 3 and 7 display data in insertion order. Options 4 and 8 display data sorted (A-Z and by total score respectively). The enrolled-student count updates automatically in the header.

---

## Features

### Option 1 — Add New Student Profile
Collects name, date of birth, ID, address, and guardian phone. Validates each field before accepting it. Enforces uniqueness of both name and ID.

### Option 2 — Student Profile Lookup, Edit & Remove
Unified lookup flow: search by name or ID, view the full profile card, then choose to edit all fields, remove the profile (with Y/N confirmation), or search again — all within one continuous session.

### Option 3 — View All Student Profiles (Insertion Order)
Displays all profiles in a formatted table in the order they were added.

### Option 4 — List All Student Profiles (A-Z)
Builds a temporary pointer array, sorts it alphabetically, and displays the table. The linked list is unchanged.

### Option 5 — Student Scores Lookup & Edit
Search by name or ID, view the full score card with per-subject GPA, then optionally edit all subjects or one subject. Displays updated results immediately.

### Option 6 — Edit All Students' Scores
Choose a mode (all subjects / one specific subject) and a subject (if specific), then step through every student sequentially. Per student: K skips, B stops and saves progress.

### Option 7 — View All Students' Scores (Insertion Order)
Displays all scores in a formatted table in insertion order.

### Option 8 — List All Students' Scores (Ranked by Score)
Builds a temporary pointer array, sorts by total score descending, and displays the score table. The linked list is unchanged.

---

## GPA System

Scores follow the American grading scale (0–100). Letter grades are computed and stored after every score change.

| Score | Grade |
|-------|-------|
| 90–100 | A |
| 80–89  | B |
| 70–79  | C |
| 60–69  | D |
| 0–59   | F |

The total score is the arithmetic average of all five subjects. The overall GPA letter is derived from the rounded total score.

---

## Input Validation

All fields are validated before acceptance. The program retries until input is valid and displays the constraints before each prompt.

| Field | Rules |
|-------|-------|
| **Name** | Letters, spaces, hyphens only. Min two words, each word ≥ 2 chars. Hyphen must be between two letters. No character repeated more than twice in a row. Length: 5–29. |
| **ID** | Letters and digits only (no spaces or symbols). Length: 4–9. Must be unique. |
| **Date of Birth** | Format `DD/MM/YYYY`. Leap-year-aware calendar validation. Student age must be 14–21. |
| **Address** | Letters, digits, spaces, `.` `,` `-` `/`. No consecutive identical non-alphanumeric characters. Length: 10–39. |
| **Guardian Phone** | Digits only. Use `00` prefix for international numbers (e.g. `0020100123456`). Length: 6–15. |
| **Scores** | Integers 0–100 only. |

---

## Design Notes

**Insertion-order preservation** — The linked list always maintains insertion order. Sorted views (A-Z, ranked) are produced by building a temporary `Student **` pointer array, running bubble sort on the pointers (not the node data), printing the result, and freeing the array. This approach avoids permanently reordering the database.

**Duplicate prevention** — Before inserting or editing a record, the system performs silent searches on both name and ID. During edit, the current field is temporarily cleared before the uniqueness check to avoid rejecting the existing value as a duplicate.

**Deletion edge cases** — `DELETE_STUDENT` handles all three structural positions: front (advance `front`), rear (retract `rear`), and middle (bypass with `prev->pNext = cur->pNext`). Memory is freed in every case.

**Unified lookup flows** — Options 2 and 5 share the same N/I/B navigation pattern for consistency. State is managed through labeled `goto` statements, a standard pattern for console-menu state machines in C.

**GPA caching** — Letter grades and total score are recomputed and stored in the node itself after every score update (`update_gpa()`). This avoids repeated calculation during display operations.

---

## Build

The project uses `<conio.h>` and `stricmp()`, which are part of the MinGW/MSVC runtime on Windows.

```bash
# MinGW on Windows
gcc src/main.c src/school.c -o school_management_system.exe
./school_management_system.exe
```

> **Windows users:** A pre-built executable is available in [Releases](../../releases) — no build required.

> **Linux / macOS:** Replace `#include <conio.h>` with a `getchar()`-based alternative and `stricmp()` with `strcasecmp()` from `<strings.h>`.

---

## Project Structure

```
/
├── src/
│   ├── main.c        # Entry point
│   ├── school.c      # All function implementations
│   ├── school.h      # Structs, constants, and public interface
│   └── config.h      # UI labels and border strings (edit here to change any text)
├── docs/
│   └── School Management System.pptx
└── README.md
```

---

## Test Cases

> _Test cases will be documented here after validation testing._
> _Coverage will include: input boundary conditions, duplicate detection,_
> _deletion edge cases (front / middle / rear), sort correctness, and GPA computation._

---

## Known Limitations

- Uses `<conio.h>` (`getche`, `stricmp`) — Windows/MinGW only. Portable replacements are straightforward but not included.
- Sorting uses bubble sort on a pointer array — suitable for school-sized datasets; not intended for large-scale use.
- Data is in-memory only — no file persistence between sessions.

---

## Context

Built as the capstone project for the C module of the **NTI Embedded Systems diploma**. The goal was to implement a working data management system using dynamic memory and linked-list data structures from scratch, with no external libraries beyond the C standard library.
