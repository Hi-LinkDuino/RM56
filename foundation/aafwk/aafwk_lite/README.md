# Ability Management Framework<a name="EN-US_TOPIC_0000001062157546"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section1464106163817)
-   [Usage](#section1954314201620)
-   [Repositories Involved](#section93061357133720)

## Introduction<a name="section11660541593"></a>

The ability management framework is provided by OpenHarmony for you to develop OpenHarmony applications. The following figure shows the architecture of the ability management framework.

**Figure 1**  Ability management framework

![](figures/en-us_image_0000001133175707.png)

-   **AbilityKit**  is a development kit provided by the ability management framework. You can use this kit to develop applications based on the  **Ability**  component. There are two types of applications developed based on the  **Ability**  component:  **JS Ability**  developed using the JavaScript language and  **Native Ability**  developed using the C/C++ language. The  **JS application development framework**  encapsulates JavaScript UI components on the basis of the AbilityKit and is used to help you quickly develop JS Ability-based applications.
-   **Ability**  is the minimum unit for the system to schedule applications. It is a component that can implement an independent functionality. An application can contain one or more  **Ability**  instances. There are two types of templates that you can use to create an  **Ability**  instance: Page and Service.
    -   An  **Ability using the Page template**  \(Page ability for short\) provides a UI for interacting with users.
    -   An  **Ability using the Service template**  \(Service ability for short\) does not have a UI and is used for running background tasks.

-   An  **AbilitySlice**  represents a single screen and its control logic. It is specific to Page abilities. A Page ability may contain one ability slice or multiple ability slices that provide highly relevant capabilities. The following figure shows the relationship between a Page ability and its ability slices.

**Figure 2**  Relationship between a Page ability and its ability slices

![](figures/en-us_image_0000001085773976.gif)

-   **Lifecycle**  is a general term for all states of an ability, including  **UNINITIALIZED**,  **INITIAL**,  **INACTIVE**,  **ACTIVE**, and  **BACKGROUND**. The following figure shows the lifecycle state transition of an ability.

**Figure 3**  Lifecycle state transition of a Page ability

![](figures/en-us_image_0000001086697634.png)

-   Description of ability lifecycle states:
    -   **UNINITIALIZED**: The ability is not initialized. This state is a temporary state. An ability changes directly to the  **INITIAL**  state upon its creation.

    -   **INITIAL**: This state refers to the initial or stopped state. The ability in this state is not running. The ability enters the  **INACTIVE**  state after it is started.

    -   **INACTIVE**: The ability is visible but does not gain focus.

    -   **ACTIVE**: The ability is in the foreground and has focus. The ability changes from the  **ACTIVE**  state to the  **INACTIVE**  state before returning to the background.

    -   **BACKGROUND**: The ability returns to the background. After being re-activated, the ability enters the  **ACTIVE**  state. After being destroyed, the ability enters the  **INITIAL**  state.

-   **AbilityLoader**  is used to register and load  **Ability**  classes. After creating an  **Ability**  class, you should first call the registration API defined in  **AbilityLoader**  to register the  **Ability**  class name with the ability management framework so that this  **Ability**  can be instantiated when being started.
-   **AbilityManager**  enables inter-process communication \(IPC\) between the AbilityKit and the Ability Manager Service.
-   **EventHandler**  is provided by the AbilityKit to enable inter-thread communication between abilities.
-   The  **Ability Manager Service**  is a system service used to coordinate the running relationships and lifecycle states of  **Ability**  instances. It consists of the following sub-modules:
    -   The  **service startup**  sub-module starts and registers the Ability Manager Service.
    -   The  **service interface management**  sub-module manages external capabilities provided by the Ability Manager Service.
    -   The  **process management**  sub-module starts and destroys processes where  **Ability**  instances are running, and maintains the process information.
    -   The  **ability stack management**  sub-module maintains the presentation sequence of abilities in the stack.
    -   The  **lifecycle scheduling**  sub-module changes an ability to a particular state based on the current operation of the system.
    -   The  **connection management**  sub-module manages connections to Service abilities.

-   **AppSpawn**  is a system service used to create the process for running an ability. This service has high permissions. It sets permissions for  **Ability**  instances and pre-loads some common modules to accelerate application startup.

## Directory Structure<a name="section1464106163817"></a>

```
/foundation/aafwk/aafwk_lite
     ├── frameworks
     │      ├── ability_lite               # Core implementation code of AbilityKit
     │      ├── abilitymgr_lite            # Client code used for communication between the AbilityKit and Ability Manager Service
     │      └── want_lite                  # Implementation code of the information carrier for interaction between abilities
     ├── interfaces
     │      ├── kits
     │      │     ├── ability_lite        # AbilityKit APIs exposed externally
     │      │     └── want_lite           # External APIs of the information carrier for interaction between abilities
     │      └── innerkits
     │             └── abilitymgr_lite     # Internal APIs provided by the Ability Manager Service for other subsystems
     └── services
             └── abilitymgr_lite            # Implementation code of the Ability Manager Service
```

## Usage<a name="section1954314201620"></a>

-   The Ability Manager Service is running in the foundation process.
-   The Ability Manager Service is registered with  **sa\_manager**.  **sa\_manager**  runs in the foundation process and sets up a thread runtime environment for the service. For details about how to create and use the Ability Manager Service, see  [SA Framework](en-us_topic_0000001051589563.md).
-   The Ability Manager Service starts upon OS startup.

-   After the HAP installation is complete, you can use the aa tool to run the demo for starting the specified ability through the following command. \(Taking  **hispark\_taurus**  as an example, you can obtain the aa tool from the  **out/hispark\_taurus/ipcamera\_hispark\_taurus/dev\_tools/bin**  directory after the version building.\)

```
./bin/aa start -p com.xxxxxx.hiability -n MainAbility
```

## Repositories Involved<a name="section93061357133720"></a>

[Application framework](https://gitee.com/openharmony/docs/blob/master/en/readme/application-framework.md)

**aafwk\_aafwk\_lite**

[appexecfwk\_appexecfwk\_lite](https://gitee.com/openharmony/appexecfwk_appexecfwk_lite/blob/master/README.md)

