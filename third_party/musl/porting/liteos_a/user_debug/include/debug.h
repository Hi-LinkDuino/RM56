#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of the memory debug function, including the output method of memory debug information
 * and signal registration. You can use command line parameters, "--mwatch" or "--mrecord <full path>", to
 * call mem_check_init(char *) when executing your program.
 *
 * @param f_path   The full path of the file to be created where the memory debug information will be written.
 * If the param is NULL or the file creation fails, the memory debug information will be output via serial
 * port.
 *
 * @return void.
 */
void mem_check_init(char *f_path);

/**
 * @brief View thread-level heap memory usage information, signal registration index is 35, you can use "kill -35 pid"
 * to call watch_mem() when your program is running. The output way of memory debug information is determined by how
 * the mem_check_init(char *) interface is called.
 *
 * @param void.
 *
 * @return void.
 */
void watch_mem(void);

/**
 * @brief Check whether the heap memory leak is exist or not, signal registration index is 36, you can use "kill -36
 * pid" to call check_leak() when your program is running. The output way of memory debug information is determined by
 * how the mem_check_init(char *) interface is called.
 *
 * @param void.
 *
 * @return void.
 */
void check_leak(void);

/**
 * @brief Check whether the heap memory is integrited or not, signal registration index is 37, you can use "kill -37
 * pid" to call check_heap_integrity() when your program is running. The output way of memory debug information is
 * determined by how the mem_check_init(char *) interface is called.
 *
 * @param void.
 *
 * @return void.
 */
void check_heap_integrity(void);

/**
 * @brief Store the address of the call stack information, the max number is param size.
 *
 * @param buffer   The array to store address of the call stack information.
 * @param size     The size of buffer.
 *
 * @return The exact number of the address.
 */
int backtrace(void **buffer, int size);

/**
 * @brief Find the symbol information corresponding to the address stored in the buffer for dynamic linking.
 *
 * @param buffer   The array stored address of the exact number.
 * @param size     The exact number of the address stored in the buffer.
 *
 * @return The pointer to the memory allocated from heap holds the symbol information corresponding to the address
 * stored in the buffer. You should free the memory the pointer points to after calling backtrace_symbols().
 */
char **backtrace_symbols(void *const *buffer, int size);

/**
 * @brief Print the call stack information of the function calling print_trace().
 *
 * @param void.
 *
 * @return void.
 */
void print_trace(void);

#ifdef __cplusplus
}
#endif

#endif
