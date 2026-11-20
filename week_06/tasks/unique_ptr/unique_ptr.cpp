#include <string>
#include <utility>

class UniquePtr {
public:
    UniquePtr() noexcept = default;

    explicit UniquePtr(std::string* ptr) noexcept : ptr_(ptr) {}

    ~UniquePtr() {
        delete ptr_;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    std::string* Get() const noexcept {
        return ptr_;
    }

    std::string& operator*() const {
        return *ptr_;
    }

    std::string* operator->() const noexcept {
        return ptr_;
    }

    std::string* Release() noexcept {
        std::string* result = ptr_;
        ptr_ = nullptr;
        return result;
    }

    void Reset(std::string* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

private:
    std::string* ptr_ = nullptr;
};

inline UniquePtr MakeUnique(std::string value) {
    return UniquePtr(new std::string(std::move(value)));
}

inline void Swap(UniquePtr& lhs, UniquePtr& rhs) noexcept {
    lhs.Swap(rhs);
}
