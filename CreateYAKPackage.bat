copy .\manifest.yml .\build
cd .\build
"C:\Program Files\Rhino 7\System\Yak.exe" build
del .\manifest.yml
cd ..
