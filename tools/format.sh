find ../src -name \*.hpp -name \*.h -o -name \*.cpp -o -name \*.c -name \*.inl | xargs clang-format -i -style=file
