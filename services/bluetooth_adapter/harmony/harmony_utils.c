#include "harmony_utils.h"

BtStatus bes_status_to_ohos(bt_status_t bes_status)
{
    BtStatus status = OHOS_BT_STATUS_SUCCESS;

    switch(bes_status){
        case BT_STS_SUCCESS:
            status = OHOS_BT_STATUS_SUCCESS;
            break;
            
        case BT_STS_FAILED:
            status = OHOS_BT_STATUS_FAIL;
            break;
            
        case  BT_STS_PENDING:
            status = OHOS_BT_STATUS_SUCCESS;
            break;
            
        case BT_STS_BUSY:
            status = OHOS_BT_STATUS_SUCCESS;
            break;
            
        case BT_STS_NO_RESOURCES:
             status = OHOS_BT_STATUS_NOMEM;
            break;

        case BT_STS_INVALID_PARM:
            status = OHOS_BT_STATUS_PARM_INVALID;
            break;

        case BT_STS_IN_PROGRESS:
            status = OHOS_BT_STATUS_BUSY;
            break;

        default:
            status = OHOS_BT_STATUS_FAIL;
            break;
            
    }
    return status;
}

