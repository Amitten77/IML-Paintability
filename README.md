# IML Paintability Project

## Introduction
This project is a part of the IML Paintability project, which explores the online-coloring of multi-partite graphs.
The project implements the minimax algorithm to search all possibilities and determines the optimal strategy for the Pusher or Remover.

## Requirements
- Compiler:
  - GCC: version 13 or higher
  - Clang: version 12 or higher
  - MSVC: version 17 (Visual Studio 2022) or higher
- CMake: version 3.15 or higher

## Build
First, clone the repository:
```shell
git clone https://github.com/Amitten77/IML-Paintability.git
cd IML-paintability/c/
git switch tianyue
```

Then, build the project with the following commands:
```shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Run
The CMake build system generates three targets:

1. `main`:

    This is the main executable that runs the minimax algorithm.
    To run the executable, use the following command:
    ```shell
    ./main [config_file]
    ```
    Replace `[config_file]` with the path to the configuration file.
    See the configuration section below for more information.

    The program will generate two files: `winning/N[N]_K[K]_goal[GOAL]_board.txt` and `losing/N[N]_K[K]_goal[GOAL]_board.txt`,
    where `[N]`, `[K]`, and `[GOAL]` are the parameters in the configuration file.
    The first file contains a list of winning boards, and the second file contains a list of losing boards.

2. `verify`:

    This is a helper executable that verifies the generated boards.
    To run the executable, use the following command:
    ```shell
    ./verify [config_file]
    ```
    Replace `[config_file]` with the path to the configuration file.
    See the configuration section below for more information.

    The program will read the generated boards and verify that the winning and losing states are correct.
    Given `N`, `K`, and `GOAL`, the winning states and losing states will be loaded from `winning/N[N]_K[K]_goal[GOAL]_board.txt` and `losing/N[N]_K[K]_goal[GOAL]_board.txt`.
    If multiple groups of `n` and `k` are provided, the final `N` is the sum of all `n` values, and the final `K` is the maximum `k` value.

3. `simple_verify`:

    This is a simplified version of the `verify` executable that prioritizes readability over flexibility.
    To run the executable, use the following command:
    ```shell
    ./simple_verify [N] [K] [GOAL]
    ```
    Replace `[N]`, `[K]`, and `[GOAL]` with the respective parameters.

    Note that different from the `verify` executable, this program only accepts starting states with a single pair of `N` and `K`.

> On Windows, use `\` instead of `/` in the above commands.

## Configuration
Here are all the parameters that can be configured in the configuration file:
- `common.k-and-n` (used in `main` and `verify`):

    A list of numbers where each number represent the `k` value of a column. The length of the list is the number of columns.
    For example, `[2, 2, 2, 4, 4, 4, 4, 4]` represents a graph with 8 columns, where 3 columns have 2 chips each and 5 columns have 4 chips each.

- `common.goal` (used in `main` and `verify`):

    The target row for the Pusher to reach. The Pusher wins if, after the Remover's turn, there is at least one chip at or above this row.
    The paintability is the smallest goal that results in the Remover winning.

- `common.symmetric` (used in `main` and `verify`):

    Specifies whether to simulate a symmetric chip game.

- `minimax.threads` (used in `main`):

    The number of threads to use for the minimax algorithm.

- `minimax.hours-per-save` (used in `main`):

    To avoid losing progress, the program saves the current list of winning and losing states every `hours-per-save` hours.
    The files are saved as `winning/temp/N[N]_K[K]_goal[GOAL]_board_[DATETIME].txt` and `losing/temp/N[N]_K[K]_goal[GOAL]_board_[DATETIME].txt`.

    Note: If `hours-per-save` is set to 0 or negative, the program will not make any temporary save.

- `minimax.files-to-load-from.winning` (used in `main`):

    The main program allows the user to load known winning states from files, which can be used to speed up the search.
    This parameter is a list of paths to the files containing the known winning states.

    Note: The file `winning/N[N]_K[K]_goal[GOAL]_board.txt` is always loaded if it exists.

- `minimax.files-to-load-from.losing` (used in `main`):

    Same as `minimax.files-to-load-from.winning`, but for known losing states. The file `losing/N[N]_K[K]_goal[GOAL]_board.txt` is always loaded if it exists.

- `verify.threads` (used in `verify`):

    The number of threads to use for the verification.

Example configuration file:
```json
{
    "common": {
        "k-and-n": [3, 3, 3, 3, 3, 3],
        "goal": 8,
        "symmetric": false
    },
    "minimax": {
        "threads": 32,
        "hours-per-save": 8,
        "files-to-load-from": {
            "winning": [],
            "losing": []
        }
    },
    "verify": {
        "threads": 32
    }
}
```

## Acknowledgements
- [CMake](https://cmake.org/) - Licensed under [BSD 3-Clause License](https://cmake.org/licensing/)
- [JSON for Modern C++](https://json.nlohmann.me/) - Licensed under [MIT License](https://json.nlohmann.me/home/license/)
- [GoogleTest](https://google.github.io/googletest) - Licensed under [BSD 3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)
