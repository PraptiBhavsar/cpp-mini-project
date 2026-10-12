#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

// Each student has five subjects.
const int SUBJECT_COUNT = 5;
const string SUBJECTS[SUBJECT_COUNT] = {"Mathematics", "Physics", "Programming",
                                        "Digital Logic", "English"};

// A struct groups related information about one student.
struct Student {
  int enrollment = 0;
  string name;
  int semester = 1;
  int marks[SUBJECT_COUNT] = {-1, -1, -1, -1, -1}; // -1 means marks not entered
  int attended = 0;
  int conducted = 0;
};

// All student records are kept in this vector while the program runs.
vector<Student> students;
const string FILE_NAME = "students.txt";

// Read a whole-number input and reject invalid values.
int readNumber(const string &message, int minimum, int maximum) {
  int number;

  while (true) {
    cout << message;

    if (cin >> number && number >= minimum && number <= maximum) {
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return number;
    }

    cout << "Invalid input. Try again.\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}

// Read text that may contain spaces, such as a student's full name.
string readText(const string &message) {
  string text;

  do {
    cout << message;
    getline(cin, text);

    if (text.empty()) {
      cout << "This field cannot be empty.\n";
    }
  } while (text.empty());

  return text;
}

// Find a student by enrollment number.
// Return their position in the vector, or -1 if they are not found.
int findStudent(int enrollment) {
  for (int i = 0; i < (int)students.size(); i++) {
    if (students[i].enrollment == enrollment) {
      return i;
    }
  }

  return -1;
}

// Return true only when marks have been entered for all five subjects.
bool hasMarks(const Student &s) {
  for (int i = 0; i < SUBJECT_COUNT; i++) {
    if (s.marks[i] == -1) {
      return false;
    }
  }

  return true;
}

// Add all five subject marks.
double getTotal(const Student &s) {
  double total = 0;

  for (int i = 0; i < SUBJECT_COUNT; i++) {
    total += s.marks[i];
  }

  return total;
}

// Each of the five subjects is out of 100, so percentage = total / 5.
double getPercentage(const Student &s) { return getTotal(s) / SUBJECT_COUNT; }

// Convert a percentage into a grade.
string getGrade(const Student &s) {
  if (!hasMarks(s))
    return "N/A";

  double percentage = getPercentage(s);

  if (percentage >= 90)
    return "A+";
  if (percentage >= 80)
    return "A";
  if (percentage >= 70)
    return "B";
  if (percentage >= 60)
    return "C";
  if (percentage >= 50)
    return "D";

  return "F";
}

// Calculate attendance percentage safely.
double getAttendance(const Student &s) {
  if (s.conducted == 0)
    return 0;

  return s.attended * 100.0 / s.conducted;
}

// Save all students. Each student is written on one line.
// The | character separates the fields.
bool saveStudents() {
  ofstream file(FILE_NAME);

  if (!file) {
    cout << "ERROR: Could not open students.txt for saving.\n";
    return false;
  }

  for (const Student &s : students) {
    file << s.enrollment << '|' << s.name << '|' << s.semester;

    for (int i = 0; i < SUBJECT_COUNT; i++) {
      file << '|' << s.marks[i];
    }

    file << '|' << s.attended << '|' << s.conducted << '\n';
  }

  file.close();
  return true;
}

// Load saved students when the program starts.
void loadStudents() {
  ifstream file(FILE_NAME);

  if (!file) {
    cout << "No saved file yet. A new record file will be created.\n";
    return;
  }

  string line;

  while (getline(file, line)) {
    stringstream row(line);
    string field;
    Student s;

    try {
      getline(row, field, '|');
      s.enrollment = stoi(field);

      getline(row, s.name, '|');

      getline(row, field, '|');
      s.semester = stoi(field);

      for (int i = 0; i < SUBJECT_COUNT; i++) {
        getline(row, field, '|');
        s.marks[i] = stoi(field);
      }

      getline(row, field, '|');
      s.attended = stoi(field);

      getline(row, field, '|');
      s.conducted = stoi(field);

      // Avoid loading the same enrollment number twice.
      bool duplicate = false;

      for (const Student &existing : students) {
        if (existing.enrollment == s.enrollment) {
          duplicate = true;
          break;
        }
      }

      if (!duplicate) {
        students.push_back(s);
      }
    } catch (...) {
      cout << "Skipped one invalid line in students.txt.\n";
    }
  }

  cout << students.size() << " student record(s) loaded.\n";
}

void addStudent() {
  cout << "\n--- ADD STUDENT ---\n";

  Student s;

  s.enrollment = readNumber("Enrollment number: ", 1, 2147483647);

  if (findStudent(s.enrollment) != -1) {
    cout << "That enrollment number already exists.\n";
    return;
  }

  s.name = readText("Student name: ");
  s.semester = readNumber("Semester (1-8): ", 1, 8);

  students.push_back(s);

  if (saveStudents()) {
    cout << "Student added and saved successfully.\n";
  }
}

void viewStudents() {
  cout << "\n--- ALL STUDENTS ---\n";

  if (students.empty()) {
    cout << "No student records. Add a student first.\n";
    return;
  }

  cout << left << setw(12) << "Enrollment" << setw(24) << "Name" << setw(10)
       << "Semester" << setw(14) << "Percentage"
       << "Grade\n";

  cout << string(70, '-') << '\n';

  for (const Student &s : students) {
    cout << left << setw(12) << s.enrollment << setw(24) << s.name.substr(0, 22)
         << setw(10) << s.semester;

    if (hasMarks(s)) {
      cout << setw(14) << fixed << setprecision(2) << getPercentage(s)
           << getGrade(s);
    } else {
      cout << setw(14) << "Pending" << "N/A";
    }

    cout << '\n';
  }
}

void showStudent(const Student &s) {
  cout << "\nEnrollment: " << s.enrollment << "\nName: " << s.name
       << "\nSemester: " << s.semester << "\n\nMarks:\n";

  for (int i = 0; i < SUBJECT_COUNT; i++) {
    cout << "  " << SUBJECTS[i] << ": ";

    if (s.marks[i] == -1) {
      cout << "Not entered\n";
    } else {
      cout << s.marks[i] << "/100\n";
    }
  }

  if (hasMarks(s)) {
    cout << "Total: " << getTotal(s) << "/500\n"
         << "Percentage: " << fixed << setprecision(2) << getPercentage(s)
         << "%\n"
         << "Grade: " << getGrade(s) << '\n';
  } else {
    cout << "Result: Pending (enter all five subject marks)\n";
  }

  cout << "Attendance: " << s.attended << "/" << s.conducted << " (" << fixed
       << setprecision(2) << getAttendance(s) << "%)\n";
}

void searchStudent() {
  cout << "\n--- SEARCH STUDENT ---\n";

  int enrollment = readNumber("Enrollment number: ", 1, 2147483647);
  int index = findStudent(enrollment);

  if (index == -1) {
    cout << "Student not found.\n";
  } else {
    showStudent(students[index]);
  }
}

void updateStudent() {
  cout << "\n--- UPDATE STUDENT ---\n";

  int enrollment = readNumber("Enrollment number: ", 1, 2147483647);
  int index = findStudent(enrollment);

  if (index == -1) {
    cout << "Student not found.\n";
    return;
  }

  cout << "1. Change name\n2. Change semester\n0. Cancel\n";
  int choice = readNumber("Choice: ", 0, 2);

  if (choice == 1) {
    students[index].name = readText("New name: ");
  } else if (choice == 2) {
    students[index].semester = readNumber("New semester (1-8): ", 1, 8);
  } else {
    return;
  }

  if (saveStudents()) {
    cout << "Student updated.\n";
  }
}

void deleteStudent() {
  cout << "\n--- DELETE STUDENT ---\n";

  int enrollment = readNumber("Enrollment number: ", 1, 2147483647);
  int index = findStudent(enrollment);

  if (index == -1) {
    cout << "Student not found.\n";
    return;
  }

  cout << "Delete " << students[index].name << "?\n1. Yes\n0. No\n";

  if (readNumber("Choice: ", 0, 1) == 0) {
    return;
  }

  students.erase(students.begin() + index);

  if (saveStudents()) {
    cout << "Student deleted.\n";
  }
}

void enterMarks() {
  cout << "\n--- ENTER MARKS ---\n";

  int enrollment = readNumber("Enrollment number: ", 1, 2147483647);
  int index = findStudent(enrollment);

  if (index == -1) {
    cout << "Student not found. Add the student first.\n";
    return;
  }

  for (int i = 0; i < SUBJECT_COUNT; i++) {
    students[index].marks[i] =
        readNumber(SUBJECTS[i] + " marks (0-100): ", 0, 100);
  }

  if (saveStudents()) {
    cout << "Marks saved.\n";
    showStudent(students[index]);
  }
}

void manageAttendance() {
  cout << "\n--- ATTENDANCE ---\n";

  int enrollment = readNumber("Enrollment number: ", 1, 2147483647);
  int index = findStudent(enrollment);

  if (index == -1) {
    cout << "Student not found. Add the student first.\n";
    return;
  }

  int conducted = readNumber(
      "Total classes conducted: ", students[index].conducted, 1000000);

  int attended = readNumber(
      "Total classes attended: ", students[index].attended, conducted);

  students[index].conducted = conducted;
  students[index].attended = attended;

  if (saveStudents()) {
    cout << "Attendance saved: " << fixed << setprecision(2)
         << getAttendance(students[index]) << "%\n";
  }
}

void rankStudents() {
  vector<Student> ranked;

  for (const Student &s : students) {
    if (hasMarks(s))
      ranked.push_back(s);
  }

  if (ranked.empty()) {
    cout << "No results yet. Enter marks first.\n";
    return;
  }

  sort(ranked.begin(), ranked.end(), [](const Student &a, const Student &b) {
    return getPercentage(a) > getPercentage(b);
  });

  cout << "\n--- RANKING ---\n";

  for (int i = 0; i < (int)ranked.size(); i++) {
    cout << i + 1 << ". " << ranked[i].name << " - " << fixed << setprecision(2)
         << getPercentage(ranked[i]) << "%\n";
  }
}

void showAnalytics() {
  double totalPercentages = 0;
  int resultCount = 0;
  int passed = 0;
  const Student *topper = nullptr;

  for (const Student &s : students) {
    if (!hasMarks(s))
      continue;

    double percentage = getPercentage(s);
    totalPercentages += percentage;
    resultCount++;

    if (percentage >= 50)
      passed++;

    if (topper == nullptr || percentage > getPercentage(*topper)) {
      topper = &s;
    }
  }

  cout << "\n--- CLASS ANALYTICS ---\n";
  cout << "Registered students: " << students.size() << '\n';
  cout << "Students with complete marks: " << resultCount << '\n';

  if (resultCount == 0) {
    cout << "Enter marks to see analytics.\n";
    return;
  }

  cout << "Class average: " << fixed << setprecision(2)
       << totalPercentages / resultCount << "%\n";
  cout << "Students with 50% or above: " << passed << '\n';
  cout << "Students below 50%: " << resultCount - passed << '\n';
  cout << "Topper: " << topper->name << " (" << getPercentage(*topper)
       << "%)\n";
}

int main() {
  loadStudents();

  while (true) {
    cout << "\n========== SMART STUDENT MANAGEMENT ==========\n"
         << "1. Add student\n"
         << "2. View all students\n"
         << "3. Search student\n"
         << "4. Update student\n"
         << "5. Delete student\n"
         << "6. Enter marks\n"
         << "7. Manage attendance\n"
         << "8. Rank students\n"
         << "9. Class analytics\n"
         << "10. Save records\n"
         << "0. Exit\n";

    int choice = readNumber("Enter choice: ", 0, 10);

    switch (choice) {
    case 1:
      addStudent();
      break;
    case 2:
      viewStudents();
      break;
    case 3:
      searchStudent();
      break;
    case 4:
      updateStudent();
      break;
    case 5:
      deleteStudent();
      break;
    case 6:
      enterMarks();
      break;
    case 7:
      manageAttendance();
      break;
    case 8:
      rankStudents();
      break;
    case 9:
      showAnalytics();
      break;
    case 10:
      if (saveStudents())
        cout << "Records saved.\n";
      break;
    case 0:
      if (saveStudents())
        cout << "Records saved. Goodbye!\n";
      else
        cout << "Warning: records could not be saved.\n";
      return 0;
    }
  }
}
