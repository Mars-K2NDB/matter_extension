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

# BEGIN_SIMPLICITY_STUDIO_METADATA=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=END_SIMPLICITY_STUDIO_METADATA
