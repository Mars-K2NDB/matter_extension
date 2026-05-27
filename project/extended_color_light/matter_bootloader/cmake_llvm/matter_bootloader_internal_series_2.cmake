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