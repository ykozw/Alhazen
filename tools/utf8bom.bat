for /R ../src/ %%i in (*.cpp,*hpp,*inl) do (
nkf32.exe --overwrite --oc=UTF-8-BOM "%%i"
)
pause
