set PROJ_NAME=eyeMapper

set GIT_PROJ_DIR=Lexicon\%PROJ_NAME%
set PROJ_DIR=lexicon\%PROJ_NAME%

set APPS_DIR=C:\proj\tools\openframeworks_8.0\apps
set OF_PROJ_DIR=%APPS_DIR%\%PROJ_DIR%
set GIT_DIR=C:\Users\guy\Documents\GitHub\pixelator
set GIT_PROJ_LOC=%GIT_DIR%\%GIT_PROJ_DIR%

del /q %GIT_PROJ%\src\*.*
mkdir %GIT_PROJ%\src
del /q %GIT_PROJ%\%PROJ_NAME%.cbp
del /q %GIT_PROJ%\%PROJ_NAME%.workspace

copy %OF_PROJ_DIR%\src\*.* %GIT_PROJ%\src
copy %OF_PROJ_DIR%\%PROJ_NAME%.cbp %GIT_PROJ_LOC%
copy %OF_PROJ_DIR%\%PROJ_NAME%.workspace %GIT_PROJ_LOC%