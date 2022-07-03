@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir build
pushd build

del *.exe *.ilk *.pdb

::This includes all include directory
SET INCLUDE_DIR=-I ..\vendor -I ..\vendor\glad

::This includes all cpp and c files that are needed to be compiled
SET CPP_C_FILES=C:\Users\pguan\4coder\skeletal_animation\src\*.cpp C:\Users\pguan\4coder\skeletal_animation\vendor\glad\*.c C:\Users\pguan\4coder\skeletal_animation\vendor\imgui\*.cpp  

::This includes all libs used in the current project
SET KTLIBS=..\vendor\GLFW\glfw3.lib user32.lib  opengl32.lib Gdi32.lib msvcrt.lib Shell32.lib

cl -W3 -Zi -EHsc %INCLUDE_DIR% %CPP_C_FILES% -link -NODEFAULTLIB:LIBCMT -LIBPATH:..\vendor\ -out:opengl_animation.exe %KTLIBS% /MACHINE:X64

popd
