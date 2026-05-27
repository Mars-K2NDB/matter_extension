####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################

set(SDK_PATH "/home/mars/matter_extension/third_party/simplicity_sdk")
set(COPIED_SDK_PATH "simplicity_sdk_2025.12.2")
set(PKG_PATH "/home/mars/.silabs/slt/installs")

add_library(slc OBJECT
    "${SDK_PATH}/../matter_sdk/examples/platform/silabs/syscalls_stubs.cpp"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_bootload.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_core.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_main.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_parse.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/btl_reset.c"
    "${SDK_PATH}/bootloader/platform/bootloader/core/flash/btl_internal_flash.c"
    "${SDK_PATH}/bootloader/platform/bootloader/debug/btl_debug.c"
    "${SDK_PATH}/bootloader/platform/bootloader/debug/btl_debug_swo.c"
    "${SDK_PATH}/bootloader/platform/bootloader/driver/btl_driver_util.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/compression/btl_decompress_lz4.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/compression/btl_decompress_lzma.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/compression/lzma/LzmaDec.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_custom_tags.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_format.c"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/gbl/btl_gbl_parser.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_crc16.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_crc32.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_aes.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_ecdsa.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_sha256.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/btl_security_tokens.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/ecc/ecc.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/btl_sha256.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/crypto_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/cryptoacc_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/security/sha/se_sha.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/bootloadinfo/btl_storage_bootloadinfo.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/btl_storage.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/btl_storage_library.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/internal_flash/btl_storage_internal_flash.c"
    "${SDK_PATH}/bootloader/platform/bootloader/storage/internal_flash/btl_storage_internal_flash_raw.c"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32MG24/Source/startup_efr32mg24.c"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32MG24/Source/system_efr32mg24.c"
    "${SDK_PATH}/platform_common/platform/common/src/sl_assert.c"
    "${SDK_PATH}/platform_core/platform/common/src/sl_core_cortexm.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_acmp.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_burtc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_cmu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_dbg.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_emu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_eusart.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_gpcrc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_gpio.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_i2c.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_iadc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_ldma.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_letimer.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_msc.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_opamp.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_pcnt.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_prs.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_rmu.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_system.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_timer.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_usart.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_vdac.c"
    "${SDK_PATH}/platform_core/platform/emlib/src/em_wdog.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_dynamic_reservation.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_pool.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_pool_common.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_region.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sl_memory_manager_retarget.c"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src/sli_memory_manager_common.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/src/se_aes.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/src/sl_mbedtls.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_common.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_init.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_signature.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c"
    "${SDK_PATH}/security_mbedtls_source/library/aes.c"
    "${SDK_PATH}/security_mbedtls_source/library/constant_time.c"
    "${SDK_PATH}/security_mbedtls_source/library/platform.c"
    "${SDK_PATH}/security_mbedtls_source/library/platform_util.c"
    "${SDK_PATH}/security_mbedtls_source/library/psa_crypto_client.c"
    "${SDK_PATH}/security_mbedtls_source/library/psa_util.c"
    "${SDK_PATH}/security_mbedtls_source/library/threading.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_attestation.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_cipher.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_entropy.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_hash.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_key_derivation.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_key_handling.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_signature.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sl_se_manager_util.c"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/src/sli_se_manager_mailbox.c"
    "${SDK_PATH}/security_tfm/lib/fih/src/fih.c"
    "${SDK_PATH}/security_tfm/platform/ext/target/siliconlabs/hse/sli_se.c"
)

target_include_directories(slc PUBLIC
   "../config"
   "../autogen"
    "${SDK_PATH}/devices/platform/Device/SiliconLabs/EFR32MG24/Include"
    "${SDK_PATH}/platform_common/platform/common/inc"
    "${SDK_PATH}/bootloader/platform/bootloader"
    "${SDK_PATH}/bootloader/platform/bootloader/api"
    "${SDK_PATH}/bootloader/platform/bootloader/parser/compression"
    "${SDK_PATH}/bootloader/platform/bootloader/debug"
    "${SDK_PATH}/bootloader/platform/bootloader/parser"
    "${SDK_PATH}/bootloader/platform/bootloader/core/flash"
    "${SDK_PATH}/bootloader/platform/bootloader/security"
    "${SDK_PATH}/cmsis/Core/Include"
    "${SDK_PATH}/platform_core/platform/emlib/inc"
    "${SDK_PATH}/platform_core/platform/common/errno_error_codes/inc"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/config"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/config/preset"
    "${SDK_PATH}/security_mbedtls_source/include"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_mbedtls_support/inc"
    "${SDK_PATH}/security_mbedtls_source/library"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/inc"
    "${SDK_PATH}/platform_core/platform/service/memory_manager/src"
    "${SDK_PATH}/security_mbedtls/platform/security/sl_component/sl_psa_driver/inc"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/se_manager/inc"
    "${SDK_PATH}/platform_core/platform/common/inc"
    "${SDK_PATH}/security_tfm/lib/fih/inc"
    "${SDK_PATH}/security_tfm/platform/include"
    "${SDK_PATH}/security_se_manager/platform/security/sl_component/sli_psec_osal/inc"
)

