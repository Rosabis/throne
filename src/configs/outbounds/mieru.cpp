#include "include/configs/outbounds/mieru.h"

#include <QUrlQuery>
#include <QJsonArray>
#include <include/global/Utils.hpp>

#include "include/configs/common/utils.h"

namespace Configs {
    bool mieru::ParseFromLink(const QString& link)
    {
        auto url = QUrl(link);
        if (!url.isValid()) return false;

        // Parse mierus:// format (simple sharing link)
        if (url.scheme() != "mierus") return false;

        outbound::ParseFromLink(link);
        username = url.userName();
        password = url.password();

        QUrlQuery query(url.query(QUrl::ComponentFormattingOption::FullyDecoded));
        
        if (query.hasQueryItem("profile")) {
            profile_name = query.queryItemValue("profile");
        }
        if (query.hasQueryItem("mtu")) {
            mtu = query.queryItemValue("mtu").toInt();
            if (mtu < 1280 || mtu > 1400) mtu = 1400;
        }
        if (query.hasQueryItem("multiplexing")) {
            multiplexing = query.queryItemValue("multiplexing");
        }
        if (query.hasQueryItem("handshake-mode")) {
            handshake_mode = query.queryItemValue("handshake-mode");
        }
        
        // Parse port and protocol (can appear multiple times)
        QStringList portValues = query.allQueryItemValues("port");
        QStringList protocolValues = query.allQueryItemValues("protocol");
        
        // port and protocol must appear the same number of times
        if (portValues.size() == protocolValues.size() && !portValues.isEmpty()) {
            port_bindings = portValues;
            protocols = protocolValues;
        } else if (!portValues.isEmpty()) {
            // If only ports are specified, default to TCP
            port_bindings = portValues;
            protocols.clear();
            for (int i = 0; i < portValues.size(); i++) {
                protocols << "TCP";
            }
        }

        if (server_port == 0 && !port_bindings.isEmpty()) {
            // Use first port as server_port for display
            QString firstPort = port_bindings.first();
            if (firstPort.contains("-")) {
                // Port range, use start port
                server_port = firstPort.split("-").first().toInt();
            } else {
                server_port = firstPort.toInt();
            }
        }

        return !(username.isEmpty() || password.isEmpty() || server.isEmpty());
    }

    bool mieru::ParseFromJson(const QJsonObject& object)
    {
        if (object.isEmpty() || object["type"].toString() != "mieru") return false;
        outbound::ParseFromJson(object);
        if (object.contains("username")) username = object["username"].toString();
        if (object.contains("password")) password = object["password"].toString();
        if (object.contains("profile_name")) profile_name = object["profile_name"].toString();
        if (object.contains("mtu")) mtu = object["mtu"].toInt();
        if (object.contains("multiplexing")) multiplexing = object["multiplexing"].toString();
        if (object.contains("handshake_mode")) handshake_mode = object["handshake_mode"].toString();
        if (object.contains("port_bindings")) port_bindings = QJsonArray2QListString(object["port_bindings"].toArray());
        if (object.contains("protocols")) protocols = QJsonArray2QListString(object["protocols"].toArray());
        return true;
    }

    QString mieru::ExportToLink()
    {
        QUrl url;
        QUrlQuery query;
        url.setScheme("mierus");
        url.setUserName(username);
        url.setPassword(password);
        url.setHost(server);
        if (server_port > 0) url.setPort(server_port);
        if (!name.isEmpty()) url.setFragment(name);

        query.addQueryItem("profile", profile_name);
        if (mtu != 1400) query.addQueryItem("mtu", Int2String(mtu));
        if (multiplexing != "MULTIPLEXING_LOW") query.addQueryItem("multiplexing", multiplexing);
        if (handshake_mode != "HANDSHAKE_STANDARD") query.addQueryItem("handshake-mode", handshake_mode);
        
        // Add port and protocol pairs
        for (int i = 0; i < port_bindings.size() && i < protocols.size(); i++) {
            query.addQueryItem("port", port_bindings[i]);
            query.addQueryItem("protocol", protocols[i]);
        }
        
        url.setQuery(query);
        return url.toString();
    }

    QJsonObject mieru::ExportToJson()
    {
        QJsonObject object;
        object["type"] = "mieru";
        mergeJsonObjects(object, outbound::ExportToJson());
        if (!username.isEmpty()) object["username"] = username;
        if (!password.isEmpty()) object["password"] = password;
        if (profile_name != "default") object["profile_name"] = profile_name;
        if (mtu != 1400) object["mtu"] = mtu;
        if (multiplexing != "MULTIPLEXING_LOW") object["multiplexing"] = multiplexing;
        if (handshake_mode != "HANDSHAKE_STANDARD") object["handshake_mode"] = handshake_mode;
        if (!port_bindings.isEmpty()) object["port_bindings"] = QListStr2QJsonArray(port_bindings);
        if (!protocols.isEmpty()) object["protocols"] = QListStr2QJsonArray(protocols);
        return object;
    }

    BuildResult mieru::Build()
    {
        // External mieru.exe provides local socks, sing-box only needs socks outbound.
        auto listenAddr = Configs::dataStore->mieru_socks_listen_addr.trimmed();
        if (listenAddr.isEmpty()) listenAddr = "127.0.0.1";
        int base = Configs::dataStore->mieru_socks_port_base;
        if (base <= 0) base = 32000;
        // Use stable hash to allocate a deterministic local port per node.
        uint h = qHash(server + ":" + username + ":" + password);
        int listenPort = base + (int)(h % 10000);
        if (listenPort <= 0 || listenPort > 65535) listenPort = 32000;

        // Build base outbound first, then override with local socks settings
        auto baseResult = outbound::Build();
        QJsonObject object = baseResult.object;
        object["type"] = "socks";
        // Override with local mieru.exe socks address/port (not remote server)
        object["server"] = listenAddr;
        object["server_port"] = listenPort;
        object.remove("name");
        return {object, ""};
    }

    QString mieru::DisplayType()
    {
        return "Mieru";
    }

    // Mieru 在 sing-box 中是普通 outbound 类型，而不是 endpoint。
    bool mieru::IsEndpoint()
    {
        return false;
    }
}

