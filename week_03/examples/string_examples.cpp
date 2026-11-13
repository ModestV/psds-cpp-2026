#include <cstdio>
#include <iostream>
#include <string>


void string_init() {
    std::cout << __func__ << ':' << std::endl;
    std::string s1;
    std::string s2 = "Hello";
    std::string s3("Hello", 4);
    std::string s4(5, 'a');
    std::string s5{5, 'a'};
    std::string s6(s2, 1, 3);
    std::cout << "s1 = \"" << s1 << "\" size = " << s1.size() << std::endl;
    std::cout << "s3 = \"" << s3 << "\" size = " << s3.size() << std::endl;
    std::cout << "s4 = \"" << s4 << "\" size = " << s4.size() << std::endl;
    std::cout << "s5 size = " << s5.size() << " codes: "
        << +s5[0] << ' ' << +s5[1] << std::endl;
    std::cout << "s6 = \"" << s6 << "\" size = " << s6.size() << std::endl;
    std::cout << std::endl;
}

void string_literal() {
    std::cout << __func__ << ':' << std::endl;
    using namespace std::string_literals;
    std::cout << "std::string(\"a\\0b\").size() = " << std::string("a\0b").size() << std::endl;
    std::cout << "\"a\\0b\"s.size() = " << "a\0b"s.size() << std::endl;
    std::cout << std::endl;
}

void string_concat() {
    std::cout << __func__ << ':' << std::endl;
    std::string str = "Hello";
    str += ", ";
    str += "my";
    str += ' ';
    str += std::string("friend");
    str.append("!");
    std::cout << str << std::endl;

    std::string result;
    for (int i = 0; i < 5; ++i) {
        result += "ab";
    }
    std::cout << result << std::endl;
    std::cout << std::endl;
}

void string_modify() {
    std::cout << __func__ << ':' << std::endl;
    std::string s = "Hello world";
    s.insert(5, ",");
    std::cout << s << std::endl;
    s.erase(0, 7);
    std::cout << s << std::endl;
    s.replace(0, 1, "W");
    std::cout << s << std::endl;
    std::cout << s.substr(1, 3) << std::endl;
    s.resize(3);
    std::cout << s << std::endl;
    s.resize(5, '!');
    std::cout << s << std::endl;
    std::cout << std::endl;
}

void string_find() {
    std::cout << __func__ << ':' << std::endl;
    std::string s = "one two one";
    std::cout << "find(\"one\") = " << s.find("one") << std::endl;
    std::cout << "find(\"one\", 1) = " << s.find("one", 1) << std::endl;
    std::cout << "rfind(\"one\") = " << s.rfind("one") << std::endl;
    std::cout << "find_first_of(\"tw\") = " << s.find_first_of("tw") << std::endl;
    std::cout << "find_first_not_of(\"one \") = " << s.find_first_not_of("one ") << std::endl;
    if (s.find('x') == std::string::npos) {
        std::cout << "'x' not found, npos = " << std::string::npos << std::endl;
    }
    std::cout << std::boolalpha;
    std::cout << "starts_with(\"one\") = " << s.starts_with("one") << std::endl;
    std::cout << "ends_with('e') = " << s.ends_with('e') << std::endl;
    std::cout << std::noboolalpha << std::endl;
}

void string_conversion() {
    std::cout << __func__ << ':' << std::endl;
    std::cout << std::to_string(42) << ' ' << std::to_string(3.5) << std::endl;
    std::cout << std::stoi("123") << ' ' << std::stod("2.5") << std::endl;
    size_t idx = 0;
    int j = std::stoi("42abc", &idx);
    std::cout << "stoi(\"42abc\") = " << j << ", idx = " << idx << std::endl;
    std::cout << std::endl;
}

void string_sso() {
    std::cout << __func__ << ':' << std::endl;
    std::string empty;
    std::string small = "short";
    std::string big(100, 'x');
    std::cout << "sizeof(std::string) = " << sizeof(std::string) << std::endl;
    std::cout << "empty capacity = " << empty.capacity() << std::endl;
    std::cout << "&small = " << &small << ", small.data() = "
        << static_cast<const void*>(small.data()) << std::endl;
    std::cout << "&big   = " << &big << ", big.data()   = "
        << static_cast<const void*>(big.data()) << std::endl;
    std::cout << std::endl;
}

int main() {
    string_init();
    string_literal();
    string_concat();
    string_modify();
    string_find();
    string_conversion();
    string_sso();
}
