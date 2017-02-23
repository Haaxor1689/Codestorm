// Find all the problems in this code.
// Use valgrind, clang-tidy, clang sanitizers, ...
// Look at all the clang-tidy warnings and try to fix them.

#include <iostream>
#include <string>
#include <vector>

class Person {
	std::string name;
protected:
	const std::string& getName() const { return name; }
public:
	// Do not worry about the std::move here.
	// We will talk about move semantics later.
	// For now, just assume that it works for strings as for unique_ptr,
	// i.e. after x = std::move(y), y is empty and x contains the
	// original content of y (without copying anything).
	Person(std::string n) : name(std::move(n)) {}
	Person(Person&& other) : name(std::move(other.name)) {}
	virtual void report() {
		std::cout << "Generic person: " << name << '\n';
	}
};

class Student : public Person {
	std::vector<std::string> courses;
public:
	Student(std::string n) : Person(std::move(n)) {}
	void enroll(std::string course) {
		courses.push_back(std::move(course));
	}
	virtual void report() const {
		std::cout << "Student " << getName() << ", enrolled in courses:";
		for (std::vector<std::string>::const_iterator it = courses.begin();
		     it != courses.end(); ++it) {
			std::cout << "\n\t" << *it;
		}
		std::cout << "\n\t(" << courses.size() << " courses in total)\n";
	}
};

class Teacher : public Person {
	std::string office;
	std::string phoneNumber;
public:
	Teacher(const std::string& n, const std::string& o, const std::string& pn)
		: Person(n), office(o), phoneNumber(pn) {}
	virtual void report() const {
		std::cout << "Teacher " << getName() << ", office "
		          << office << ", phone no. " << phoneNumber << "\n";
	}
};

std::string ask(const std::string& query) {
	std::string response;
	std::cout << query;
	std::getline(std::cin, response);
	return response;
}

void askForCourses(Student& s) {
	std::string course;
	std::cout << "You may now add student's courses, finish with empty line.\n";
	do {
		course = ask("Course: ");
		s.enroll(course);
	} while (course != "");
}

int main() {
	using namespace std;
	vector<Person*> people;

	string choice;

	while (choice != "X") {
		choice = ask("Add person (T: teacher, S: student, P: generic person, X: end)? ");
		if (choice == "X") { break; }
		if (choice != "T" && choice != "S" && choice != "P") {
			continue;
		}
		string name = ask("Name: ");
		if (choice == "P") {
			people.push_back(new Person(name));
		} else if (choice == "S") {
			Student s(name);
			askForCourses(s);
			people.push_back(&s);
		} else {
			string office = ask("Office: ");
			string phoneNo = ask("Phone no.: ");
			people.push_back(new Teacher(name, office, phoneNo));
		}
	}

	cout << "\nPeople:\n";
	for (vector<Person*>::iterator it = people.begin(); it != people.end(); ++it) {
		(*it)->report();
	}
}
