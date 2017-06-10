find . -name ..\*.cpp -type f | xargs nkf -Lw --overwrite --oc=UTF-8-BOM
find . -name ..\*.hpp -type f | xargs nkf -Lw --overwrite --oc=UTF-8-BOM
find . -name ..\*.inl -type f | xargs nkf -Lw --overwrite --oc=UTF-8-BOM
