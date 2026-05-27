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
