#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>

// tree stuff
struct TreeNode {
	int courseID;
	TreeNode* left;
	TreeNode* right;

	TreeNode(int id) : courseID(id), left(nullptr), right(nullptr) {}
};

// student stuff

struct Student {
	int id;
	std::string name;
	double gpa;
	std::vector<int> enrolledCourses;
};

// course stuff

struct Course {
	int id;
	std::string name;
	int capacity;
	std::vector<int> enrolled;
	std::list<int> waitlist;
	TreeNode* prereqRoot;

	Course(int i, std::string n, int c) :
		id(i), name(n), capacity(c), prereqRoot(nullptr) {}
};


// global storage

std::map<int, Student>  students;
std::vector<Course> courses;
std::stack<std::string> undoStack;

// recursion (tree)

bool checkPrerequisities(TreeNode* node, Student& s) {
	if (node == nullptr) return true;

	bool completed = false;
	for (int c : s.enrolledCourses) {
		if (c == node->courseID) {
			completed = true;
		}
	}

	if (!completed) return false;

	return checkPrerequisities(node->left, s) && checkPrerequisities(node->right, s);
}


// searching algs

Student* linearSearchByName(std::string name) {
	for (auto& pair : students) {
		if (pair.second.name == name) {
			return &pair.second;
		}
	}
	return nullptr;
}

int binarySearchByID(std::vector<Student>& arr, int target) {
	int low = 0;
	int high = arr.size() - 1;


	while (low <= high) {
		int mid = (low + high) / 2;
		if (arr[mid].id == target) {
			return mid;
		}
		else if (arr[mid].id < target) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
	}
	return -1;
}


// sorting alg

void bubbleSortByGPA(std::vector<Student>& arr) {
	for (int i = 0; i < arr.size(); i++) {
		for (int j = 0; j < arr.size() - i - 1; j++) {
			if (arr[j].gpa > arr[j + 1].gpa) {
				std::swap(arr[j], arr[j + 1]);
			}
		}
	}
}

void selectionSortByName(std::vector<Student>& arr) {
	for (int i = 0; i < arr.size(); i++) {
		int minIdx = i;
		for (int j = i + 1; j < arr.size(); j++) {
			if (arr[j].name < arr[minIdx].name) {
				minIdx = j;
			}
		}
		std::swap(arr[i], arr[minIdx]);
	}
}

// core functions

void addStudent() {
	Student s;
	std::cout << "ID: ";
	std::cin >> s.id;
	std::cout << "Name: ";
	std::cin >> s.name;
	std::cout << "GPA: ";
	std::cin >> s.gpa;


	students[s.id] = s;
}

Course* findCourse(int id) {
	for (auto& c : courses) {
		if (c.id == id) {
			return &c;
		}
	}
	return nullptr;
}

void registerCourse() {
	int sid;
	int cid;

	std::cout << "Student ID: ";
	std::cin >> sid;
	std::cout << "Course ID: ";
	std::cin >> cid;

	if (students.find(sid) == students.end()) return;

	Student& s = students[sid];
	Course* c = findCourse(cid);

	if (!c) return;

	if (!checkPrerequisities(c->prereqRoot, s)) {
		std::cout << "Prerequisites not met" << std::endl;
		return;
	}

	if (c->enrolled.size() < c->capacity) {
		c->enrolled.push_back(sid);
		s.enrolledCourses.push_back(cid);
		undoStack.push("ADD");
	}
	else {
		c->waitlist.push_back(sid);
		std::cout << "Added to waitlist" << std::endl;
	}
}

void undoAction() {
	if (undoStack.empty()) return;

	std::cout << "Undoing last action..." << std::endl;
	undoStack.pop();
}


int main() {
	// course creation 

	courses.push_back(Course(101, "CS101", 1));
	courses.push_back(Course(102, "CS102", 1));

	// prereq creation (tree)

	courses[1].prereqRoot = new TreeNode(101);

	// add student  (vector and map)

	Student s1 = { 1, "Alexander", 3.5 };
	Student s2 = { 2, "Frederick", 3.9 };
	Student s3 = { 3, "Bobert", 2.8 };

	students[s1.id] = s1;
	students[s2.id] = s2;
	students[s3.id] = s3;

	std::cout << "Students added" << std::endl;

	// course registration (linked list)

	std::cout << std::endl << "Register Alexander for CS101" << std::endl;
	students[1].enrolledCourses.push_back(101);
	courses[0].enrolled.push_back(1);

	std::cout << "Register Frederick for CS101 (should be waitlisted)" << std::endl;
	if (courses[0].enrolled.size() < courses[0].capacity) {
		courses[0].enrolled.push_back(2);
	}
	else {
		courses[0].waitlist.push_back(2);
	}

	// waitlist (queue)

	std::cout << std::endl << "Processing waitlist" << std::endl;
	if (!courses[0].waitlist.empty()) {
		int nextStudent = courses[0].waitlist.front();
		courses[0].waitlist.pop_front();
		std::cout << "Student " << nextStudent << " waiting" << std::endl;
	}

	// undo (stack)

	undoStack.push("ADD CS101");
	undoStack.push("DROP CS101");

	std::cout << std::endl << "Undo stack test" << std::endl;
	while (!undoStack.empty()) {
		std::cout << "Undo: " << undoStack.top() << std::endl;
		undoStack.pop();
	}

	// check for prereq (recursion)

	std::cout << std::endl << "Checking prerequisites for CS102" << std::endl;
	bool ok = checkPrerequisities(courses[1].prereqRoot, students[1]);
	std::cout << "Alexander prereqs met? " << (ok ? "Yes" : "No") << std::endl;

	// searching

	std::cout << std::endl << "Linear search by name (Bobert)" << std::endl;
	Student* found = linearSearchByName("Bobert");
	if (found) {
		std::cout << "Found Bobert with GPA " << found->gpa << std::endl;
	}

	std::vector<Student> studentList;
	for (auto& p : students) {
		studentList.push_back(p.second);
	}

	std::sort(studentList.begin(), studentList.end(),
		[](Student a, Student b) { return a.id < b.id; });

	std::cout << std::endl << "Binary Search by ID (3)" << std::endl;
	int idx = binarySearchByID(studentList, 3);
	if (idx != -1) {
		std::cout << "Found student ID 3: " << studentList[idx].name<< std::endl;
	}


	// sorting

	std::cout << std::endl << "Bubble sort by GPA:" << std::endl;
	bubbleSortByGPA(studentList);
	for (auto& s : studentList) {
		std::cout << s.name << " " << s.gpa << std::endl;
	}

	std::cout << std::endl << "Selection sort by Name:" << std::endl;
	selectionSortByName(studentList);
	for (auto& s : studentList) {
		std::cout << s.name << std::endl;
	}

	return 0;
}
