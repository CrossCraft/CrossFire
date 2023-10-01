#pragma once
#include "Types.hpp"
#include "Allocator.hpp"

namespace CrossFire
{

/**
 * @brief A simple linked list.
 * @tparam T The type of the elements.
 */
template <typename T> class LinkedList {
    Allocator &allocator;

public:
    /**
     * @brief Creates a new linked list.
     */
    struct Node {
        T data;
        Node *next;

        explicit Node(const T &data)
            : data(data)
            , next(nullptr)
        {
        }
    };

    /**
     * @brief Creates a new linked list.
     * @param allocator The allocator to use.
     */
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

    /**
     * Get the size of the list
     * @return The size of the list
     */
    inline auto get_size() const -> usize
    {
        return size;
    }

    /**
     * Push to the back of the list
     * @param data The data to push
     * @return Nothing -- or an error if allocation failed.
     */
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

    /**
     * Push to the front of the list
     * @param data The data to push
     * @return Nothing -- or an error if allocation failed.
     */
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

    /**
     * @brief Pop from the back of the list.
     */
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

    /**
     * @brief Pop from the front of the list.
     */
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

    /**
     * @brief Clear the list.
     */
    auto clear() -> void
    {
        while (head != nullptr)
            pop_front();
    }

    /**
     * @brief Get the data at the given index.
     * @param index The index to get the data from.
     * @return The data.
     */
    auto get(usize index) -> T &
    {
        auto node = head;
        for (usize i = 0; i < index; i++)
            node = node->next;

        return node->data;
    }

    /**
     * @brief Get the data at the given index.
     * @param index The index to get the data from.
     * @return The data.
     */
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