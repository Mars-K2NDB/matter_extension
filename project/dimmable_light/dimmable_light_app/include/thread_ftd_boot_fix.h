/*
 * Restore FTD link mode before Matter InitOpenThread().
 * Stale MTD link mode in flash causes OT_ERROR_NOT_CAPABLE (67108891) on SetRouterEligible.
 */

#pragma once

void EnsureThreadFtdLinkModeBeforeMatterInit();
