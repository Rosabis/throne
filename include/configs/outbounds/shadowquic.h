#pragma once

#include "include/configs/common/Outbound.h"

namespace Configs
{
    class shadowquic : public outbound
    {
    public:
        QString username;
        QString password;
        QString server_name;
        QStringList alpn;
        int initial_mtu = 1300;
        QString congestion_control;
        bool zero_rtt = true;
        bool over_stream = false;

        shadowquic() : outbound()
        {
            _add(new configItem("username", &username, string));
            _add(new configItem("password", &password, string));
            _add(new configItem("server_name", &server_name, string));
            _add(new configItem("alpn", &alpn, stringList));
            _add(new configItem("initial_mtu", &initial_mtu, integer));
            _add(new configItem("congestion_control", &congestion_control, string));
            _add(new configItem("zero_rtt", &zero_rtt, boolean));
            _add(new configItem("over_stream", &over_stream, boolean));
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


