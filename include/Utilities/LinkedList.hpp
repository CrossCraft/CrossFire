#pragma once
#include "Types.hpp"
#include "Allocator.hpp"

namespace CrossFire
{

// Singly Linked List
template <typename T> class LinkedList {
    Allocator &allocator;

public:
    struct Node {
        T data;
        Node *next;

        Node(const T &data)
            : data(data)
            , next(nullptr)
        {
        }
    };

    LinkedList(Allocator &allocator)
        : allocator(allocator)
        , head(nullptr)
        , tail(nullptr)
        , size(0)
    {
    }

    ~LinkedList()
    {
        clear();
    }

    inline auto get_size() const -> usize
    {
        return size;
    }

    inline auto push_back(const T &data) -> ResultVoid<AllocationError>
    {
        auto res = allocator.create<Node>(data);
        if (res.is_err())
            return res.unwrap_err();

        auto node = res.unwrap();

        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }

        size++;
        return Ok();
    }

    auto push_front(const T &data) -> ResultVoid<AllocationError>
    {
        auto res = allocator.create<Node>(data);
        if (res.is_err())
            return res.unwrap_err();

        auto node = res.unwrap();

        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            node->next = head;
            head = node;
        }

        size++;
        return Ok();
    }

    auto pop_back() -> void
    {
        if (head == nullptr)
            return;

        if (head == tail) {
            allocator.destroy(head);
            head = nullptr;
            tail = nullptr;
        } else {
            auto node = head;
            while (node->next != tail)
                node = node->next;

            allocator.destroy(tail);
            tail = node;
            tail->next = nullptr;
        }

        size--;
    }

    auto pop_front() -> void
    {
        if (head == nullptr)
            return;

        if (head == tail) {
            allocator.destroy(head);
            head = nullptr;
            tail = nullptr;
        } else {
            auto node = head;
            head = head->next;
            allocator.destroy(node);
        }

        size--;
    }

    auto clear() -> void
    {
        while (head != nullptr)
            pop_front();
    }

    auto get(usize index) -> T &
    {
        auto node = head;
        for (usize i = 0; i < index; i++)
            node = node->next;

        return node->data;
    }

    auto operator[](usize index) -> T &
    {
        return get(index);
    }

private:
    Node *head;
    Node *tail;
    usize size;
};

}