# Utils<a name="EN-US_TOPIC_0000001121139825"></a>

-   [Overview](#section62661387114)
-   [Directory Structure](#section1464106163817)
-   [Usage](#section25021123178)
-   [Repositories Involved](#section4571352889)

## Overview<a name="section62661387114"></a>

The Utils repository stores basic components of OpenHarmony. These basic components are used by OpenHarmony subsystems and upper-layer applications.

The Utils library provides the following capabilities on different platforms:

-   LiteOS Cortex-M \(Hi3861 platform\): key value \(KV\) store, file operations, timer, and system attribute dumping
-   LiteOS Cortex-A \(Hi3516 or Hi3518 platform\): KV store, timer, JavaScript APIs for device query and data storage, and system attribute dumping

**Table  1**  Utils capabilities

<a name="en-us_topic_0000001052623010_table206292206282"></a>
<table><thead align="left"><tr id="en-us_topic_0000001052623010_row8629020112819"><th class="cellrowborder" valign="top" width="21.22%" id="mcps1.2.4.1.1"><p id="en-us_topic_0000001052623010_p66291220192816"><a name="en-us_topic_0000001052623010_p66291220192816"></a><a name="en-us_topic_0000001052623010_p66291220192816"></a>Module</p>
</th>
<th class="cellrowborder" valign="top" width="24.77%" id="mcps1.2.4.1.2"><p id="en-us_topic_0000001052623010_p3629122014289"><a name="en-us_topic_0000001052623010_p3629122014289"></a><a name="en-us_topic_0000001052623010_p3629122014289"></a>Platform</p>
</th>
<th class="cellrowborder" valign="top" width="54.010000000000005%" id="mcps1.2.4.1.3"><p id="en-us_topic_0000001052623010_p206298206288"><a name="en-us_topic_0000001052623010_p206298206288"></a><a name="en-us_topic_0000001052623010_p206298206288"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="en-us_topic_0000001052623010_row1162992082812"><td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0000001052623010_p136291920192814"><a name="en-us_topic_0000001052623010_p136291920192814"></a><a name="en-us_topic_0000001052623010_p136291920192814"></a>KV store</p>
</td>
<td class="cellrowborder" valign="top" width="24.77%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0000001052623010_p5629112019284"><a name="en-us_topic_0000001052623010_p5629112019284"></a><a name="en-us_topic_0000001052623010_p5629112019284"></a>LiteOS Cortex-M and LiteOS Cortex-A</p>
</td>
<td class="cellrowborder" valign="top" width="54.010000000000005%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0000001052623010_p3630112072811"><a name="en-us_topic_0000001052623010_p3630112072811"></a><a name="en-us_topic_0000001052623010_p3630112072811"></a>Provides KV storage for applications.</p>
</td>
</tr>
<tr id="en-us_topic_0000001052623010_row116301920152816"><td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0000001052623010_p96300207286"><a name="en-us_topic_0000001052623010_p96300207286"></a><a name="en-us_topic_0000001052623010_p96300207286"></a>File operation</p>
</td>
<td class="cellrowborder" valign="top" width="24.77%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0000001052623010_p1463032012281"><a name="en-us_topic_0000001052623010_p1463032012281"></a><a name="en-us_topic_0000001052623010_p1463032012281"></a>LiteOS Cortex-M</p>
</td>
<td class="cellrowborder" valign="top" width="54.010000000000005%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0000001052623010_p163042052810"><a name="en-us_topic_0000001052623010_p163042052810"></a><a name="en-us_topic_0000001052623010_p163042052810"></a>Provides unified file operation interfaces that can be used on different underlying chip components.</p>
</td>
</tr>
<tr id="en-us_topic_0000001052623010_row1163022022812"><td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0000001052623010_p66308202284"><a name="en-us_topic_0000001052623010_p66308202284"></a><a name="en-us_topic_0000001052623010_p66308202284"></a>Timer</p>
</td>
<td class="cellrowborder" valign="top" width="24.77%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0000001052623010_p26301620192820"><a name="en-us_topic_0000001052623010_p26301620192820"></a><a name="en-us_topic_0000001052623010_p26301620192820"></a>LiteOS Cortex-M and LiteOS Cortex-A</p>
</td>
<td class="cellrowborder" valign="top" width="54.010000000000005%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0000001052623010_p2630172062815"><a name="en-us_topic_0000001052623010_p2630172062815"></a><a name="en-us_topic_0000001052623010_p2630172062815"></a>Provides unified timer operation interfaces that can be used on different underlying chip components.</p>
</td>
</tr>
<tr id="en-us_topic_0000001052623010_row363012202282"><td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0000001052623010_p4502105418285"><a name="en-us_topic_0000001052623010_p4502105418285"></a><a name="en-us_topic_0000001052623010_p4502105418285"></a>JavaScript API</p>
</td>
<td class="cellrowborder" valign="top" width="24.77%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0000001052623010_p199870135293"><a name="en-us_topic_0000001052623010_p199870135293"></a><a name="en-us_topic_0000001052623010_p199870135293"></a>LiteOS Cortex-A</p>
</td>
<td class="cellrowborder" valign="top" width="54.010000000000005%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0000001052623010_p1550005422814"><a name="en-us_topic_0000001052623010_p1550005422814"></a><a name="en-us_topic_0000001052623010_p1550005422814"></a>Provides JavaScript APIs for obtaining device information and storing data.</p>
</td>
</tr>
<tr id="en-us_topic_0000001052623010_row5553145162815"><td class="cellrowborder" valign="top" width="21.22%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0000001052623010_p106300200288"><a name="en-us_topic_0000001052623010_p106300200288"></a><a name="en-us_topic_0000001052623010_p106300200288"></a>System attribute dumping</p>
</td>
<td class="cellrowborder" valign="top" width="24.77%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0000001052623010_p16300204280"><a name="en-us_topic_0000001052623010_p16300204280"></a><a name="en-us_topic_0000001052623010_p16300204280"></a>LiteOS Cortex-M and LiteOS Cortex-A</p>
</td>
<td class="cellrowborder" valign="top" width="54.010000000000005%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0000001052623010_p1563018208286"><a name="en-us_topic_0000001052623010_p1563018208286"></a><a name="en-us_topic_0000001052623010_p1563018208286"></a>Provides the command line tool for dumping system attributes.</p>
</td>
</tr>
</tbody>
</table>

## Directory Structure<a name="section1464106163817"></a>

```
utils/native/lite/              # Root directory
├── file                        # Implementation of the file system APIs
├── hals                        # HAL directory
│   └── file                    # Header files of the hardware abstraction layer for file operations
├── include                     # Header files of external APIs
├── js                          # JavaScript APIs
│   └── builtin
│       ├── common              # Builtin common function
│       ├── deviceinfokit       # Device information kit
│       ├── filekit             # File kit
│       └── kvstorekit          # KV store kit
├── kal                         # KAL directory
│   └── timer                   # KAL implementation of the timer
├── kv_store	                # KV store implementation
│   ├── innerkits               # Internal KV store APIs
│   └── src	                    # KV store source file
├── memory
│   └── include                 # Memory pool management APIs
├── os_dump                     # System attribute dumping
└── timer_task                  # Timer implementation
```

## Usage<a name="section25021123178"></a>

-   **KV store**

    ```
    // Store or update the value of a key.
    const char key1[] = "key_sample";
    const char defValue[] = "test case of key value store.";
    int ret = UtilsSetValue(key1, defValue);
    
    // Obtain the value of the key.
    char value1[32] = {0};
    ret = UtilsGetValue(key1, value1, 32);
    
    // Delete the value of the key.
    UtilsDeleteValue(key1);
    ```

-   **File operation**

    ```
    // Open or create a file.
    const char fileName[] = "testfile";
    int fd = UtilsFileOpen(fileName, O_RDWR_FS | O_CREAT_FS | O_TRUNC_FS, 0);
    
    // Write a specified length of data into a file with the specified file descriptor.
    const char defValue[] = "test case of file system.";
    int ret = UtilsFileWrite(fd, defValue, strlen(defValue));
    
    // Close a file with a specified file descriptor.
    UtilsFileClose(fd);
    
    // Obtain the file size.
    int fileLen = 0;
    ret = UtilsFileStat(fileName, &fileLen);
    printf("file size = %d\n", fileLen);
    
    // Adjust the read and write position offset in a file.
    int fd1 = UtilsFileOpen(fileName, O_RDWR_FS, 0);
    ret = UtilsFileSeek(fd1, 5, SEEK_SET_FS);
    
    // Read a specified length of data from a file with the specified file descriptor and write the data into the buffer.
    char buf[32] = {0};
    int readLen = UtilsFileRead(fd1, buf, 32);
    ret = UtilsFileClose(fd1);
    printf("read len = %d : buf = %s\n", readLen, buf);
    
    // Delete a specified file.
    ret = UtilsFileDelete(fileName);
    ```


-   **System attribute dumping**

    LiteOS Cortex-M kernel: Run the following command over the serial port to dump the current system parameters:

    ```
    AT+SYSPARA
    ```

    LiteOS Cortex-A kernel: Run the  **os\_dump**  command in the  **bin**  directory to dump the current system parameters:

    ```
    ./bin/os_dump syspara
    ```


## Repositories Involved<a name="section4571352889"></a>

[Utils](https://gitee.com/openharmony/docs/blob/master/en/readme/utils.md)

utils\_native\_lite

