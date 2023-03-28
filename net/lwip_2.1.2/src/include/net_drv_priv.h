#ifndef __NET_DRV_PRIV_H__
#define __NET_DRV_PRIV_H__
#include "cw1200.h"
#include "fwio.h"

struct wiphy_hw_priv {
    struct cw1200_common priv;
};

struct ieee80211_hw_vif_data {
   struct cw1200_vif vif;
};

#endif
