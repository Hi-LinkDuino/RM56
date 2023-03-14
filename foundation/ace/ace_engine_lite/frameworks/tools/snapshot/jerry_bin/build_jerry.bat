@rem Copyright (c) 2020 Huawei Device Co., Ltd.
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

echo ==jerry building on windows requires mscv compiler or mingw installed==
echo step 1: clone jerryscript repo
git clone git@gitee.com:openharmony/third_party_jerryscript.git jerryscript

echo step 2: reset the commit to the 2.1.0 tag
cd ./jerryscript
git reset --hard e8bc7a2b93a6edfa463458c8bb69fac2a36feb9e

echo step 3: compile jerry
python tools/build.py --mem-heap=48 --snapshot-exec=ON --snapshot-save=ON^
  --profile=es5.1 --error-messages=ON --logging=ON --mem-stats=ON^
  --jerry-cmdline-snapshot=ON

echo step 4: copy jerry-snapshot bin
cd ../
md win
xcopy /e/y/i/f ..\jerryscript\build\bin\MinSizeRel\jerry.exe .\win\
xcopy /e/y/i/f ..\jerryscript\build\bin\MinSizeRel\jerry-snapshot.exe .\win\

pause
