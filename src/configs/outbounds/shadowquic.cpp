#include "include/configs/outbounds/shadowquic.h"

#include <QUrl>
#include <QUrlQuery>
#include <QJsonArray>

#include "include/global/Utils.hpp"
#include "include/configs/common/utils.h"

namespace Configs {

    // 暂时没有统一的 shadowquic 分享链接格式，这里先不实现从链接解析 / 导出链接。
    bool shadowquic::ParseFromLink(const QString& link)
    {
        Q_UNUSED(link);
        return false;
    }

    bool shadowquic::ParseFromJson(const QJsonObject& object)
    {
        if (object.isEmpty() || object["type"].toString() != "shadowquic") return false;
        outbound::ParseFromJson(object);
        if (object.contains("username")) username = object["username"].toString();
        if (object.contains("password")) password = object["password"].toString();
        if (object.contains("server_name")) server_name = object["server_name"].toString();
        if (object.contains("alpn")) alpn = QJsonArray2QListString(object["alpn"].toArray());
        if (object.contains("initial_mtu")) initial_mtu = object["initial_mtu"].toInt();
        if (object.contains("congestion_control")) congestion_control = object["congestion_control"].toString();
        if (object.contains("zero_rtt")) zero_rtt = object["zero_rtt"].toBool();
        if (object.contains("over_stream")) over_stream = object["over_stream"].toBool();
        return true;
    }

    QString shadowquic::ExportToLink()
    {
        // 暂不支持链接导入导出
        return {};
    }

    QJsonObject shadowquic::ExportToJson()
    {
        QJsonObject object;
        object["type"] = "shadowquic";
        mergeJsonObjects(object, outbound::ExportToJson());
        if (!username.isEmpty()) object["username"] = username;
        if (!password.isEmpty()) object["password"] = password;
        if (!server_name.isEmpty()) object["server_name"] = server_name;
        if (!alpn.isEmpty()) object["alpn"] = QListStr2QJsonArray(alpn);
        if (initial_mtu != 1300) object["initial_mtu"] = initial_mtu;
        if (!congestion_control.isEmpty()) object["congestion_control"] = congestion_control;
        if (zero_rtt) object["zero_rtt"] = zero_rtt;
        if (over_stream) object["over_stream"] = over_stream;
        return object;
    }

    BuildResult shadowquic::Build()
    {
        // 外部 shadowquic 提供本地 socks，sing-box 只需要 socks outbound。
        auto listenAddr = Configs::dataStore->shadowquic_socks_listen_addr.trimmed();
        if (listenAddr.isEmpty()) listenAddr = "127.0.0.1";
        int base = Configs::dataStore->shadowquic_socks_port_base;
        if (base <= 0) base = 33000;
        uint h = qHash(server + ":" + Int2String(server_port) + ":" + username);
        int listenPort = base + (int)(h % 10000);
        if (listenPort <= 0 || listenPort > 65535) listenPort = 33000;

        auto baseResult = outbound::Build();
        QJsonObject object = baseResult.object;
        object["type"] = "socks";
        object["server"] = listenAddr;
        object["server_port"] = listenPort;
        object.remove("name");
        return {object, ""};
    }

    QString shadowquic::DisplayType()
    {
        return "ShadowQUIC";
    }

    bool shadowquic::IsEndpoint()
    {
        return false;
    }
}


