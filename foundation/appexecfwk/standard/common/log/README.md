## How To Use Log

# C++
1. Add a statement to your gn file

```
    deps = [
        "${common_path}/log:appexecfwk_log_source_set",
    ]

    defines = [
        "APP_LOG_TAG = \"Appexecfwk_Core\"",
        "LOG_DOMAIN = 0xD001110",
    ]
```
2. Include header file `#include "app_log_wrapper.h"`
3. Control log print level `AppLogWrapper::SetLogLevel(AppLogLevel::DEBUG);` default is DEBUG
4. Example output format `[fileName(functionName)] string`
```
    // dynamic control log level
    AppLogWrapper::SetLogLevel(AppLogLevel::FATAL);

    // The following log statement will not print
    APP_LOGD("one %{public}d", 1234);
    APP_LOGI("two %{public}s", "1234");
    APP_LOGW("three %{private}s", "1234");
    APP_LOGE("four %{private}s", "1234");

    // The following log statement will print
    AppLogWrapper::SetLogLevel(AppLogLevel::DEBUG);
    APP_LOGD("five %{public}d", 1234);
    APP_LOGI("six %{public}s", "1234");
    APP_LOGW("seven %{private}s", "1234");
    APP_LOGE("eight %{private}s", "1234");
```

# Java

1. import dependent in your class
```
import ohosos.appexecfwk.utils.AppLog;
import ohosos.hiviewdfx.HiLogLabel;
```

2. custom HiLogLabel (if you don't custom label, the default will be used)
```
private static final HiLogLabel LABEL = new HiLogLabel(HiLog.LOG_CORE, 0xD001110, "AppKit");
```

3. use AppLog interface
```
App_Log.d(LABEL, "log %{public}s", "123");
App_Log.i("log %{public}d", 123);
```