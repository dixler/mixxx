#include "waveform/renderers/waveformrenderbeat.h"

#include <QPainter>
#include <QVector>
#include <array>

#include "track/track.h"
#include "util/painterscope.h"
#include "waveform/renderers/waveformwidgetrenderer.h"
#include "widget/wskincolor.h"

class QPaintEvent;

WaveformRenderBeat::WaveformRenderBeat(WaveformWidgetRenderer* waveformWidgetRenderer)
        : WaveformRendererAbstract(waveformWidgetRenderer) {
}

WaveformRenderBeat::~WaveformRenderBeat() {
}

void WaveformRenderBeat::setup(const QDomNode& node, const SkinContext& context) {
    m_beatColor = QColor(context.selectString(node, "BeatColor"));
    m_beatColor = WSkinColor::getCorrectColor(m_beatColor).toRgb();
}

void WaveformRenderBeat::draw(QPainter* painter, QPaintEvent* /*event*/) {
    TrackPointer pTrackInfo = m_waveformRenderer->getTrackInfo();

    if (!pTrackInfo) {
        return;
    }

    mixxx::BeatsPointer trackBeats = pTrackInfo->getBeats();
    if (!trackBeats) {
        return;
    }

    const int alpha = m_waveformRenderer->getBeatGridAlpha();
    if (alpha == 0) {
        return;
    }
#ifdef MIXXX_USE_QOPENGL
    // Using alpha transparency with drawLines causes a graphical issue when
    // drawing with QPainter on the QOpenGLWindow: instead of individual lines
    // a large rectangle encompassing all beatlines is drawn.
    const float beatAlpha = 1.f;
#else
    const float beatAlpha = m_beatColor.alphaF() * (alpha / 100.0f);
#endif

    if (beatAlpha <= 0.0f) {
        return;
    }

    const std::array<QColor, 4> beatColors = {Qt::red, Qt::yellow, Qt::green, Qt::blue};
    std::array<QVector<QLineF>, 4> coloredBeats;

    const double trackSamples = m_waveformRenderer->getTrackSamples();
    if (trackSamples <= 0) {
        return;
    }

    const float devicePixelRatio = m_waveformRenderer->getDevicePixelRatio();

    const double firstDisplayedPosition =
            m_waveformRenderer->getFirstDisplayedPosition();
    const double lastDisplayedPosition =
            m_waveformRenderer->getLastDisplayedPosition();

    // qDebug() << "trackSamples" << trackSamples
    //          << "firstDisplayedPosition" << firstDisplayedPosition
    //          << "lastDisplayedPosition" << lastDisplayedPosition;

    const auto startPosition = mixxx::audio::FramePos::fromEngineSamplePos(
            firstDisplayedPosition * trackSamples);
    const auto endPosition = mixxx::audio::FramePos::fromEngineSamplePos(
            lastDisplayedPosition * trackSamples);
    auto it = trackBeats->iteratorFrom(startPosition);

    // if no beat do not waste time saving/restoring painter
    if (it == trackBeats->cend() || *it > endPosition) {
        return;
    }

    PainterScope PainterScope(painter);

    painter->setRenderHint(QPainter::Antialiasing);

    const Qt::Orientation orientation = m_waveformRenderer->getOrientation();
    const float rendererWidth = m_waveformRenderer->getWidth();
    const float rendererHeight = m_waveformRenderer->getHeight();

    for (; it != trackBeats->cend() && *it <= endPosition; ++it) {
        double beatPosition = it->toEngineSamplePos();
        double xBeatPoint =
                m_waveformRenderer->transformSamplePositionInRendererWorld(beatPosition);

        xBeatPoint = qRound(xBeatPoint * devicePixelRatio) / devicePixelRatio;

        const int colorIndex = static_cast<int>(it - trackBeats->cbegin()) & 0x3;
        auto& beatLines = coloredBeats[colorIndex];

        auto line = orientation == Qt::Horizontal
                ? QLineF{xBeatPoint, 0.0f, xBeatPoint, rendererHeight}
                : QLineF{0.0f, xBeatPoint, rendererWidth, xBeatPoint};
        beatLines.append(line);
    }

    QPen beatPen;
    beatPen.setWidthF(std::max(1.0, scaleFactor()));
    for (int i = 0; i < coloredBeats.size(); ++i) {
        if (coloredBeats[i].isEmpty()) {
            continue;
        }
        QColor color = beatColors[i];
        color.setAlphaF(beatAlpha);
        beatPen.setColor(color);
        painter->setPen(beatPen);
        painter->drawLines(coloredBeats[i].constData(), coloredBeats[i].size());
    }
}
