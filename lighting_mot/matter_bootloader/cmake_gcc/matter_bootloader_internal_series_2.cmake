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
    "${SDK_PATH}/platform_common/platform/common/src/sl_syscalls.c"
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
    "-Wl,--start-group"
    "gcc"
    "c"
    "m"
    "nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:C>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:C>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:C>:-fno-lto>
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-mcmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Os>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-lto>
    $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=hard>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_bootloader_internal_series_2.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>")
set_property(TARGET slc PROPERTY C_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.ld
    --specs=nano.specs
    "SHELL:-Xlinker -Map=$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>/matter_bootloader_internal_series_2.map"
    -Wl,--no-warn-rwx-segment
    "SHELL:-Wl,--wrap=_free_r -Wl,--wrap=_malloc_r -Wl,--wrap=_calloc_r -Wl,--wrap=_realloc_r"
    -fno-lto
    -Wl,--gc-sections
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA