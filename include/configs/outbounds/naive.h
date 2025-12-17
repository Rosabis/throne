#pragma once

#include "include/configs/common/Outbound.h"

namespace Configs
{
    class naive : public outbound
    {
    public:
        QString username;
        QString password;
        QString protocol = "https";
        QString extra_headers;
        QString sni;
        QString certificate;
        int insecure_concurrency = 0;
        bool disable_log = false;

        naive() : outbound()
        {
            _add(new configItem("username", &username, string));
            _add(new configItem("password", &password, string));
            _add(new configItem("protocol", &protocol, string));
            _add(new configItem("extra_headers", &extra_headers, string));
            _add(new configItem("sni", &sni, string));
            _add(new configItem("certificate", &certificate, string));
            _add(new configItem("insecure_concurrency", &insecure_concurrency, integer));
            _add(new configItem("disable_log", &disable_log, boolean));
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




