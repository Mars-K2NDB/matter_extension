/***************************************************************************//**
 * @file
 * @brief PA Tables for RF-BM-MG24B2 (EFR32MG24A420, 19.5 dBm PA).
 ******************************************************************************/

#ifndef SL_RAIL_UTIL_PA_TABLES_CONFIG_H
#define SL_RAIL_UTIL_PA_TABLES_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> PA Table Configuration
// <o SL_RAIL_UTIL_PA_TABLE_HEADER> PA Table Selection
// <"sl_rail_util_pa_dbm_powersetting_mapping_table_20dbm.h"=> 20dBm PA powersetting mapping table
// <i> Default: "sl_rail_util_pa_dbm_powersetting_mapping_table_20dbm.h"
#define SL_RAIL_UTIL_PA_TABLE_HEADER    "sl_rail_util_pa_dbm_powersetting_mapping_table_20dbm.h"
// </h>

// <<< end of configuration section >>>

#include SL_RAIL_UTIL_PA_TABLE_HEADER

#endif // SL_RAIL_UTIL_PA_TABLES_CONFIG_H
