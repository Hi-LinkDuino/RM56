@rem Copyright (c) 2020-2021 Huawei Device Co., Ltd.
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem     http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.

@echo off
set BASE_DIR=%~dp0
set PYTHON=python
set TOOLS=tools
cd /d %BASE_DIR%

(where %PYTHON% | findstr %PYTHON%) >nul 2>&1 || (
    @echo "Python3.7 or higher version required!"
    pause
    goto:eof
)

python -c "import sys; exit(1) if sys.version_info.major < 3 or sys.version_info.minor < 7 else exit(0)"
@if errorlevel 1 (
    @echo "Python3.7 or higher version required!"
    pause
    goto:eof
)

python -c "import easy_install"
@if errorlevel 1 (
    @echo "Please install setuptools first!"
    goto:eof
)

if not exist %TOOLS% (
    @echo "no %TOOLS% directory exist"
	goto:eof
)

for %%a in (%TOOLS%/*.egg) do (
    python -m easy_install --user %TOOLS%/%%a
    @if errorlevel 1 (
      @echo "Error occurs to install %%a!"
    )
)
for %%a in (%TOOLS%/*.tar.gz) do (
    python -m easy_install --user %TOOLS%/%%a
    @if errorlevel 1 (
      @echo "Error occurs to install %%a!"
    )
)
python -m xdevice %*
