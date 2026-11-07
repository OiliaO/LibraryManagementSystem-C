# 📚 C Library Management System

A robust, console-based Library Management System written in **C**. This application provides a file-backed database solution to handle dynamic library operations, supporting distinct user roles, session state, and activity history tracking.

---

## 🌟 Key Features

### 🔐 Authentication & Role Management
* **Role Separation:** Distinct panels and workflows for **Librarians** and **Students**.
* **Auto-Bootstrap System:** Detects initial state on launch; if no librarian exists, prompts for first-time admin/librarian setup.
* **Credential Validation:** Prevents invalid input characters (e.g., `/`) to ensure data integrity during authentication.

### 👨‍💼 Librarian Capabilities
* Create new librarian and student accounts with duplicate username prevention.
* Add new books with auto-generated unique IDs.
* Edit book details (Title, Author, Genre/Subject).
* Remove books safely (prevents deletion of currently borrowed books).
* Process pending book borrow requests with interactive Accept/Reject workflows.
* View comprehensive activity history and log records for any book.

### 🎓 Student Capabilities
* Browse the full catalog of available books with real-time status (Available / Borrowed).
* Submit borrow requests for available books (prevents duplicate pending requests).
* Return currently borrowed books.

### 💾 Data Persistence & Safety
* **File-Based Storage:** Stores users, books, requests, and history logs across text files (`.txt`).
* **Atomic File Updates:** Uses temporary swap files (`temp.txt`) to ensure safe data modification and removal.
* **Input Sanitization:** Custom input buffer handlers (`get_line`, `read_credential`) prevent standard buffer overflow issues.
* **Timestamping:** Logs transactions with real-time date/time stamps.

---

## 🛠️ File Structure

* `library-management-system.c` - Core C source code containing program logic, CLI menus, and file handling routines.
* `listketab.txt` - Database of all registered books and their current statuses.
* `userpassketabdaar.txt` - Store for librarian credentials.
* `userpassdaneshjoo.txt` - Store for student credentials.
* `darkhastha.txt` - Queue for pending borrow requests.
* `natijeh.txt` - Transaction history and activity logs.

---

## 🚀 Getting Started

### Prerequisites
You need a C compiler installed on your system (e.g., `gcc`, `clang`, or MSVC).

### Compilation
Open your terminal or command prompt and run:

```bash
gcc library-management-system.c -o library_system
