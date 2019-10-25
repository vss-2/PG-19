curl https://cin.ufpe.br/~vss2/drive/PG/New_PG%20-%20Linux_MinGW.zip >> 'PG.zip'
7z x PG.zip
rm -rf *.ini *.h *.cpp *.obj *.md *.dll
rm -rf ImGUI SDL2 lib
rm PG.zip
cd PG\ -\ Linux_MinGW/
rm -rf camera.h
rm -rf main.cpp

curl https://raw.githubusercontent.com/vss-2/PG-19/master/main.cpp >> 'main.cpp'
curl https://raw.githubusercontent.com/vss-2/PG-19/master/camera.h >> 'camera.h'
curl https://raw.githubusercontent.com/vss-2/PG-19/master/monkey_smooth.obj >> 'monkey_smooth.obj'
cd ImGUI
rm -rf imgui_sdl.cpp
curl https://raw.githubusercontent.com/vss-2/PG-19/e1722148b4bfb7c302f7c5dd588b3a33d2c609f0/ImGUI/imgui_sdl.cpp >> 'imgui_sdl.cpp'
