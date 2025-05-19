#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "../include/circular_queue/circular_queue.hpp"


TEST_SUITE("CircularQueue<int, 3>") {

    TEST_CASE("Initial state is empty") {
        chx::CircularQueue<int, 3> q;
        CHECK(q.size() == 0);
        CHECK(q.front() == nullptr);
    }

    TEST_CASE("Push and front work correctly") {
        chx::CircularQueue<int, 3> q;
        CHECK(q.push(10));
        CHECK(q.push(20));
        CHECK(*q.front() == 10);
    }

    TEST_CASE("Pop removes elements in FIFO order") {
        chx::CircularQueue<int, 3> q;
        q.push(1);
        q.push(2);
        q.push(3);
        CHECK(*q.front() == 1);
        q.pop();
        CHECK(*q.front() == 2);
        q.pop();
        CHECK(*q.front() == 3);
        q.pop();
        CHECK(q.front() == nullptr);
        CHECK(q.size() == 0);
    }

    TEST_CASE("Cannot push when full") {
        chx::CircularQueue<int, 3> q;
        CHECK(q.push(1));
        CHECK(q.push(2));
        CHECK(q.push(3));
        CHECK_FALSE(q.push(4)); // full
    }

    TEST_CASE("Push with move works") {
        chx::CircularQueue<std::string, 2> q;
        std::string name = "test";
        CHECK(q.push(std::move(name)));
        CHECK(name.empty());
        CHECK(q.front() != nullptr);
        CHECK(*q.front() == "test");
    }

    TEST_CASE("Queue wraps around correctly") {
        chx::CircularQueue<int, 2> q;
        CHECK(q.push(1));
        CHECK(q.push(2));
        CHECK_FALSE(q.push(3));
        q.pop(); // remove 1
        CHECK(q.push(4)); // should go to position 0
        CHECK(q.size() == 2);
        CHECK(*q.front() == 2);
        q.pop();
        CHECK(*q.front() == 4);
    }

    TEST_CASE("Pop on empty queue is safe") {
        chx::CircularQueue<int, 2> q;
        q.pop(); // should not crash
        CHECK(q.front() == nullptr);
        CHECK(q.size() == 0);
    }
}
