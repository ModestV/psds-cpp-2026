#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity)
        : data_(std::max<std::size_t>(1, capacity), 0) {}

    RingBuffer(std::size_t capacity, int initial_value)
        : data_(std::max<std::size_t>(1, capacity), initial_value),
          size_(data_.size()) {}

    RingBuffer(std::initializer_list<int> values)
        : data_(std::max<std::size_t>(1, values.size()), 0),
          size_(values.size()) {
        std::copy(values.begin(), values.end(), data_.begin());
    }

    void Push(int value) {
        if (Full()) {
            data_[head_] = value;
            head_ = (head_ + 1) % Capacity();
            return;
        }

        const std::size_t position = (head_ + size_) % Capacity();
        data_[position] = value;
        ++size_;
    }

    bool TryPush(int value) {
        if (Full()) {
            return false;
        }

        Push(value);
        return true;
    }

    bool Pop() {
        if (Empty()) {
            return false;
        }

        head_ = (head_ + 1) % Capacity();
        --size_;

        if (size_ == 0) {
            head_ = 0;
        }

        return true;
    }

    bool TryPop(int& value) {
        if (Empty()) {
            return false;
        }

        value = Back();
        Pop();
        return true;
    }

    int& operator[](std::size_t index) {
        return data_[(head_ + index) % Capacity()];
    }

    const int& operator[](std::size_t index) const {
        return data_[(head_ + index) % Capacity()];
    }

    int& Front() {
        if (Empty()) {
            return empty_value_;
        }

        return data_[(head_ + size_ - 1) % Capacity()];
    }

    const int& Front() const {
        if (Empty()) {
            return empty_value_;
        }

        return data_[(head_ + size_ - 1) % Capacity()];
    }

    int& Back() {
        return Empty() ? empty_value_ : data_[head_];
    }

    const int& Back() const {
        return Empty() ? empty_value_ : data_[head_];
    }

    bool Empty() const {
        return size_ == 0;
    }

    bool Full() const {
        return size_ == Capacity();
    }

    std::size_t Size() const {
        return size_;
    }

    std::size_t Capacity() const {
        return data_.size();
    }

    void Clear() {
        head_ = 0;
        size_ = 0;
    }

    void Resize(std::size_t new_capacity) {
        new_capacity = std::max<std::size_t>(1, new_capacity);

        const std::size_t new_size = std::min(size_, new_capacity);
        const std::size_t first_kept = size_ - new_size;

        std::vector<int> new_data(new_capacity, 0);

        for (std::size_t i = 0; i < new_size; ++i) {
            new_data[i] = (*this)[first_kept + i];
        }

        data_ = std::move(new_data);
        head_ = 0;
        size_ = new_size;
    }

    std::vector<int> Vector() const {
        std::vector<int> result;
        result.reserve(size_);

        for (std::size_t i = 0; i < size_; ++i) {
            result.push_back((*this)[i]);
        }

        return result;
    }

private:
    std::vector<int> data_;
    std::size_t head_ = 0;
    std::size_t size_ = 0;
    inline static int empty_value_ = 0;
};
