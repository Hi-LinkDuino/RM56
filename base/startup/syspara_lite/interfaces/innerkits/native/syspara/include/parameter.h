/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STARTUP_SYSPARAM_PARAMETER_API_H
#define STARTUP_SYSPARAM_PARAMETER_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Obtains a system parameter matching the specified <b>key</b>.
 *
 * If no system parameter is found, the <b>def</b> parameter will be returned.\n
 *
 * @param key Indicates the key for the system parameter to query.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 32 bytes (including the end-of-text character in the string).
 * @param def Indicates the default value to return when no query result is found.
 * This parameter is specified by the caller.
 * @param value Indicates the data buffer that stores the query result.
 * This parameter is applied for and released by the caller and can be used as an output parameter.
 * @param len Indicates the length of the data in the buffer.
 * @return Returns the number of bytes of the system parameter if the operation is successful;
 * returns <b>-9</b> if a parameter is incorrect; returns <b>-1</b> in other scenarios.
 * @since 1
 * @version 1
 */
int GetParameter(const char *key, const char *def, char *value, unsigned int len);

/**
 * @brief Sets or updates a system parameter.
 *
 * You can use this function to set a system parameter that matches <b>key</b> as <b>value</b>.\n
 *
 * @param key Indicates the key for the parameter to set or update.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 32 bytes (including the end-of-text character in the string).
 * @param value Indicates the system parameter value.
 * Its length cannot exceed 128 bytes (including the end-of-text character in the string).
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-9</b> if a parameter is incorrect; returns <b>-1</b> in other scenarios.
 * @since 1
 * @version 1
 */
int SetParameter(const char *key, const char *value);

/**
 * @brief Wait for a system parameter with specified value.
 *
 * You can use this function to wait a system parameter that matches <b>key</b> as <b>value</b>.\n
 *
 * @param key Indicates the key for the parameter to wait.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 96 bytes (including the end-of-text character in the string).
 * @param value Indicates the system parameter value.
 * Its length cannot exceed 96 bytes (including the end-of-text character in the string).
 * value can use "*" to do arbitrary match.
 * @param timeout Indicates the timeout value, in seconds.
 * <=0 means wait for ever.
 * >0 means wait for specified seconds
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-10</b> if timeout; returns <b>-1</b> in other scenarios.
 * @since 1.1
 * @version 1.1
 */
int WaitParameter(const char *key, const char *value, int timeout);

/**
 * @brief Watch for system parameter values.
 *
 * You can use this function to watch system parameter values.\n
 *
 * @param keyprefix Indicates the key prefix for the parameter to be watched.
 * If keyprefix is not a full name, "A.B." for example, it means to watch for all parameter started with "A.B.".
 * @param callback Indicates value change callback.
 * If callback is NULL, it means to cancel the watch.
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-1</b> in other scenarios.
 * @since 1.1
 * @version 1.1
 */
typedef void (*ParameterChgPtr)(const char *key, const char *value, void *context);
int WatchParameter(const char *keyprefix, ParameterChgPtr callback, void *context);

const char *GetSecurityPatchTag(void);
const char *GetOSFullName(void);
const char *GetVersionId(void);
const char *GetBuildRootHash(void);
const char *GetOsReleaseType(void);
int GetSdkApiVersion(void);

const char *GetDeviceType(void);
const char *GetProductModel(void);
const char *GetManufacture(void);
const char *GetBrand(void);
const char *GetMarketName(void);
const char *GetProductSeries(void);
const char *GetSoftwareModel(void);
const char *GetHardwareModel(void);
const char *GetHardwareProfile(void);
const char *GetSerial(void);
const char *GetAbiList(void);
const char *GetDisplayVersion(void);
const char *GetIncrementalVersion(void);
const char *GetBootloaderVersion(void);
const char *GetBuildType(void);
const char *GetBuildUser(void);
const char *GetBuildHost(void);
const char *GetBuildTime(void);
int GetFirstApiVersion(void);
int GetDevUdid(char *udid, int size);

const char *AclGetSerial(void);
int AclGetDevUdid(char *udid, int size);

/**
 * @brief Obtains a system parameter matching the specified <b>key</b>.
 *
 * If no system parameter is found, return -1.\n
 *
 * @param key Indicates the key for the system parameter to find.
 * @return Returns the index for parameter;
 * returns <b>handle</b> if a parameter is correct; returns <b>-1</b> in other scenarios.
 * @since 1
 * @version 1
 */
unsigned int FindParameter(const char *key);
unsigned int GetParameterCommitId(unsigned int handle);
int GetParameterName(unsigned int handle, char *key, unsigned int len);
int GetParameterValue(unsigned int handle, char *value, unsigned int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // STARTUP_SYSPARAM_PARAMETER_API_H
