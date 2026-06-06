/***************************************************************************//**
 * @file
 * @brief PA Tables for RF-BM-MG24B1 (EFR32MG24A410) (EFR32MG24B210, 10 dBm PA).
 ******************************************************************************/

#ifndef SL_RAIL_UTIL_PA_TABLES_CONFIG_H
#define SL_RAIL_UTIL_PA_TABLES_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> PA Table Configuration
// <o SL_RAIL_UTIL_PA_TABLE_HEADER> PA Table Selection
// <"sl_rail_util_pa_dbm_powersetting_mapping_table_10dbm.h"=> 10dBm PA powersetting mapping table
// <"sl_rail_util_pa_dbm_powersetting_mapping_table_0dbm.h"=> 0dBm PA powersetting mapping table
// <"sl_rail_util_pa_dbm_powersetting_mapping_table_automode_0_10dbm.h"=> 0dBm-10dBm automode PA powersetting mapping table
// <i> Default: "sl_rail_util_pa_dbm_powersetting_mapping_table_automode_0_10dbm.h"
#define SL_RAIL_UTIL_PA_TABLE_HEADER    "sl_rail_util_pa_dbm_powersetting_mapping_table_automode_0_10dbm.h"
// </h>

// <<< end of configuration section >>>

#include SL_RAIL_UTIL_PA_TABLE_HEADER

#endif // SL_RAIL_UTIL_PA_TABLES_CONFIG_H
