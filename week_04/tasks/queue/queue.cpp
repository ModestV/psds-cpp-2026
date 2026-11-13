#include <cstddef>
#include <initializer_list>
#include <stack>
#include <utility>
#include <vector>

class Queue {
public:
    Queue() = default;

    explicit Queue(std::size_t capacity) {
        input_.reserve(capacity);
        output_.reserve(capacity);
    }

    Queue(const std::vector<int>& values) : input_(values) {}

    Queue(std::initializer_list<int> values) : input_(values) {}

    explicit Queue(const std::stack<int>& values) {
        output_.reserve(values.size());
        std::stack<int> copy(values);

        while (!copy.empty()) {
            output_.push_back(copy.top());
            copy.pop();
        }
    }

    void Push(int value) {
        input_.push_back(value);
    }

    bool Pop() {
        if (Empty()) {
            return false;
        }

        MoveInputToOutput();
        output_.pop_back();
        return true;
    }

    int& Front() {
        if (Empty()) {
            return empty_value_;
        }

        MoveInputToOutput();
        return output_.back();
    }

    const int& Front() const {
        if (!output_.empty()) {
            return output_.back();
        }

        return input_.empty() ? empty_value_ : input_.front();
    }

    int& Back() {
        if (Empty()) {
            return empty_value_;
        }

        return input_.empty() ? output_.front() : input_.back();
    }

    const int& Back() const {
        if (!input_.empty()) {
            return input_.back();
        }

        return output_.empty() ? empty_value_ : output_.front();
    }

    bool Empty() const {
        return input_.empty() && output_.empty();
    }

    std::size_t Size() const {
        return input_.size() + output_.size();
    }

    void Clear() {
        input_.clear();
        output_.clear();
    }

    void Swap(Queue& other) noexcept {
        input_.swap(other.input_);
        output_.swap(other.output_);
    }

    friend bool operator==(const Queue& lhs, const Queue& rhs) {
        if (lhs.Size() != rhs.Size()) {
            return false;
        }

        for (std::size_t i = 0; i < lhs.Size(); ++i) {
            if (lhs.ElementAt(i) != rhs.ElementAt(i)) {
                return false;
            }
        }

        return true;
    }

    friend bool operator!=(const Queue& lhs, const Queue& rhs) {
        return !(lhs == rhs);
    }

private:
    void MoveInputToOutput() {
        if (!output_.empty()) {
            return;
        }

        while (!input_.empty()) {
            output_.push_back(input_.back());
            input_.pop_back();
        }
    }

    int ElementAt(std::size_t index) const {
        if (index < output_.size()) {
            return output_[output_.size() - 1 - index];
        }

        return input_[index - output_.size()];
    }

    std::vector<int> input_;
    std::vector<int> output_;
    inline static int empty_value_ = 0;
};
