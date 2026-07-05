@echo off
setlocal enabledelayedexpansion

:: Find the absolute path of the workspace root (two directories up)
for %%i in ("%~dp0..\..") do set "WORKSPACE_ROOT=%%~fidx"

set "LOCAL_RUBY_BIN=%WORKSPACE_ROOT%\ruby\installation\bin"

set "PATH=%LOCAL_RUBY_BIN%;%PATH%"

echo ====================================================
echo Initializing isolated Ceedling environment (Batch)...
echo Using Ruby at: %LOCAL_RUBY_BIN%
echo ====================================================

bundle exec ceedling test:all

endlocal