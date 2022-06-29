@echo off
pushd build
del *.exe *.ilk *.pdb *.obj
Echo Y | rmdir /s .vs
popd