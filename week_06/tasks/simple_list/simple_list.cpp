#include <cstddef>
#include <string>
#include <utility>

class SimpleList {
private:
    struct Node {
        explicit Node(const std::string& value) : data(value) {}

        explicit Node(std::string&& value) noexcept
            : data(std::move(value)) {}

        std::string data;
        Node* next = nullptr;
        Node* prev = nullptr;
    };

public:
    SimpleList() noexcept = default;

    ~SimpleList() {
        Clear();
    }

    SimpleList(const SimpleList& other) {
        try {
            for (Node* node = other.head_;
                 node != nullptr;
                 node = node->next) {
                PushBack(node->data);
            }
        } catch (...) {
            Clear();
            throw;
        }
    }

    SimpleList(SimpleList&& other) noexcept
        : head_(other.head_),
          tail_(other.tail_),
          size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    SimpleList& operator=(const SimpleList& other) {
        if (this != &other) {
            SimpleList copy(other);
            Swap(copy);
        }

        return *this;
    }

    SimpleList& operator=(SimpleList&& other) noexcept {
        if (this != &other) {
            Clear();

            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;

            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }

        return *this;
    }

    void Swap(SimpleList& other) noexcept {
        if (this == &other) {
            return;
        }

        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    std::size_t Size() const noexcept {
        return size_;
    }

    bool Empty() const noexcept {
        return size_ == 0;
    }

    void PushBack(const std::string& value) {
        Node* node = new Node(value);
        LinkAfter(tail_, node);
    }

    void PushBack(std::string&& value) {
        Node* node = new Node(std::move(value));
        LinkAfter(tail_, node);
    }

    void PushFront(const std::string& value) {
        Node* node = new Node(value);
        LinkBefore(head_, node);
    }

    void PushFront(std::string&& value) {
        Node* node = new Node(std::move(value));
        LinkBefore(head_, node);
    }

    void PopBack() noexcept {
        if (tail_ == nullptr) {
            return;
        }

        Node* node = tail_;
        tail_ = tail_->prev;

        if (tail_ != nullptr) {
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }

        delete node;
        --size_;
    }

    void PopFront() noexcept {
        if (head_ == nullptr) {
            return;
        }

        Node* node = head_;
        head_ = head_->next;

        if (head_ != nullptr) {
            head_->prev = nullptr;
        } else {
            tail_ = nullptr;
        }

        delete node;
        --size_;
    }

    std::string& Back() {
        return tail_->data;
    }

    const std::string& Back() const {
        return tail_->data;
    }

    std::string& Front() {
        return head_->data;
    }

    const std::string& Front() const {
        return head_->data;
    }

    void Clear() noexcept {
        Node* node = head_;

        while (node != nullptr) {
            Node* next = node->next;
            delete node;
            node = next;
        }

        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

private:
    void LinkAfter(Node* position, Node* node) noexcept {
        if (position == nullptr) {
            node->next = head_;

            if (head_ != nullptr) {
                head_->prev = node;
            } else {
                tail_ = node;
            }

            head_ = node;
        } else {
            node->prev = position;
            node->next = position->next;

            if (position->next != nullptr) {
                position->next->prev = node;
            } else {
                tail_ = node;
            }

            position->next = node;
        }

        ++size_;
    }

    void LinkBefore(Node* position, Node* node) noexcept {
        if (position == nullptr) {
            node->prev = tail_;

            if (tail_ != nullptr) {
                tail_->next = node;
            } else {
                head_ = node;
            }

            tail_ = node;
        } else {
            node->next = position;
            node->prev = position->prev;

            if (position->prev != nullptr) {
                position->prev->next = node;
            } else {
                head_ = node;
            }

            position->prev = node;
        }

        ++size_;
    }

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

inline void Swap(SimpleList& lhs, SimpleList& rhs) noexcept {
    lhs.Swap(rhs);
}
