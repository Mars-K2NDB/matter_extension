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