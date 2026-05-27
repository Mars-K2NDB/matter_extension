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