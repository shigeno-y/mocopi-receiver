// SPDX-License-Identifier: MIT
#if !defined(SHIGENOY_MOCOPIPARSER_USDMOCOPIRECEIVER_TOKENS_HPP)
#    define SHIGENOY_MOCOPIPARSER_USDMOCOPIRECEIVER_TOKENS_HPP

#    include "pxr/base/tf/staticData.h"
#    include "pxr/base/tf/token.h"

namespace shigenoy::mocopi_parser {
struct PrivateTokens
{
public:
    // metadata for this plugin
    pxr::TfToken listen_host{ "listenHost", pxr::TfToken::Immortal };
    pxr::TfToken listen_port{ "listenPort", pxr::TfToken::Immortal };
    pxr::TfToken honor_frame_number{ "honorFrameNumber", pxr::TfToken::Immortal };

    // Root prim
    pxr::TfToken skels{ "Skels", pxr::TfToken::Immortal };

    // client prim
    pxr::TfToken skeleton{ "Skeleton", pxr::TfToken::Immortal };
    pxr::TfToken skel_animationSource{ "skel:animationSource", pxr::TfToken::Immortal };
    pxr::TfToken motion{ "Motion", pxr::TfToken::Immortal };
};
extern const pxr::TfStaticData<PrivateTokens> tokens;
} // namespace shigenoy::mocopi_parser

#endif
