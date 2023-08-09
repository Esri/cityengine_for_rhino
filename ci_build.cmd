@echo on

set RHSDK=C:/rhino/rhsdk/
if %RHINO_VER_MAJOR%==6 (set DOTNET_FRAMEWORK_VERSION=4.5.2) else (set DOTNET_FRAMEWORK_VERSION=4.8)

msbuild /p:TargetFrameworkVersion=%DOTNET_FRAMEWORK_VERSION% /p:RhinoTargetVersion=%RHINO_VER_MAJOR% /p:PumaVersionBuild=%PUMA_VER_BUILD% /p:RhinoSdkDir=%RHSDK% /p:RhinoSdkPath=%RHSDK% /p:RhinoPropertySheetDir=%RHSDK%PropertySheets/ /p:RhinoIncDir=%RHSDK%inc

C:\rhino\python.3.9.13\tools\python.exe create_package.py --rhino-target %RHINO_VER_MAJOR%
