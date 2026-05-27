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

# BEGIN_SIMPLICITY_STUDIO_METADATA=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=END_SIMPLICITY_STUDIO_METADATA