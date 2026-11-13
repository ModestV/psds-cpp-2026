#include <iostream>
#include <ostream>
#include <string>

struct Student {
    std::string name;
    int age;
};

using Resource = Student;

class SmartPtr {
public:
    SmartPtr(Resource* raw_ptr) : raw_ptr_(raw_ptr) {};
    ~SmartPtr() { delete raw_ptr_; };
    Resource& operator*() const { return *raw_ptr_; };
    Resource* operator->() const { return raw_ptr_; };
private:
    Resource* raw_ptr_ = nullptr;
};

int main() {
    SmartPtr student_ptr(new Student("Student", 18));
}