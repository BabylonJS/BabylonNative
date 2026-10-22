@echo off
call "%~dp0..\..\..\..\Dependencies\JsRuntimeHost\Tests\UnitTests\Source\Android\gradlew.bat" -p "%~dp0." %*
exit /b %errorlevel%
