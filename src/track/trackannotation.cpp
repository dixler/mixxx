#include "track/trackannotation.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtDebug>

namespace mixxx {

TrackAnnotationList TrackAnnotationList::fromJsonFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open annotation file:" << filePath;
        return TrackAnnotationList();
    }

    QByteArray data = file.readAll();
    file.close();

    return fromJson(data);
}

TrackAnnotationList TrackAnnotationList::fromJson(const QByteArray& jsonData) {
    TrackAnnotationList result;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse annotation JSON:" << parseError.errorString();
        return result;
    }

    if (!doc.isObject()) {
        qDebug() << "Annotation JSON is not an object";
        return result;
    }

    QJsonObject root = doc.object();
    QJsonValue annotationsValue = root.value("annotations");

    if (!annotationsValue.isArray()) {
        qDebug() << "Annotation JSON does not contain 'annotations' array";
        return result;
    }

    QJsonArray annotationsArray = annotationsValue.toArray();

    for (const QJsonValue& value : annotationsArray) {
        if (!value.isObject()) {
            qDebug() << "Skipping non-object annotation entry";
            continue;
        }

        QJsonObject obj = value.toObject();

        double start = obj.value("start").toDouble(-1.0);
        double end = obj.value("end").toDouble(-1.0);
        QString label = obj.value("label").toString();
        QString colorStr = obj.value("color").toString("#808080");

        QColor color(colorStr);
        if (!color.isValid()) {
            qDebug() << "Invalid color in annotation:" << colorStr << "- using gray";
            color = QColor("#808080");
        }

        TrackAnnotation annotation(start, end, label, color);

        if (annotation.isValid()) {
            result.addAnnotation(annotation);
        } else {
            qDebug() << "Skipping invalid annotation: start=" << start
                     << "end=" << end << "label=" << label;
        }
    }

    qDebug() << "Loaded" << result.count() << "annotations";

    return result;
}

} // namespace mixxx
