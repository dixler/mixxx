#include <gtest/gtest.h>

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "track/trackannotation.h"

class TrackAnnotationTest : public testing::Test {
  protected:
    void SetUp() override {
        m_tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(m_tempDir->isValid());
    }

    QString createTestAnnotationFile(const QString& content) {
        QString filePath = m_tempDir->filePath("test.annotations.json");
        QFile file(filePath);
        EXPECT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&file);
        out << content;
        file.close();
        return filePath;
    }

    std::unique_ptr<QTemporaryDir> m_tempDir;
};

TEST_F(TrackAnnotationTest, LoadValidAnnotations) {
    QString jsonContent = R"({
        "annotations": [
            {
                "start": 0.0,
                "end": 30.0,
                "label": "Intro",
                "color": "#FF0000"
            },
            {
                "start": 30.0,
                "end": 60.0,
                "label": "Verse",
                "color": "#00FF00"
            }
        ]
    })";

    QString filePath = createTestAnnotationFile(jsonContent);
    mixxx::TrackAnnotationList annotations = mixxx::TrackAnnotationList::fromJsonFile(filePath);

    EXPECT_EQ(2, annotations.count());

    const auto& annotationList = annotations.getAnnotations();
    ASSERT_EQ(2, annotationList.size());

    // Check first annotation
    const auto& ann1 = annotationList.at(0);
    EXPECT_DOUBLE_EQ(0.0, ann1.getStartTimeSeconds());
    EXPECT_DOUBLE_EQ(30.0, ann1.getEndTimeSeconds());
    EXPECT_EQ("Intro", ann1.getLabel());
    EXPECT_EQ(QColor("#FF0000"), ann1.getColor());

    // Check second annotation
    const auto& ann2 = annotationList.at(1);
    EXPECT_DOUBLE_EQ(30.0, ann2.getStartTimeSeconds());
    EXPECT_DOUBLE_EQ(60.0, ann2.getEndTimeSeconds());
    EXPECT_EQ("Verse", ann2.getLabel());
    EXPECT_EQ(QColor("#00FF00"), ann2.getColor());
}

TEST_F(TrackAnnotationTest, LoadInvalidJSON) {
    QString jsonContent = "{ invalid json }";
    QString filePath = createTestAnnotationFile(jsonContent);
    mixxx::TrackAnnotationList annotations = mixxx::TrackAnnotationList::fromJsonFile(filePath);
    EXPECT_TRUE(annotations.isEmpty());
}

TEST_F(TrackAnnotationTest, LoadMissingFile) {
    QString filePath = m_tempDir->filePath("nonexistent.annotations.json");
    mixxx::TrackAnnotationList annotations = mixxx::TrackAnnotationList::fromJsonFile(filePath);
    EXPECT_TRUE(annotations.isEmpty());
}

TEST_F(TrackAnnotationTest, SkipInvalidAnnotations) {
    QString jsonContent = R"({
        "annotations": [
            {
                "start": 0.0,
                "end": 30.0,
                "label": "Valid",
                "color": "#FF0000"
            },
            {
                "start": -5.0,
                "end": 10.0,
                "label": "Negative Start",
                "color": "#00FF00"
            },
            {
                "start": 50.0,
                "end": 40.0,
                "label": "End Before Start",
                "color": "#0000FF"
            },
            {
                "start": 60.0,
                "end": 90.0,
                "label": "Another Valid",
                "color": "#FFFF00"
            }
        ]
    })";

    QString filePath = createTestAnnotationFile(jsonContent);
    mixxx::TrackAnnotationList annotations = mixxx::TrackAnnotationList::fromJsonFile(filePath);

    // Should only load the 2 valid annotations
    EXPECT_EQ(2, annotations.count());
}

TEST_F(TrackAnnotationTest, HandleInvalidColor) {
    QString jsonContent = R"({
        "annotations": [
            {
                "start": 0.0,
                "end": 30.0,
                "label": "Test",
                "color": "notacolor"
            }
        ]
    })";

    QString filePath = createTestAnnotationFile(jsonContent);
    mixxx::TrackAnnotationList annotations = mixxx::TrackAnnotationList::fromJsonFile(filePath);

    // Should still load with fallback color
    EXPECT_EQ(1, annotations.count());
    const auto& ann = annotations.getAnnotations().at(0);
    EXPECT_TRUE(ann.getColor().isValid());
}

TEST_F(TrackAnnotationTest, AnnotationValidity) {
    mixxx::TrackAnnotation validAnnotation(0.0, 30.0, "Test", QColor("#FF0000"));
    EXPECT_TRUE(validAnnotation.isValid());

    mixxx::TrackAnnotation invalidNegative(-5.0, 30.0, "Test", QColor("#FF0000"));
    EXPECT_FALSE(invalidNegative.isValid());

    mixxx::TrackAnnotation invalidEndBeforeStart(30.0, 10.0, "Test", QColor("#FF0000"));
    EXPECT_FALSE(invalidEndBeforeStart.isValid());

    mixxx::TrackAnnotation invalidEqual(30.0, 30.0, "Test", QColor("#FF0000"));
    EXPECT_FALSE(invalidEqual.isValid());
}
