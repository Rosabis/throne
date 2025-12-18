#include "include/configs/outbounds/juicity.h"

#include <QUrlQuery>
#include <include/global/Utils.hpp>

#include "include/configs/common/utils.h"

namespace Configs {
    bool juicity::ParseFromLink(const QString& link)
    {
        auto url = QUrl(link);
        if (!url.isValid()) return false;

        // Parse juicity:// format
        if (url.scheme() != "juicity") return false;

        outbound::ParseFromLink(link);
        uuid = url.userName();
        password = url.password();

        QUrlQuery query(url.query());
        if (query.hasQueryItem("congestion_control")) {
            congestion_control = query.queryItemValue("congestion_control");
            query.removeAllQueryItems("congestion_control");
        }
        if (query.hasQueryItem("udp_relay_mode")) {
            udp_relay_mode = query.queryItemValue("udp_relay_mode");
            query.removeAllQueryItems("udp_relay_mode");
        }
        if (query.hasQueryItem("udp_over_stream")) {
            udp_over_stream = query.queryItemValue("udp_over_stream") == "true" || query.queryItemValue("udp_over_stream") == "1";
            query.removeAllQueryItems("udp_over_stream");
        }
        if (query.hasQueryItem("zero_rtt_handshake")) {
            zero_rtt_handshake = query.queryItemValue("zero_rtt_handshake") == "true" || query.queryItemValue("zero_rtt_handshake") == "1";
            query.removeAllQueryItems("zero_rtt_handshake");
        }
        if (query.hasQueryItem("heartbeat")) {
            heartbeat = query.queryItemValue("heartbeat");
            query.removeAllQueryItems("heartbeat");
        }

        // 其余未识别字段原样保留，避免丢失 sni、allow_insecure、pinned_certchain_sha256 等参数
        extra_params = query.toString(QUrl::FullyEncoded);

        return !(uuid.isEmpty() || password.isEmpty() || server.isEmpty());
    }

    bool juicity::ParseFromJson(const QJsonObject& object)
    {
        if (object.isEmpty() || object["type"].toString() != "juicity") return false;
        outbound::ParseFromJson(object);
        if (object.contains("uuid")) uuid = object["uuid"].toString();
        if (object.contains("password")) password = object["password"].toString();
        if (object.contains("congestion_control")) congestion_control = object["congestion_control"].toString();
        if (object.contains("udp_relay_mode")) udp_relay_mode = object["udp_relay_mode"].toString();
        if (object.contains("udp_over_stream")) udp_over_stream = object["udp_over_stream"].toBool();
        if (object.contains("zero_rtt_handshake")) zero_rtt_handshake = object["zero_rtt_handshake"].toBool();
        if (object.contains("heartbeat")) heartbeat = object["heartbeat"].toString();
        if (object.contains("extra_params")) extra_params = object["extra_params"].toString();
        return true;
    }

    QString juicity::ExportToLink()
    {
        QUrl url;
        url.setScheme("juicity");
        url.setUserName(uuid);
        url.setPassword(password);
        url.setHost(server);
        url.setPort(server_port);
        
        QUrlQuery query;
        if (!congestion_control.isEmpty()) query.addQueryItem("congestion_control", congestion_control);
        if (!udp_relay_mode.isEmpty()) query.addQueryItem("udp_relay_mode", udp_relay_mode);
        if (udp_over_stream) query.addQueryItem("udp_over_stream", "true");
        if (zero_rtt_handshake) query.addQueryItem("zero_rtt_handshake", "true");
        if (!heartbeat.isEmpty()) query.addQueryItem("heartbeat", heartbeat);
        if (!extra_params.isEmpty()) {
            // 合并额外参数（已经移除了已识别的键，不会重复）
            QUrlQuery extra(extra_params);
            const auto items = extra.queryItems();
            for (const auto &item : items) {
                query.addQueryItem(item.first, item.second);
            }
        }
        url.setQuery(query);
        
        if (!name.isEmpty()) url.setFragment(name);
        return url.toString(QUrl::FullyEncoded);
    }

    QJsonObject juicity::ExportToJson()
    {
        QJsonObject object;
        object["type"] = "juicity";
        mergeJsonObjects(object, outbound::ExportToJson());
        if (!uuid.isEmpty()) object["uuid"] = uuid;
        if (!password.isEmpty()) object["password"] = password;
        if (!congestion_control.isEmpty()) object["congestion_control"] = congestion_control;
        if (!udp_relay_mode.isEmpty()) object["udp_relay_mode"] = udp_relay_mode;
        if (udp_over_stream) object["udp_over_stream"] = udp_over_stream;
        if (zero_rtt_handshake) object["zero_rtt_handshake"] = zero_rtt_handshake;
        if (!heartbeat.isEmpty()) object["heartbeat"] = heartbeat;
        if (!extra_params.isEmpty()) object["extra_params"] = extra_params;
        return object;
    }

    BuildResult juicity::Build()
    {
        // External juicity.exe provides local socks, sing-box only needs socks outbound.
        auto listenAddr = Configs::dataStore->juicity_socks_listen_addr.trimmed();
        if (listenAddr.isEmpty()) listenAddr = "127.0.0.1";
        int base = Configs::dataStore->juicity_socks_port_base;
        if (base <= 0) base = 31000;
        // Use stable hash to allocate a deterministic local port per node.
        uint h = qHash(server + ":" + Int2String(server_port) + ":" + uuid);
        int listenPort = base + (int)(h % 10000);
        if (listenPort <= 0 || listenPort > 65535) listenPort = 31000;

        // Build base outbound first, then override with local socks settings
        auto baseResult = outbound::Build();
        QJsonObject object = baseResult.object;
        object["type"] = "socks";
        // Override with local juicity.exe socks address/port (not remote server)
        object["server"] = listenAddr;
        object["server_port"] = listenPort;
        // Do not add auth here; juicity local socks usually doesn't require it.
        return {object, ""};
    }

    QString juicity::DisplayType()
    {
        return "Juicity";
    }

    // Juicity 在 sing-box 中是普通 outbound 类型，而不是 endpoint。
    // 这里返回 false，确保生成的配置放到 \"outbounds\" 数组里。
    bool juicity::IsEndpoint()
    {
        return false;
    }
}

