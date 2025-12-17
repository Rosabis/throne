#include "include/configs/outbounds/naive.h"

#include <QUrlQuery>
#include <include/global/Utils.hpp>

#include "include/configs/common/utils.h"

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
        // Naive proxy typically runs as an external process
        // Return configuration that will be used by external naive-proxy binary
        QJsonObject object;
        object["type"] = "naive";
        mergeJsonObjects(object, outbound::Build().object);
        if (!username.isEmpty()) object["username"] = username;
        if (!password.isEmpty()) object["password"] = password;
        if (!protocol.isEmpty()) object["protocol"] = protocol;
        if (!extra_headers.isEmpty()) object["extra_headers"] = extra_headers;
        if (!sni.isEmpty()) object["sni"] = sni;
        if (!certificate.isEmpty()) object["certificate"] = certificate;
        if (insecure_concurrency > 0) object["insecure_concurrency"] = insecure_concurrency;
        if (disable_log) object["disable_log"] = disable_log;
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




