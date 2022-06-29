@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir build
pushd build

del *.exe *.ilk *.pdb

cl -W3 -Zi -EHsc -I ..\vendor -I ..\vendor\glad  C:\Users\pguan\4coder\hello_world\src\*.cpp C:\Users\pguan\4coder\hello_world\vendor\glad\*.c C:\Users\pguan\4coder\hello_world\vendor\imgui\*.cpp -link -NODEFAULTLIB:LIBCMT -LIBPATH:..\vendor\ -out:opengl_animation.exe ..\vendor\GLFW\glfw3.lib user32.lib  opengl32.lib Gdi32.lib msvcrt.lib Shell32.lib /MACHINE:X64

popd
