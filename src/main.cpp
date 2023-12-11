#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

template<typename Type>
class SingleLinkedList {
	struct Node {
        Type value{};
        Node* next_node{};

		Node() = default;
		Node(const Type& val, Node* next) : value(val), next_node(next) {}
	};
    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template<typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) : node_(node) {}
    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            assert(node_);
            auto old_value(*this);
            node_ = node_->next_node;
            return old_value;
        }

        reference operator*() const noexcept {
            assert(node_);
            return node_->value;
        }

        pointer operator->() const noexcept {
            assert(node_);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        if (IsEmpty() && size_ == 0) {
            SingleLinkedList tmp;
            for (auto it = rbegin(values); it != rend(values); ++it) {
                tmp.PushFront(*it);
            }
            swap(tmp);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (IsEmpty() && size_ == 0) {
            SingleLinkedList tmp;
            std::vector<Type> tmp_v;
            for (auto it = other.begin(); it != other.end(); ++it) {
                tmp_v.push_back(*it);
            }
            for (auto it = tmp_v.rbegin(); it != tmp_v.rend(); ++it) {
                tmp.PushFront(*it);
            }
            swap(tmp);
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            auto copy_value(rhs);
            swap(copy_value);
        }
        return *this;
    }

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{const_cast<Node*>(&head_)};
    }

    ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    Iterator end() noexcept {
        return Iterator{nullptr};
    }

    ConstIterator begin() const noexcept {
        return cbegin();
    }

    ConstIterator end() const noexcept {
        return cend();
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        try {
            pos.node_->next_node = new Node(value, pos.node_->next_node);
            ++size_;
        }
        catch (...) {
            throw;
        }
        return Iterator{pos.node_->next_node};
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (!IsEmpty()) {
            Node* next = pos.node_->next_node;
            pos.node_->next_node = pos.node_->next_node->next_node;
            delete next;
            --size_;
        }
        return Iterator{pos.node_->next_node};
    }

    void PopFront() noexcept {
        if (!IsEmpty()) {
            Node* current = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete current;
            --size_;
        }
    }

	size_t GetSize() const noexcept {
		return size_;
	}

	bool IsEmpty() const noexcept {
		return head_.next_node == nullptr;
	}

	void PushFront(const Type& value) {
		try {
			head_.next_node = new Node(value, head_.next_node);
			++size_;
		}
		catch (...) {
			throw;
		}
	}

	void Clear() noexcept {
		Node* current = head_.next_node;
		while (!(this->IsEmpty())) {
			head_.next_node = head_.next_node->next_node;
			delete current;
			current = head_.next_node;
		}
		size_ = 0;
	}

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    ~SingleLinkedList() {
        Clear();
    }

private:
	Node head_{};
	size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

int main() {}