target_compile_definitions(slc PUBLIC
    "EFR32MG24B210F1536IM48=1"
    "SL_CODE_COMPONENT_SYSTEM=system"
    "SE_MANAGER_CONFIG_FILE=\"btl_aes_ctr_stream_block_cfg.h\""
    "BTL_PARSER_SUPPORT_CUSTOM_TAGS=1"
    "BTL_PARSER_SUPPORT_LZMA=1"
    "_LZMA_SIZE_OPT=1"
    "BOOTLOADER_ENABLE=1"
    "BOOTLOADER_SECOND_STAGE=1"
    "SL_RAMFUNC_DISABLE=1"
    "__START=main"
    "__STARTUP_CLEAR_BSS=1"
    "SYSTEM_NO_STATIC_MEMORY=1"
    "BTL_PARSER_SUPPORT_LZ4=1"
    "BOOTLOADER_SUPPORT_INTERNAL_STORAGE=1"
    "BOOTLOADER_SUPPORT_STORAGE=1"
    "HARDWARE_BOARD_DEFAULT_RF_BAND_2400=1"
    "HARDWARE_BOARD_SUPPORTS_1_RF_BAND=1"
    "HARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1"
    "HFXO_FREQ=39000000"
    "SL_BOARD_NAME=\"BRD2703A\""
    "SL_BOARD_REV=\"A02\""
    "SL_COMPONENT_CATALOG_PRESENT=1"
    "MBEDTLS_CONFIG_FILE=<sl_mbedtls_trustzone_config.h>"
    "SL_CODE_COMPONENT_MEMORY_MANAGER=memory_manager"
    "MBEDTLS_PSA_CRYPTO_CONFIG_FILE=<psa_crypto_config.h>"
    "SL_CODE_COMPONENT_SE_MANAGER=se_manager"
    "SL_CODE_COMPONENT_CORE=core"
    "SL_CODE_COMPONENT_PSEC_OSAL=psec_osal"
    "SL_TRUSTZONE_SECURE=1"
)

target_link_libraries(slc PUBLIC
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:--cpu=Cortex-M33.no_dsp>
    $<$<COMPILE_LANGUAGE:C>:--cpu_mode=thumb>
    $<$<COMPILE_LANGUAGE:C>:--fpu=VFPv5_sp>
    $<$<COMPILE_LANGUAGE:C>:--endian=little>
    $<$<COMPILE_LANGUAGE:C>:--cmse>
    $<$<COMPILE_LANGUAGE:C>:-Ohz>
    $<$<COMPILE_LANGUAGE:C>:--use_c++_inline>
    $<$<COMPILE_LANGUAGE:C>:--debug>
    $<$<COMPILE_LANGUAGE:C>:-e>
    $<$<COMPILE_LANGUAGE:CXX>:--cpu=Cortex-M33.no_dsp>
    $<$<COMPILE_LANGUAGE:CXX>:--cpu_mode=thumb>
    $<$<COMPILE_LANGUAGE:CXX>:--fpu=VFPv5_sp>
    $<$<COMPILE_LANGUAGE:CXX>:--endian=little>
    $<$<COMPILE_LANGUAGE:CXX>:--cmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Ohz>
    $<$<COMPILE_LANGUAGE:CXX>:--use_c++_inline>
    $<$<COMPILE_LANGUAGE:CXX>:--debug>
    $<$<COMPILE_LANGUAGE:CXX>:-e>
    $<$<COMPILE_LANGUAGE:ASM>:--cpu=Cortex-M33.no_dsp>
    $<$<COMPILE_LANGUAGE:ASM>:--cpu_mode=thumb>
    $<$<COMPILE_LANGUAGE:ASM>:--fpu=VFPv5_sp>
    $<$<COMPILE_LANGUAGE:ASM>:--endian=little>
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_bootloader_internal_series_2.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>")

target_link_options(slc INTERFACE
    --config ${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.icf
    --map "$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>/matter_bootloader_internal_series_2.map"
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA
