#include <cstddef>
#include <string>
#include <utility>

struct SharedControlBlock {
    explicit SharedControlBlock(std::string* value) noexcept
        : ptr(value), shared_count(1), weak_count(0) {}

    std::string* ptr;
    std::size_t shared_count;
    std::size_t weak_count;
};

class WeakPtr;

class SharedPtr {
public:
    SharedPtr() noexcept = default;

    explicit SharedPtr(std::string* ptr) {
        if (ptr != nullptr) {
            control_ = new SharedControlBlock(ptr);
        }
    }

    ~SharedPtr() {
        Release();
    }

    SharedPtr(const SharedPtr& other) noexcept : control_(other.control_) {
        AddSharedReference();
    }

    SharedPtr(SharedPtr&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            SharedPtr copy(other);
            Swap(copy);
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            Release();
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }

    std::string* Get() const noexcept {
        return control_ == nullptr ? nullptr : control_->ptr;
    }

    std::string& operator*() const {
        return *Get();
    }

    std::string* operator->() const noexcept {
        return Get();
    }

    void Reset(std::string* ptr = nullptr) {
        if (Get() == ptr) {
            return;
        }

        SharedPtr replacement(ptr);
        Swap(replacement);
    }

    void Swap(SharedPtr& other) noexcept {
        std::swap(control_, other.control_);
    }

    std::size_t UseCount() const noexcept {
        return control_ == nullptr ? 0 : control_->shared_count;
    }

    explicit operator bool() const noexcept {
        return Get() != nullptr;
    }

private:
    explicit SharedPtr(SharedControlBlock* control, bool add_reference) noexcept
        : control_(control) {
        if (add_reference) {
            AddSharedReference();
        }
    }

    void AddSharedReference() noexcept {
        if (control_ != nullptr) {
            ++control_->shared_count;
        }
    }

    void Release() noexcept {
        if (control_ == nullptr) {
            return;
        }

        SharedControlBlock* old_control = control_;
        control_ = nullptr;

        if (--old_control->shared_count == 0) {
            delete old_control->ptr;
            old_control->ptr = nullptr;

            if (old_control->weak_count == 0) {
                delete old_control;
            }
        }
    }

    SharedControlBlock* control_ = nullptr;

    friend class WeakPtr;
};

class WeakPtr {
public:
    WeakPtr() noexcept = default;

    WeakPtr(const SharedPtr& shared) noexcept : control_(shared.control_) {
        AddWeakReference();
    }

    WeakPtr(const WeakPtr& other) noexcept : control_(other.control_) {
        AddWeakReference();
    }

    WeakPtr(WeakPtr&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }

    ~WeakPtr() {
        Release();
    }

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        if (this != &other) {
            WeakPtr copy(other);
            Swap(copy);
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            Release();
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }

    WeakPtr& operator=(const SharedPtr& shared) noexcept {
        WeakPtr replacement(shared);
        Swap(replacement);
        return *this;
    }

    void Reset() noexcept {
        Release();
    }

    void Swap(WeakPtr& other) noexcept {
        std::swap(control_, other.control_);
    }

    std::size_t UseCount() const noexcept {
        return control_ == nullptr ? 0 : control_->shared_count;
    }

    bool Expired() const noexcept {
        return UseCount() == 0;
    }

    SharedPtr Lock() const noexcept {
        if (Expired()) {
            return SharedPtr();
        }
        return SharedPtr(control_, true);
    }

private:
    void AddWeakReference() noexcept {
        if (control_ != nullptr) {
            ++control_->weak_count;
        }
    }

    void Release() noexcept {
        if (control_ == nullptr) {
            return;
        }

        SharedControlBlock* old_control = control_;
        control_ = nullptr;

        if (--old_control->weak_count == 0 && old_control->shared_count == 0) {
            delete old_control;
        }
    }

    SharedControlBlock* control_ = nullptr;
};

inline SharedPtr MakeShared(std::string value) {
    return SharedPtr(new std::string(std::move(value)));
}

inline void Swap(SharedPtr& lhs, SharedPtr& rhs) noexcept {
    lhs.Swap(rhs);
}

inline void Swap(WeakPtr& lhs, WeakPtr& rhs) noexcept {
    lhs.Swap(rhs);
}
