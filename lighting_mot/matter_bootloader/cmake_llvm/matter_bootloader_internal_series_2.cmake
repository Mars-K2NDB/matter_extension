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

# BEGIN_SIMPLICITY_STUDIO_METADATA=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=END_SIMPLICITY_STUDIO_METADATA