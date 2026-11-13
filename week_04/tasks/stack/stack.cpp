#include <cstddef>
#include <vector>

class Stack {
public:
    void Push(int value) {
        values_.push_back(value);
    }

    bool Pop() {
        if (values_.empty()) {
            return false;
        }
        values_.pop_back();
        return true;
    }

    int& Top() {
        return values_.empty() ? empty_value_ : values_.back();
    }

    const int& Top() const {
        return values_.empty() ? empty_value_ : values_.back();
    }

    bool Empty() const {
        return values_.empty();
    }

    std::size_t Size() const {
        return values_.size();
    }

    void Clear() {
        values_.clear();
    }

    void Swap(Stack& other) noexcept {
        values_.swap(other.values_);
    }

    friend bool operator==(const Stack& lhs, const Stack& rhs) {
        return lhs.values_ == rhs.values_;
    }

    friend bool operator!=(const Stack& lhs, const Stack& rhs) {
        return !(lhs == rhs);
    }

private:
    std::vector<int> values_;
    inline static int empty_value_ = 0;
};
