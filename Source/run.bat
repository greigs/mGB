ubuntu run make
python as2gbmap.py -j ./obj/mgb.map mgb.sym
rgbfix.exe -c -f hg mgb.gb
cmd.exe /c "testrunner\bgb.exe mGB.gb -listen"
