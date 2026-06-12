// DO NOT EDIT - Generated file
//
// Application configuration for Groups based on EMBER configuration
#pragma once

#include <app/util/cluster-config.h>
#include <clusters/Groups/AttributeIds.h>
#include <clusters/Groups/CommandIds.h>
#include <clusters/Groups/Enums.h>

#include <array>

namespace chip {
namespace app {
namespace Clusters {
namespace Groups {
namespace StaticApplicationConfig {
namespace detail {
inline constexpr AttributeId kEndpoint1EnabledAttributes[] = {
    Attributes::NameSupport::Id,
    Attributes::GeneratedCommandList::Id,
    Attributes::AcceptedCommandList::Id,
    Attributes::AttributeList::Id,
    Attributes::FeatureMap::Id,
    Attributes::ClusterRevision::Id,
};
} // namespace detail

using FeatureBitmapType = Feature;

inline constexpr std::array<Clusters::StaticApplicationConfig::ClusterConfiguration<FeatureBitmapType>, 1> kFixedClusterConfig = { {
    {
        .endpointNumber    = 1,
        .featureMap        = BitFlags<FeatureBitmapType>{},
        .enabledAttributes = Span<const AttributeId>(detail::kEndpoint1EnabledAttributes),
        .enabledCommands   = Span<const CommandId>(),
    },
} };

// If a specific attribute is supported at all across all endpoint static instantiations
inline constexpr bool IsAttributeEnabledOnSomeEndpoint(AttributeId attributeId)
{
    switch (attributeId)
    {
    case Attributes::NameSupport::Id:
    case Attributes::GeneratedCommandList::Id:
    case Attributes::AcceptedCommandList::Id:
    case Attributes::AttributeList::Id:
    case Attributes::FeatureMap::Id:
    case Attributes::ClusterRevision::Id:
        return true;
    default:
        return false;
    }
}

// If a specific command is supported at all across all endpoint static instantiations
inline constexpr bool IsCommandEnabledOnSomeEndpoint(CommandId commandId)
{
    switch (commandId)
    {
    default:
        return false;
    }
}

} // namespace StaticApplicationConfig
} // namespace Groups
} // namespace Clusters
} // namespace app
} // namespace chip
