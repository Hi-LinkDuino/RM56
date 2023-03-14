**USB HDI API 接口**

| C++接口申明                                                  | C++接口描述                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| int32_t OpenDevice(const UsbDev &dev);                       | 打开设备，建立连接                                           |
| int32_t CloseDevice(const UsbDev &dev);                      | 关闭设备，释放与设备相关的所有系统资源                       |
| int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor); | 获取设备描述符                                               |
| int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor); | 获取设备的配置描述符                                         |
| int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor); | 获取设备的字符串描述符                                       |
| int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);   | 设置设备当前使用的配置，通过配置值进行指定                   |
| int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);  | 获取当前生效的配置值                                         |
| int32_t ClaimInterface(const UsbDev &dev, uint8_t interfaceid); | 打开接口，并申明独占接口，必须在数据传输前执行               |
| int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid); | 关闭接口，释放接口的占用，在停止数据传输后执行               |
| int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex); | 设置指定接口的备选设置，用于在具有相同ID但不同备用设置的两个接口之间进行选择 |
| int32_t BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data); | 在给定端点上执行批量数据读取， 返回读取的数据和长度，端点方向必须为数据读取 |
| int32_t BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, const std::vector<uint8_t> &data); | 在给定端点上执行批量数据写入， 返回读取的数据和长度，端点方向必须为数据写入 |
| int32_t ControlTransfer(const UsbDev &dev, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data); | 对此设备执行端点零的控制事务，传输方向由请求类型决定。 如果requestType＆ USB_ENDPOINT_DIR_MASK是USB_DIR_OUT ，则传输是写入，如果是USB_DIR_IN ，则传输是读取。 |
| int32_t InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, <br/>        std::vector<uint8_t> &data); | 在给定端点上执行中断数据读取， 返回读取的数据和长度，端点方向必须为数据读取 |
| int32_t InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, <br/>        std::vector<uint8_t> &data); | 在给定端点上执行中断数据写入， 返回读取的数据和长度，端点方向必须为数据写入 |
| int32_t IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data); | 在给定端点上执行等时数据读取， 返回读取的数据和长度，端点方向必须为数据读取 |
| int32_t IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data); | 在给定端点上执行等时数据写入， 返回读取的数据和长度，端点方向必须为数据写入 |
| int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &clientData, <br/>        const std::vector<uint8_t> &buffer); | 将指定的端点进行异步数据发送或者接收请求，数据传输方向由端点方向决定 |
| int32_t<br/>        RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer, int32_t timeout); | 等待RequestQueue异步请求的操作结果                           |
| int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe); | 取消待处理的数据请求                                         |
| int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole); | 设置端口工作模式（从设备）                                   |
| int32_t GetCurrentFunctions(int32_t &funcs);                 | 获取从设备支持的功能列表（按位域表示）（从设备）             |
| int32_t SetCurrentFunctions(int32_t funcs);                  | 设置从设备支持的功能列表（按位域表示）（从设备）             |
| int32_t QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode); | 查询端口的当前设置（从设备）                                 |
| int32_t BindUsbdSubscriber(const sptr<UsbdSubscriber> &subscriber); | 绑定订阅者                                                   |
| int32_t UnbindUsbdSubscriber();                              | 解绑订阅者                                                   |
