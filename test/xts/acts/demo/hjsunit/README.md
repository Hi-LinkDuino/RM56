# App Samples

-   [Introduction](#section_HJSUnit_001)
-   [Usage](#section_HJSUnit_002)
-   [Limitations](#section_HJSUnit_003)

## Introduction<a name="section_HJSUnit_001"></a>

We provide a series of app samples to help you quickly get familiar with the test capabilities and how to use of the HJSUnit. Each app sample is an independent project in DevEco Studio. You can import sample code from a project to a customized project to learn how to use APIs by browsing code, building the project, and installing and running the app.

## Usage<a name="section_HJSUnit_002"></a>

1.  Development tool: Devco Studio 2.2.0.200 or later
2.  Procedure:  Create a blank JS template project in Devco Studio, import the sample code in the ohosTest directory of the sample project to the ohosTest directory of the user-defined project, and compile, build, run, and debug the project.
3.  After HAP installation and execution, you can view the execution effect of the sample on the device and then conduct debugging.

## Limitations<a name="section_HJSUnit_003"></a>

Before installing and running the sample, check the  **deviceType**  field in the  **config.json**  file to obtain the device types supported by the sample. You can modify this field to enable the sample to run on your target device. \(The  **config.json**  file is generally stored in the  **entry/src/main**  directory, which may be different depending on the samples.\)