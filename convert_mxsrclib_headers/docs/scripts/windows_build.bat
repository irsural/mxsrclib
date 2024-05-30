set script_path=%~dp0

cd %script_path%
cd ../doxygen
doxygen Doxyfile

cd %script_path%
cd ..
sphinx-build -b html ./ _build/html

if /i %1 NEQ open goto exit

cd %script_path%
cd ../_build/html
py -m webbrowser -t index.html

:exit
cd %0