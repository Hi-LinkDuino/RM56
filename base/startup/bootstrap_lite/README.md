# bootstrap\_lite<a name="EN-US_TOPIC_0000001082300996"></a>

-   [Introduction](#section469617221261)
-   [Directory Structure](#section1464106163817)
-   [Constraints](#section12212842173518)
-   [Usage](#section1483211215513)
-   [Repositories Involved](#section641143415335)

## Introduction<a name="section469617221261"></a>

The bootstrap\_lite module provides entry identifiers for starting services and features. When Samgr starts, it will call the entry function identified by boostrap and start system services.

## Directory Structure<a name="section1464106163817"></a>

```
base/startup/bootstrap_lite/    # bootstrap_lite module
└── services
    └── source               # Source files for the bootstrap_lite module
```

## Constraints<a name="section12212842173518"></a>

Currently, the bootstrap\_lite module applies only to mini-system devices \(reference memory ≥ 128 KB\), for example, Hi3861 V100.

## Usage<a name="section1483211215513"></a>

When Samgr starts, the bootstrap\_lite module is automatically invoked to start system services, without the need to be configured.

## Repositories Involved<a name="section641143415335"></a>

[Startup subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/startup.md)

[startup\_syspara\_lite](https://gitee.com/openharmony/startup_syspara_lite/blob/master/README.md)

[startup\_appspawn\_lite](https://gitee.com/openharmony/startup_appspawn_lite/blob/master/README.md)

**[startup\_bootstrap\_lite](https://gitee.com/openharmony/startup_bootstrap_lite/blob/master/README.md)**

[startup\_init\_lite](https://gitee.com/openharmony/startup_init_lite/blob/master/README.md)

