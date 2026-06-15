#!/usr/bin/env python3
"""修正 generic-switch 多 endpoint ZAP 生成的静态集群配置。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parents[1]
STATIC_CONFIG_DIR = (
    PROJECT_DIR
    / "battery_switch_remote_app"
    / "autogen"
    / "zap-generated"
    / "app"
    / "static-cluster-config"
)
DESCRIPTOR_H = STATIC_CONFIG_DIR / "Descriptor.h"
SWITCH_H = STATIC_CONFIG_DIR / "Switch.h"

# 与 ZAP endpoint_config.h 中 FeatureMap 默认值 30 一致
SWITCH_H_CONTENT = """\
// Patched for battery-switch-remote (4 Switch endpoints)
#pragma once

#include <app/util/cluster-config.h>
#include <clusters/Switch/AttributeIds.h>
#include <clusters/Switch/CommandIds.h>
#include <clusters/Switch/Enums.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace Switch {
namespace StaticApplicationConfig {
namespace detail {
inline constexpr AttributeId kEndpointSwitchEnabledAttributes[] = {
    Attributes::NumberOfPositions::Id,
    Attributes::CurrentPosition::Id,
    Attributes::MultiPressMax::Id,
    Attributes::FeatureMap::Id,
    Attributes::ClusterRevision::Id,
};
} // namespace detail

using FeatureBitmapType = Feature;

inline constexpr auto kMomentarySwitchFeatures = BitFlags<FeatureBitmapType>{
    FeatureBitmapType::kMomentarySwitch,
    FeatureBitmapType::kMomentarySwitchRelease,
    FeatureBitmapType::kMomentarySwitchLongPress,
    FeatureBitmapType::kMomentarySwitchMultiPress,
};

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 4> kFixedClusterConfig = { {
    {
        .endpointNumber    = 1,
        .featureMap        = kMomentarySwitchFeatures,
        .enabledAttributes = Span<const AttributeId>(detail::kEndpointSwitchEnabledAttributes),
        .enabledCommands   = Span<const CommandId>(),
    },
    {
        .endpointNumber    = 2,
        .featureMap        = kMomentarySwitchFeatures,
        .enabledAttributes = Span<const AttributeId>(detail::kEndpointSwitchEnabledAttributes),
        .enabledCommands   = Span<const CommandId>(),
    },
    {
        .endpointNumber    = 3,
        .featureMap        = kMomentarySwitchFeatures,
        .enabledAttributes = Span<const AttributeId>(detail::kEndpointSwitchEnabledAttributes),
        .enabledCommands   = Span<const CommandId>(),
    },
    {
        .endpointNumber    = 4,
        .featureMap        = kMomentarySwitchFeatures,
        .enabledAttributes = Span<const AttributeId>(detail::kEndpointSwitchEnabledAttributes),
        .enabledCommands   = Span<const CommandId>(),
    },
} };

inline constexpr bool IsAttributeEnabledOnSomeEndpoint(AttributeId attributeId)
{
    switch (attributeId)
    {
    case Attributes::ClusterRevision::Id:
    case Attributes::CurrentPosition::Id:
    case Attributes::FeatureMap::Id:
    case Attributes::MultiPressMax::Id:
    case Attributes::NumberOfPositions::Id:
        return true;
    default:
        return false;
    }
}

inline constexpr bool IsCommandEnabledOnSomeEndpoint(CommandId commandId)
{
    switch (commandId)
    {
    default:
        return false;
    }
}

} // namespace StaticApplicationConfig
} // namespace Switch
} // namespace Clusters
} // namespace app
} // namespace chip
"""


def patch_descriptor_h(path: Path) -> bool:
    if not path.is_file():
        return False
    text = path.read_text(encoding="utf-8")
    endpoint_blocks = len(re.findall(r"\.endpointNumber\s*=", text))
    if endpoint_blocks == 0:
        return False
    new_text, count = re.subn(
        r"(inline constexpr std::array<[^>]+>, )\d+(> kFixedClusterConfig)",
        rf"\g<1>{endpoint_blocks}\2",
        text,
        count=1,
    )
    if count == 0 or new_text == text:
        return False
    path.write_text(new_text, encoding="utf-8")
    print(f"已修正 Descriptor.h 数组大小为 {endpoint_blocks}: {path}")
    return True


def write_switch_h(path: Path) -> bool:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(SWITCH_H_CONTENT, encoding="utf-8")
    print(f"已生成 Switch.h (4 端点): {path}")
    return True


def main() -> int:
    changed = False
    if patch_descriptor_h(DESCRIPTOR_H):
        changed = True
    if write_switch_h(SWITCH_H):
        changed = True
    if not changed:
        print("无需修正或文件不存在", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
