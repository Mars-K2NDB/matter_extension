// Patched for generic-switch-remote (4 Switch endpoints)
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
