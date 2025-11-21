#include "waveform/renderers/waveformrenderannotation.h"

#include <QPainter>
#include <QtDebug>

#include "track/track.h"
#include "track/trackannotation.h"
#include "util/painterscope.h"
#include "waveform/renderers/waveformwidgetrenderer.h"

WaveformRenderAnnotation::WaveformRenderAnnotation(
        WaveformWidgetRenderer* waveformWidgetRenderer)
        : WaveformRendererAbstract(waveformWidgetRenderer) {
}

void WaveformRenderAnnotation::setup(const QDomNode& node, const SkinContext& context) {
    Q_UNUSED(node);
    Q_UNUSED(context);
}

void WaveformRenderAnnotation::draw(QPainter* painter, QPaintEvent* /*event*/) {
    TrackPointer pTrack = m_waveformRenderer->getTrackInfo();
    if (!pTrack) {
        return;
    }

    PainterScope PainterScope(painter);
    painter->setWorldMatrixEnabled(false);

    // Get annotations from the track
    mixxx::TrackAnnotationList annotations = pTrack->getAnnotations();

    if (annotations.isEmpty()) {
        return;
    }

    // Get track duration for converting time to sample positions
    double trackDurationSeconds = pTrack->getDuration();
    if (trackDurationSeconds <= 0) {
        return;
    }

    mixxx::audio::SampleRate sampleRate = pTrack->getSampleRate();
    if (!sampleRate.isValid()) {
        return;
    }

    // Calculate total samples in track
    double totalSamples = trackDurationSeconds * sampleRate * mixxx::kEngineChannelCount;

    for (const mixxx::TrackAnnotation& annotation : annotations.getAnnotations()) {
        if (!annotation.isValid()) {
            continue;
        }

        // Convert time in seconds to sample positions
        double startSample = annotation.getStartTimeSeconds() * sampleRate * mixxx::kEngineChannelCount;
        double endSample = annotation.getEndTimeSeconds() * sampleRate * mixxx::kEngineChannelCount;

        // Clamp to track bounds
        startSample = std::max(0.0, std::min(startSample, totalSamples));
        endSample = std::max(0.0, std::min(endSample, totalSamples));

        if (startSample >= endSample) {
            continue;
        }

        // Transform sample positions to screen coordinates
        double startPosition = m_waveformRenderer->transformSamplePositionInRendererWorld(startSample);
        double endPosition = m_waveformRenderer->transformSamplePositionInRendererWorld(endSample);

        // Ensure minimum span for visibility
        const double span = std::max(endPosition - startPosition, 1.0);

        // Check if annotation is visible on screen
        if (startPosition > m_waveformRenderer->getLength() || endPosition < 0) {
            continue;
        }

        // Draw the colored rectangle
        QColor color = annotation.getColor();
        color.setAlphaF(0.3); // Semi-transparent

        QRectF rect;
        if (m_waveformRenderer->getOrientation() == Qt::Horizontal) {
            rect.setRect(startPosition, 0, span, m_waveformRenderer->getHeight());
        } else {
            rect.setRect(0, startPosition, m_waveformRenderer->getWidth(), span);
        }

        painter->fillRect(rect, color);

        // Optionally draw the label (simplified for now - just at the start of the range)
        QString label = annotation.getLabel();
        if (!label.isEmpty() && m_waveformRenderer->getOrientation() == Qt::Horizontal) {
            // Draw label text at the bottom of the annotation
            painter->setPen(Qt::white);
            QFont font = painter->font();
            font.setPixelSize(10);
            painter->setFont(font);

            QRectF textRect(startPosition + 2, m_waveformRenderer->getHeight() - 15, span - 4, 12);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignBottom, label);
        }
    }
}
