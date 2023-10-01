#pragma once
#include "Utilities/Types.hpp"
namespace CrossFire
{

template <typename T> class TailQueue {
    Allocator &allocator;

public:
    struct Node {
        Node *next;
        Node *prev;
        T data;
    };

    TailQueue(Allocator &allocator)
        : allocator(allocator)
        , head(nullptr)
        , tail(nullptr)
        , size(0)
    {
    }

    ~TailQueue()
    {
        clear();
    }

    /**
     * Get the size of the queue
     * @return The size of the queue
     */
    inline auto get_size() const -> usize
    {
        return size;
    }

    /**
     * Push to the head of the queue
     * @param data The data to push
     */
    [[nodiscard]] auto push(T data) -> ResultVoid<AllocationError>
    {
        auto res = allocator.create<Node>();
        if (res.is_err())
            return res.unwrap_err();

        auto node = res.unwrap();
        node->data = data;
        node->next = nullptr;
        node->prev = nullptr;

        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }

        size++;
        return Ok();
    }

    /**
     * Pop from the tail of the queue
     * @return The data from the tail of the queue
     */
    inline auto pop() -> T
    {
        if (tail == nullptr)
            return T();

        auto node = tail;
        auto data = node->data;

        if (tail->prev != nullptr) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            head = nullptr;
            tail = nullptr;
        }

        allocator.destroy(node);
        size--;

        return data;
    }

    /**
     * Clear the queue
     */
    inline auto clear() -> void
    {
        while (tail != nullptr) {
            pop();
        }

        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    /**
     * Get the data at the given index
     * @param index The index to get the data from
     * @return The data at the given index
     */
    inline auto get(usize index) -> T &
    {
        auto node = head;
        for (usize i = 0; i < index; i++)
            node = node->next;

        return node->data;
    }

    /**
     * Get the data at the given index
     * @param index The index to get the data from
     * @return The data at the given index
     */
    inline auto operator[](usize index) -> T &
    {
        return get(index);
    }

private:
    Node *head;
    Node *tail;
    usize size;
};

}