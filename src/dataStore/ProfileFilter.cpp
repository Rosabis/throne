#include "include/dataStore/ProfileFilter.hpp"
#include "include/global/Utils.hpp"
#include <QUrl>

namespace Configs {

    static QJsonObject normalizeJsonObject(const QJsonObject &obj) {
        QJsonObject normalized;
        QStringList keys = obj.keys();
        keys.sort();
        for (const QString &key : keys) {
            auto value = obj[key];
            // Recursively normalize nested objects
            if (value.isObject()) {
                normalized[key] = normalizeJsonObject(value.toObject());
            } else if (value.isArray()) {
                // For arrays, we keep them as-is for now
                normalized[key] = value;
            } else if (!value.isNull() && !(value.isString() && value.toString().isEmpty())) {
                // Skip null and empty strings, but keep other values
                normalized[key] = value;
            }
        }
        return normalized;
    }

    QString ProfileFilter_ent_key(const std::shared_ptr<Configs::ProxyEntity> &ent) {
        // Export JSON without name field for duplicate detection
        auto json = ent->outbound->ExportToJson();
        // Remove name/tag field to ignore it in duplicate detection
        json.remove("tag");
        json.remove("name");
        
        // Normalize JSON: sort keys and normalize nested objects
        QJsonObject normalized = normalizeJsonObject(json);
        
        QUrl url;
        url.setScheme("json");
        url.setHost("throne");
        url.setFragment(QJsonObject2QString(normalized, true)
                            .toUtf8()
                            .toBase64(QByteArray::Base64UrlEncoding));
        return url.toString();
    }

    void ProfileFilter::Uniq(const QList<std::shared_ptr<ProxyEntity>> &in,
                             QList<std::shared_ptr<ProxyEntity>> &out,
                             bool keep_last) {
        QMap<QString, std::shared_ptr<ProxyEntity>> hashMap;

        for (const auto &ent: in) {
            QString key = ProfileFilter_ent_key(ent);
            if (hashMap.contains(key)) {
                if (keep_last) {
                    out.removeAll(hashMap[key]);
                    hashMap[key] = ent;
                    out += ent;
                }
            } else {
                hashMap[key] = ent;
                out += ent;
            }
        }
    }

    void ProfileFilter::Common(const QList<std::shared_ptr<ProxyEntity>> &src,
                               const QList<std::shared_ptr<ProxyEntity>> &dst,
                               QList<std::shared_ptr<ProxyEntity>> &outSrc,
                               QList<std::shared_ptr<ProxyEntity>> &outDst) {
        QMap<QString, std::shared_ptr<ProxyEntity>> hashMap;

        for (const auto &ent: src) {
            QString key = ProfileFilter_ent_key(ent);
            hashMap[key] = ent;
        }
        for (const auto &ent: dst) {
            QString key = ProfileFilter_ent_key(ent);
            if (hashMap.contains(key)) {
                outDst += ent;
                outSrc += hashMap[key];
            }
        }
    }

    void ProfileFilter::OnlyInSrc(const QList<std::shared_ptr<ProxyEntity>> &src,
                                  const QList<std::shared_ptr<ProxyEntity>> &dst,
                                  QList<std::shared_ptr<ProxyEntity>> &out) {
        QMap<QString, bool> hashMap;

        for (const auto &ent: dst) {
            QString key = ProfileFilter_ent_key(ent);
            hashMap[key] = true;
        }
        for (const auto &ent: src) {
            QString key = ProfileFilter_ent_key(ent);
            if (!hashMap.contains(key)) out += ent;
        }
    }

    void ProfileFilter::OnlyInSrc_ByPointer(const QList<std::shared_ptr<ProxyEntity>> &src,
                                            const QList<std::shared_ptr<ProxyEntity>> &dst,
                                            QList<std::shared_ptr<ProxyEntity>> &out) {
        for (const auto &ent: src) {
            if (!dst.contains(ent)) out += ent;
        }
    }

} // namespace Configs