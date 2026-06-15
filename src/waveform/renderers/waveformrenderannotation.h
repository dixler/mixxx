#pragma once

#include "waveform/renderers/waveformrendererabstract.h"

class QPaintEvent;
class QPainter;
class WaveformWidgetRenderer;

/// Renderer for track annotations (colored ranges with labels underneath the waveform)
class WaveformRenderAnnotation : public WaveformRendererAbstract {
  public:
    explicit WaveformRenderAnnotation(WaveformWidgetRenderer* waveformWidgetRenderer);
    ~WaveformRenderAnnotation() override = default;

    void setup(const QDomNode& node, const SkinContext& context) override;
    void draw(QPainter* painter, QPaintEvent* event) override;
};
