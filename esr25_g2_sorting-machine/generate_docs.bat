@echo off
setlocal enabledelayedexpansion

echo Checking for Doxygen installation...
where doxygen >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Doxygen is not installed or not in PATH
    echo Please install Doxygen from http://www.doxygen.nl/download.html
    pause
    exit /b 1
)

echo Checking for Git installation...
where git >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Git is not installed or not in PATH
    echo Please install Git from https://git-scm.com/downloads
    pause
    exit /b 1
)

echo Checking for Doxygen Awesome theme...
if not exist temp-awesome (
    echo Cloning Doxygen Awesome theme...
    git clone --depth 1 https://github.com/jothepro/doxygen-awesome-css.git temp-awesome
    if %ERRORLEVEL% neq 0 (
        echo Error: Failed to clone Doxygen Awesome theme
        pause
        exit /b 1
    )
)

echo Cleaning up old documentation...
if exist docs\html (
    echo Removing old HTML documentation...
    rd /s /q docs\html
)
if exist docs\latex (
    echo Removing old LaTeX documentation...
    rd /s /q docs\latex
)
if exist docs\xml (
    echo Removing old XML documentation...
    rd /s /q docs\xml
)

echo Checking for Doxyfile...
if not exist Doxyfile (
    echo Error: Doxyfile not found!
    echo Please make sure you are in the correct directory
    pause
    exit /b 1
)

echo Generating documentation...
doxygen Doxyfile

if %ERRORLEVEL% neq 0 (
    echo Error: Documentation generation failed!
    pause
    exit /b 1
)

echo Creating image directory in HTML output...
mkdir docs\html\images 2>nul

echo Copying images to HTML output...
copy docs\images\*.* docs\html\images\ >nul 2>&1

echo Updating image paths in generated HTML...
powershell -Command "(Get-Content docs\html\index.html) -replace 'docs/images/', 'images/' | Set-Content docs\html\index.html"

echo Documentation generated successfully!
echo HTML documentation is available in docs\html\index.html

echo Opening documentation in default browser...
start "" "docs\html\index.html"

pause 