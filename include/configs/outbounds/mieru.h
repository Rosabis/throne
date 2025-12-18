#pragma once

#include "include/configs/common/Outbound.h"

namespace Configs
{
    class mieru : public outbound
    {
    public:
        QString username;
        QString password;
        QString profile_name = "default";
        int mtu = 1400;
        QString multiplexing = "MULTIPLEXING_LOW"; // MULTIPLEXING_OFF, MULTIPLEXING_LOW, MULTIPLEXING_MIDDLE, MULTIPLEXING_HIGH
        QString handshake_mode = "HANDSHAKE_STANDARD"; // HANDSHAKE_STANDARD, HANDSHAKE_NO_WAIT
        QStringList port_bindings; // Format: "port" or "portRange" (e.g., "2012-2022")
        QStringList protocols; // "TCP" or "UDP", must match port_bindings count

        mieru() : outbound()
        {
            _add(new configItem("username", &username, string));
            _add(new configItem("password", &password, string));
            _add(new configItem("profile_name", &profile_name, string));
            _add(new configItem("mtu", &mtu, integer));
            _add(new configItem("multiplexing", &multiplexing, string));
            _add(new configItem("handshake_mode", &handshake_mode, string));
            _add(new configItem("port_bindings", &port_bindings, stringList));
            _add(new configItem("protocols", &protocols, stringList));
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

