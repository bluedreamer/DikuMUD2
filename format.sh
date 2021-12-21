#!/bin/bash
find dm-dist-ii -name "*.cpp" -exec clang-format -i {} \;
find dm-dist-ii -name "*.h" -exec clang-format -i {} \;
