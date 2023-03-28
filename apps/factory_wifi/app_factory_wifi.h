#ifndef _APP_FACTORY_WIFI_H_
#define _APP_FACTORY_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

 void app_factorymode_wifi_nosignalingtest(void const *arg);
 void app_wifi_detail_test(void const *arg);

 void app_factory_enter_wifi_nosignaltest_mode(void);
 void app_factory_exit_wifi_nosignaltest_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_FACTORY_WIFI_H_ */