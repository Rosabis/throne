#pragma once

#include "include/configs/common/Outbound.h"

namespace Configs
{
    class juicity : public outbound
    {
    public:
        QString uuid;
        QString password;
        QString congestion_control;
        QString udp_relay_mode;
        bool udp_over_stream = false;
        bool zero_rtt_handshake = false;
        QString heartbeat;
        // 保留未识别的查询参数（如 sni、allow_insecure、pinned_certchain_sha256 等），原样透传给 juicity 客户端
        QString extra_params;

        juicity() : outbound()
        {
            _add(new configItem("uuid", &uuid, string));
            _add(new configItem("password", &password, string));
            _add(new configItem("congestion_control", &congestion_control, string));
            _add(new configItem("udp_relay_mode", &udp_relay_mode, string));
            _add(new configItem("udp_over_stream", &udp_over_stream, boolean));
            _add(new configItem("zero_rtt_handshake", &zero_rtt_handshake, boolean));
            _add(new configItem("heartbeat", &heartbeat, string));
            _add(new configItem("extra_params", &extra_params, string));
        }

        // baseConfig overrides
        bool ParseFromLink(const QString& link) override;
        bool ParseFromJson(const QJsonObject& object) override;
        QString ExportToLink() override;
        QJsonObject ExportToJson() override;
        BuildResult Build() override;

        QString DisplayType() override;
        bool IsEndpoint() override;
    };
}

