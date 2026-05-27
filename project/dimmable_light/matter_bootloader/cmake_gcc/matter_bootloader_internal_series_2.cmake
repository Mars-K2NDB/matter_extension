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