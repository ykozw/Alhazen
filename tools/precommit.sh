# format
echo "Format"
find ../src -name \*.hpp -name \*.h -o -name \*.cpp -o -name \*.c -name \*.inl | xargs clang-format -i -style=file

# UTF8-BOM
echo "Encode"
find ../src -name \*.hpp -name \*.h -o -name \*.cpp -o -name \*.c -name \*.inl | xargs nkf -Lw --overwrite --oc=UTF-8-BOM
