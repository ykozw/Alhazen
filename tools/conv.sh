find ./../src/ -name *.cpp -type f | xargs nkf -w8 -Lw --overwrite --oc=UTF-8-BOM
find ./../src/ -name *.hpp -type f | xargs nkf -w8 -Lw --overwrite --oc=UTF-8-BOM
find ./../src/ -name *.inl -type f | xargs nkf -w8 -Lw --overwrite --oc=UTF-8-BOM
