#ifndef __WEP_H__
#define __WEP_H__

extern bool ieee80211_wep_is_weak_iv(struct sk_buff *skb,
				     struct ieee80211_key *key);
extern void ieee80211_wep_free(struct ieee80211_local *local);
extern int ieee80211_wep_init(struct ieee80211_local *local);
extern int ieee80211_wep_encrypt(struct ieee80211_local *local,
				 struct sk_buff *skb,
				 const u8 *key, int keylen, int keyidx);
extern ieee80211_tx_result
ieee80211_crypto_wep_encrypt(struct ieee80211_tx_data *tx);
extern ieee80211_rx_result ieee80211_crypto_wep_decrypt(struct ieee80211_rx_data *rx);
#endif


