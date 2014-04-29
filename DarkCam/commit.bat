set SRC_DIR=C:\proj\tools\openframeworks_8.0\apps\heara\darkcam
set GIT_DIR=C:\Users\guy\Documents\GitHub\pixelator\DarkCam

del /q %GIT_DIR%\src\*.*
del /q %GIT_DIR%\darkcam.cbp
del /q %GIT_DIR%\darkcam.workspace

copy %SRC_DIR%\src\*.* %GIT_DIR%\src
copy %SRC_DIR%\darkcam.cbp %GIT_DIR%
copy %SRC_DIR%\darkcam.workspace %GIT_DIR%