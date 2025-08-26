![testing](https://github.com/Vorex075/chx/actions/workflows/tests.yml/badge.svg)

# chx
A **lightweight go-like channel** library for cpp23+.

> [!NOTE]
> This project is **still in development**, but you can create issues if you encounter any problems or have suggestions.

---
The library includes:
- **Unbuffered_Channel**: This channel will block sender and receiver until the transaction is made
- **ThreadSafeCircularQueue**: Is kind of a buffered channel. Might be removed in the future.

---
### Future work
I plan to:
- Create a `Buffered Channel`, instead of the `ThreadSafeCircularQueue`
- Add a **select-like** structure.
- Add **waitgroup** struct for syncronization.

--- 
### Requirements
This project requires **cpp23** or above, since it uses `std::expected`.
In order to execute the tests, you will need **cmake 3.28** or above.

---

### Testing
You can **run all tests** in you local machine easily. Clone the repo and use:
``` bash
make tests
```
This will **compile all tests** in the `build` directory.

Then, simply run:
``` bash
make run_tests
```
