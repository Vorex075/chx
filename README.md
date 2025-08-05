# chx
A lightweight go-like channel library for cpp23+.

> [!NOTE]
> This proyect is still not finished. Still, you can create issues if you find any problem or have any suggestions.

---
The library includes:
- **Unbuffered_Channel**: This channel will block sender and receiver until the transaction is made
- **ThreadSafeCircularQueue**: Is kind of a buffered channel. Might be removed in the future.

---
## Future work
I plan to:
- Create a `Buffered Channel`, instead of the `ThreadSafeCircularQueue`
- Add a select-like structure.
- Add waitgroup struct for syncronization.

--- 
## Requirements
This proyect requires **cpp23** or above, since it uses `std::expected`.
In order to execute the tests, you will need **cmake 3.28** or above.

---

## Testing
You can **run all tests** in you local machine easily. Clone the repo and use:
``` bash
make tests
```
This will **compile all tests** in the `build` directory.

Then, simply run:
``` bash
make run_tests
```
