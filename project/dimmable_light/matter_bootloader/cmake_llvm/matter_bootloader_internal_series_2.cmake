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
    "SLI_BUILT_WITH_LLVM=1"
    "SL_TRUSTZONE_SECURE=1"
)

target_link_libraries(slc PUBLIC
    "-Wl,--start-group"
    "c"
    "m"
    "nosys"
    "crt0-nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    "$<$<COMPILE_LANGUAGE:C>:SHELL:--target=arm-none-eabi -mcpu=cortex-m33>"
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Oz>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:C>:--config=newlib-nano.cfg>
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:--target=arm-none-eabi -mcpu=cortex-m33>"
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-mcmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Oz>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:--config=newlib-nano.cfg>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:--target=arm-none-eabi -mcpu=cortex-m33>"
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:ASM>:--config=newlib-nano.cfg>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./matter_bootloader_internal_series_2.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>")
set_property(TARGET slc PROPERTY C_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    "SHELL:--target=arm-none-eabi -mcpu=cortex-m33"
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.ld
    --config=newlib-nano.cfg
    -Oz
    "SHELL:-Xlinker -Map=$<TARGET_FILE_DIR:matter_bootloader_internal_series_2>/matter_bootloader_internal_series_2.map"
    -nostartfiles
    "SHELL:-Wl,--wrap=_free_r -Wl,--wrap=_malloc_r -Wl,--wrap=_calloc_r -Wl,--wrap=_realloc_r"
    "SHELL:-Xlinker --gc-sections"
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA