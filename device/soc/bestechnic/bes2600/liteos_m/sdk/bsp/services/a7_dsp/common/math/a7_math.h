#ifndef A7_MATH_H
#define A7_MATH_H

#ifdef __cplusplus
extern "C" {
#endif


typedef void (* a7_test_callback_t)(char *data, int len);
void a7_math_init(void);
void a7_math_send_to_mcu(void *data, int len);
// void set_a7_math_callback_handler (a7_test_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // A7_MATH_H
