#pragma once

#include <QColor>
#include <QList>
#include <QString>

#include "audio/types.h"

namespace mixxx {

/// Represents a single annotation on a track with a time range, label, and color
class TrackAnnotation {
  public:
    TrackAnnotation() = default;
    TrackAnnotation(double startTimeSeconds,
            double endTimeSeconds,
            const QString& label,
            const QColor& color)
            : m_startTimeSeconds(startTimeSeconds),
              m_endTimeSeconds(endTimeSeconds),
              m_label(label),
              m_color(color) {
    }

    double getStartTimeSeconds() const {
        return m_startTimeSeconds;
    }
    void setStartTimeSeconds(double time) {
        m_startTimeSeconds = time;
    }

    double getEndTimeSeconds() const {
        return m_endTimeSeconds;
    }
    void setEndTimeSeconds(double time) {
        m_endTimeSeconds = time;
    }

    QString getLabel() const {
        return m_label;
    }
    void setLabel(const QString& label) {
        m_label = label;
    }

    QColor getColor() const {
        return m_color;
    }
    void setColor(const QColor& color) {
        m_color = color;
    }

    bool isValid() const {
        return m_startTimeSeconds >= 0 && m_endTimeSeconds > m_startTimeSeconds;
    }

  private:
    double m_startTimeSeconds{-1.0};
    double m_endTimeSeconds{-1.0};
    QString m_label;
    QColor m_color;
};

/// Container for all annotations on a track
class TrackAnnotationList {
  public:
    TrackAnnotationList() = default;

    void addAnnotation(const TrackAnnotation& annotation) {
        m_annotations.append(annotation);
    }

    const QList<TrackAnnotation>& getAnnotations() const {
        return m_annotations;
    }

    void clear() {
        m_annotations.clear();
    }

    int count() const {
        return m_annotations.count();
    }

    bool isEmpty() const {
        return m_annotations.isEmpty();
    }

    /// Load annotations from a JSON file
    /// Expected format:
    /// {
    ///   "annotations": [
    ///     {
    ///       "start": 10.5,
    ///       "end": 30.2,
    ///       "label": "Intro",
    ///       "color": "#FF0000"
    ///     },
    ///     ...
    ///   ]
    /// }
    static TrackAnnotationList fromJsonFile(const QString& filePath);

    /// Load annotations from JSON data
    static TrackAnnotationList fromJson(const QByteArray& jsonData);

  private:
    QList<TrackAnnotation> m_annotations;
};

} // namespace mixxx
