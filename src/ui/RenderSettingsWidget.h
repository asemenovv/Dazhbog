#pragma once

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "render/Renderer.h"

class RenderSettingsWidget final : public QWidget {
    Q_OBJECT
public:
    explicit RenderSettingsWidget(QWidget *parent = nullptr);

    void SetSettings(const Renderer::Settings& s);
    [[nodiscard]] Renderer::Settings GetSettings() const;

signals:
    void settingsChanged(const Renderer::Settings& newSettings);

private slots:
    void onAnyChanged();


private:
    // === Rendering ===
    QComboBox*      m_renderModeCombo;
    QSpinBox*       m_rayBouncesSpin;
    QSpinBox*       m_sppSpin;

    // === Accumulation ===
    QCheckBox*      m_accumulateCheck;
    QSpinBox*       m_accumFramesSpin;

    // === Tone / Color ===
    QCheckBox*      m_hdrCheck;
    QDoubleSpinBox* m_exposureSpin;

    QCheckBox*      m_tonemapCheck;

    QCheckBox*      m_gammaCheck;
    QDoubleSpinBox* m_gammaSpin;

    // === Bloom ===
    QCheckBox*      m_bloomCheck;
    QDoubleSpinBox* m_bloomThresholdSpin;
    QSpinBox*       m_bloomLevelsSpin;
    QSpinBox*       m_bloomRadiusSpin;
    QDoubleSpinBox* m_bloomSigmaSpin;
    QDoubleSpinBox* m_bloomIntensitySpin;
};
