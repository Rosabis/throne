#include "include/configs/outbounds/juicity.h"

#include <QUrlQuery>
#include <include/global/Utils.hpp>

#include "include/configs/common/utils.h"

namespace Configs {
    bool juicity::ParseFromLink(const QString& link)
    {
        // 先用 QUrl 校验基本格式和拿到 fragment（节点名称），然后手动解析 authority，兼容官方 juicity / tuic 链接格式：
        // juicity://uuid:password@example.com:port?congestion_control=...
        // juicity://uuid:password@122.12.31.66:port?...
        QUrl url(link);
        if (!url.isValid() || url.scheme() != "juicity")
            return false;

        // 解析名称（#后面的备注）
        if (url.hasFragment())
            name = url.fragment(QUrl::FullyDecoded);

        // 去掉前缀和 query，只保留 authority 部分：uuid:password@example.com:port / uuid:password@ip:port
        QString withoutScheme = link.mid(QStringLiteral("juicity://").size());
        int qPos = withoutScheme.indexOf('?');
        QString authority = qPos >= 0 ? withoutScheme.left(qPos) : withoutScheme;

        // 拆出端口：最后一个 ':' 之后
        int lastColon = authority.lastIndexOf(':');
        if (lastColon <= 0)
            return false;
        QString portStr = authority.mid(lastColon + 1);
        bool okPort = false;
        int port = portStr.toInt(&okPort);
        if (!okPort || port <= 0 || port > 65535)
            return false;

        // 剩下的是 uuid 和 password@host 这一段
        QString left = authority.left(lastColon);              // uuid:password@example.com  或  uuid:password@ip
        int firstColon = left.indexOf(':');
        if (firstColon <= 0)
            return false;

        QString uuidStr = left.left(firstColon);               // uuid
        QString mid = left.mid(firstColon + 1);                // password@example.com / password@ip / 仅 password

        QString passwordStr;
        QString hostStr;
        int atPos = mid.indexOf('@');
        if (atPos >= 0) {
            passwordStr = mid.left(atPos);
            hostStr = mid.mid(atPos + 1);
        } else {
            // 退化情况：没有 @，尽量从 QUrl 里取 host，密码就整段使用
            passwordStr = mid;
            hostStr = url.host();
        }

        uuid = uuidStr.trimmed();
        password = passwordStr;
        server = hostStr.trimmed();
        server_port = port;

        if (uuid.isEmpty() || password.isEmpty() || server.isEmpty())
            return false;

        // 解析 query，识别通用 TUIC 字段，其余原样保存到 extra_params（如 sni / allow_insecure / pinned_certchain_sha256 等）
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
            auto v = query.queryItemValue("udp_over_stream");
            udp_over_stream = (v == "true" || v == "1");
            query.removeAllQueryItems("udp_over_stream");
        }
        if (query.hasQueryItem("zero_rtt_handshake")) {
            auto v = query.queryItemValue("zero_rtt_handshake");
            zero_rtt_handshake = (v == "true" || v == "1");
            query.removeAllQueryItems("zero_rtt_handshake");
        }
        if (query.hasQueryItem("heartbeat")) {
            heartbeat = query.queryItemValue("heartbeat");
            query.removeAllQueryItems("heartbeat");
        }

        // 其余未识别字段原样保留，避免丢失 sni、allow_insecure、pinned_certchain_sha256 等参数
        extra_params = query.toString(QUrl::FullyEncoded);

        return true;
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

