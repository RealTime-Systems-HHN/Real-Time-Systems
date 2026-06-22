#include "app_azure_rtos.h"
#include "tx_api.h"

static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE];
static TX_BYTE_POOL tx_app_byte_pool;

VOID tx_application_define(VOID *first_unused_memory)
{
    tx_byte_pool_create(&tx_app_byte_pool,
                        "Tx App memory pool",
                        tx_byte_pool_buffer,
                        TX_APP_MEM_POOL_SIZE);
}