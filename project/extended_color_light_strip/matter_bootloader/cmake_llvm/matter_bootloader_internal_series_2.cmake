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