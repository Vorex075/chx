![testing](https://github.com/Vorex075/chx/actions/workflows/tests.yml/badge.svg)

# chx
A **lightweight go-like channel** library for cpp23+. 

Found this structure from [go](https://go.dev/) and, as a way to learn about concurrency, I tried to build it from scratch.

---
The library includes:
- **Unbuffered Channel**: This channel will block sender and receiver until the transaction is made
- **Buffered Channel**: This channel will only block sender if the internal `buffer` is full, and the receiver if it is empty.

---
## Future work
I plan to:
- Add iterators for the channels.
- Add **waitgroup** struct for syncronization.

--- 
## Requirements
This project requires **cpp23** or above, since it uses `std::expected`.
In order to execute the tests, you will need **cmake 3.28** or above.

---

## Testing
You can **run all tests** in you local machine easily. Clone the repo and use:
``` bash
make test
```
This will **compile all tests** in the `build` directory.

Then, simply run:
``` bash
make run_tests
```

---
## Installation
Simply download the compressed headers into your local machine using: 
``` bash
curl -LOJ 'https://github.com/Vorex075/chx/releases/download/v0.1.0/chx-0.1.0-Linux-x86_64.tar.gz'`. (both .tar.gz and .zip files contains the same)
```

Under `chx-<version>.tar.gz` there are 2 folders: `include` and `lib`. I recommend you to use cmake, with the following lines to your `CMakeLists.txt` file to add the library into your project:
``` Cmake
# ...
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dir") # where 'dir' is the directory where the chx library is located
find_package(chx REQUIRED CONFIG)  
# ...
```

## Contributing
If you'd like to contribute, please fork the repository and open a pull request to the `main` branch.

