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