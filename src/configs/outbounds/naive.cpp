#include "include/configs/outbounds/naive.h"

#include <QUrlQuery>
#include <include/global/Utils.hpp>

#include "include/configs/common/utils.h"
#include "include/global/DataStore.hpp"

namespace Configs {
    bool naive::ParseFromLink(const QString& link)
    {
        auto url = QUrl(link);
        if (!url.isValid()) return false;

        // Parse naive+https:// or naive+quic:// format
        protocol = url.scheme().replace("naive+", "");
        if (protocol != "https" && protocol != "quic") return false;

        outbound::ParseFromLink(link);
        username = url.userName();
        password = url.password();

        return !(username.isEmpty() || password.isEmpty() || server.isEmpty());
    }

    bool naive::ParseFromJson(const QJsonObject& object)
    {
        if (object.isEmpty() || object["type"].toString() != "naive") return false;
        outbound::ParseFromJson(object);
        if (object.contains("username")) username = object["username"].toString();
        if (object.contains("password")) password = object["password"].toString();
        if (object.contains("protocol")) protocol = object["protocol"].toString();
        if (object.contains("extra_headers")) extra_headers = object["extra_headers"].toString();
        if (object.contains("sni")) sni = object["sni"].toString();
        if (object.contains("certificate")) certificate = object["certificate"].toString();
        if (object.contains("insecure_concurrency")) insecure_concurrency = object["insecure_concurrency"].toInt();
        if (object.contains("disable_log")) disable_log = object["disable_log"].toBool();
        return true;
    }

    QString naive::ExportToLink()
    {
        QUrl url;
        url.setScheme("naive+" + protocol);
        url.setUserName(username);
        url.setPassword(password);
        url.setHost(server);
        url.setPort(server_port);
        if (!name.isEmpty()) url.setFragment(name);
        return url.toString(QUrl::FullyEncoded);
    }

    QJsonObject naive::ExportToJson()
    {
        QJsonObject object;
        object["type"] = "naive";
        mergeJsonObjects(object, outbound::ExportToJson());
        if (!username.isEmpty()) object["username"] = username;
        if (!password.isEmpty()) object["password"] = password;
        if (protocol != "https") object["protocol"] = protocol;
        if (!extra_headers.isEmpty()) object["extra_headers"] = extra_headers;
        if (!sni.isEmpty()) object["sni"] = sni;
        if (!certificate.isEmpty()) object["certificate"] = certificate;
        if (insecure_concurrency > 0) object["insecure_concurrency"] = insecure_concurrency;
        if (disable_log) object["disable_log"] = disable_log;
        return object;
    }

    BuildResult naive::Build()
    {
        // External naive.exe provides local socks, sing-box only needs socks outbound.
        auto listenAddr = Configs::dataStore->naive_socks_listen_addr.trimmed();
        if (listenAddr.isEmpty()) listenAddr = "127.0.0.1";
        int base = Configs::dataStore->naive_socks_port_base;
        if (base <= 0) base = 30000;
        // Use stable hash to allocate a deterministic local port per node.
        // (Avoid relying on profile id which isn't available in outbound.)
        uint h = qHash(server + ":" + Int2String(server_port) + ":" + username);
        int listenPort = base + (int)(h % 10000);
        if (listenPort <= 0 || listenPort > 65535) listenPort = 30000;

        QJsonObject object;
        object["type"] = "socks";
        object["server"] = listenAddr;
        object["server_port"] = listenPort;
        // Do not add auth here; naive local socks usually doesn't require it.
        mergeJsonObjects(object, outbound::Build().object);
        return {object, ""};
    }

    QString naive::DisplayType()
    {
        return "Naive";
    }

    // Naive 在 sing-box 中是普通 outbound 类型，而不是 endpoint。
    // 这里返回 false，确保生成的配置放到 \"outbounds\" 数组里，
    // 避免出现 \"unknown endpoint type: naive\" 的错误。
    bool naive::IsEndpoint()
    {
        return false;
    }
}




