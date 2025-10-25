#include "RenderSettingsWidget.h"

static QGroupBox *makeGroup(QWidget *parent, const QString &title, QLayout *innerLayout) {
    auto *box = new QGroupBox(title, parent);
    box->setLayout(innerLayout);
    return box;
}

RenderSettingsWidget::RenderSettingsWidget(QWidget *parent) : QWidget(parent) {
    //
    // --- Rendering group ---
    //
    m_renderModeCombo = new QComboBox(this);
    m_renderModeCombo->addItem("High Performance", static_cast<int>(Renderer::RenderMode::HighPerformance));
    m_renderModeCombo->addItem("High Quality", static_cast<int>(Renderer::RenderMode::HighQuality));

    m_rayBouncesSpin = new QSpinBox(this);
    m_rayBouncesSpin->setRange(1, 64);
    m_rayBouncesSpin->setValue(5);

    m_sppSpin = new QSpinBox(this);
    m_sppSpin->setRange(1, 4096);
    m_sppSpin->setValue(8);

    auto *renderingLayout = new QFormLayout();
    renderingLayout->addRow("Mode", m_renderModeCombo);
    renderingLayout->addRow("Ray bounces", m_rayBouncesSpin);
    renderingLayout->addRow("Samples / pixel", m_sppSpin);

    QGroupBox *renderingGroup = makeGroup(this, "Rendering", renderingLayout);

    //
    // --- Accumulation group ---
    //
    m_accumulateCheck = new QCheckBox("Enable accumulation", this);
    m_accumulateCheck->setChecked(true);

    m_accumFramesSpin = new QSpinBox(this);
    m_accumFramesSpin->setRange(1, 100000);
    m_accumFramesSpin->setValue(300);

    auto *accumLayout = new QFormLayout();
    accumLayout->addRow(m_accumulateCheck);
    accumLayout->addRow("Frames to accumulate", m_accumFramesSpin);

    QGroupBox *accumulationGroup = makeGroup(this, "Accumulation", accumLayout);

    //
    // --- Tone / Color group ---
    //
    m_hdrCheck = new QCheckBox("Enable HDR", this);
    m_hdrCheck->setChecked(true);

    m_exposureSpin = new QDoubleSpinBox(this);
    m_exposureSpin->setRange(-10.0, 10.0);
    m_exposureSpin->setSingleStep(0.1);
    m_exposureSpin->setDecimals(2);
    m_exposureSpin->setValue(0.0);

    m_tonemapCheck = new QCheckBox("Enable Tonemap", this);
    m_tonemapCheck->setChecked(true);

    m_gammaCheck = new QCheckBox("Enable Gamma", this);
    m_gammaCheck->setChecked(true);

    m_gammaSpin = new QDoubleSpinBox(this);
    m_gammaSpin->setRange(0.1, 5.0);
    m_gammaSpin->setSingleStep(0.05);
    m_gammaSpin->setDecimals(2);
    m_gammaSpin->setValue(2.2);

    auto *toneLayout = new QFormLayout();
    toneLayout->addRow(m_hdrCheck);
    toneLayout->addRow("Exposure (EV)", m_exposureSpin);
    toneLayout->addRow(m_tonemapCheck);
    toneLayout->addRow(m_gammaCheck);
    toneLayout->addRow("Gamma", m_gammaSpin);

    QGroupBox *toneGroup = makeGroup(this, "Color / Tone", toneLayout);

    //
    // --- Bloom group ---
    //
    m_bloomCheck = new QCheckBox("Enable Bloom", this);
    m_bloomCheck->setChecked(true);

    m_bloomThresholdSpin = new QDoubleSpinBox(this);
    m_bloomThresholdSpin->setRange(0.0, 100.0);
    m_bloomThresholdSpin->setSingleStep(0.1);
    m_bloomThresholdSpin->setDecimals(3);
    m_bloomThresholdSpin->setValue(1.0);

    m_bloomLevelsSpin = new QSpinBox(this);
    m_bloomLevelsSpin->setRange(1, 10);
    m_bloomLevelsSpin->setValue(4);

    m_bloomRadiusSpin = new QSpinBox(this);
    m_bloomRadiusSpin->setRange(1, 64);
    m_bloomRadiusSpin->setValue(6);

    m_bloomSigmaSpin = new QDoubleSpinBox(this);
    m_bloomSigmaSpin->setRange(0.1, 100.0);
    m_bloomSigmaSpin->setSingleStep(0.1);
    m_bloomSigmaSpin->setDecimals(2);
    m_bloomSigmaSpin->setValue(4.0);

    m_bloomIntensitySpin = new QDoubleSpinBox(this);
    m_bloomIntensitySpin->setRange(0.0, 10.0);
    m_bloomIntensitySpin->setSingleStep(0.05);
    m_bloomIntensitySpin->setDecimals(2);
    m_bloomIntensitySpin->setValue(0.6);

    auto *bloomLayout = new QFormLayout();
    bloomLayout->addRow(m_bloomCheck);
    bloomLayout->addRow("Threshold", m_bloomThresholdSpin);
    bloomLayout->addRow("Levels", m_bloomLevelsSpin);
    bloomLayout->addRow("Radius", m_bloomRadiusSpin);
    bloomLayout->addRow("Sigma", m_bloomSigmaSpin);
    bloomLayout->addRow("Intensity", m_bloomIntensitySpin);

    QGroupBox *bloomGroup = makeGroup(this, "Bloom", bloomLayout);

    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(8);

    mainLayout->addWidget(renderingGroup);
    mainLayout->addWidget(accumulationGroup);
    mainLayout->addWidget(toneGroup);
    mainLayout->addWidget(bloomGroup);
    mainLayout->addStretch(1);

    mainLayout->addStretch(1);
    setLayout(mainLayout);

    //
    // сигналы чтобы дергать settingsChanged()
    //
    auto connectAll = [&](auto *w) {
        if (!w) return;
        // любое изменение -> onAnyChanged()
        QObject::connect(w, SIGNAL(toggled(bool)), this, SLOT(onAnyChanged()));
        QObject::connect(w, SIGNAL(valueChanged(int)), this, SLOT(onAnyChanged()));
        QObject::connect(w, SIGNAL(valueChanged(double)), this, SLOT(onAnyChanged()));
        QObject::connect(w, SIGNAL(currentIndexChanged(int)), this, SLOT(onAnyChanged()));
    };

    connectAll(m_renderModeCombo);
    connectAll(m_rayBouncesSpin);
    connectAll(m_sppSpin);

    connectAll(m_accumulateCheck);
    connectAll(m_accumFramesSpin);

    connectAll(m_hdrCheck);
    connectAll(m_exposureSpin);
    connectAll(m_tonemapCheck);
    connectAll(m_gammaCheck);
    connectAll(m_gammaSpin);

    connectAll(m_bloomCheck);
    connectAll(m_bloomThresholdSpin);
    connectAll(m_bloomLevelsSpin);
    connectAll(m_bloomRadiusSpin);
    connectAll(m_bloomSigmaSpin);
    connectAll(m_bloomIntensitySpin);
}

void RenderSettingsWidget::SetSettings(const Renderer::Settings &s) {
    // Rendering
    m_renderModeCombo->setCurrentIndex(
        (s.RenderMode == Renderer::RenderMode::HighPerformance) ? 0 : 1
    );
    m_rayBouncesSpin->setValue(s.RayBounces);
    m_sppSpin->setValue(s.SamplesPerPixel);

    // Accumulation
    m_accumulateCheck->setChecked(s.Accumulate);
    m_accumFramesSpin->setValue(s.FramesToAccumulate);

    // Tone / Color
    m_hdrCheck->setChecked(s.HDREnabled);
    m_exposureSpin->setValue(s.Exposure);
    m_tonemapCheck->setChecked(s.TonemapEnabled);
    m_gammaCheck->setChecked(s.GammaCorrectionEnabled);
    m_gammaSpin->setValue(s.Gamma);

    // Bloom
    m_bloomCheck->setChecked(s.BloomEnabled);
    m_bloomThresholdSpin->setValue(s.BloomThreshold);
    m_bloomLevelsSpin->setValue(s.BloomLevels);
    m_bloomRadiusSpin->setValue(s.BloomRadius);
    m_bloomSigmaSpin->setValue(s.BloomSigma);
    m_bloomIntensitySpin->setValue(s.BloomIntensity);
}

Renderer::Settings RenderSettingsWidget::GetSettings() const {
    Renderer::Settings s;

    // Rendering
    s.RenderMode = (m_renderModeCombo->currentIndex() == 0)
                       ? Renderer::RenderMode::HighPerformance
                       : Renderer::RenderMode::HighQuality;
    s.RayBounces = m_rayBouncesSpin->value();
    s.SamplesPerPixel = m_sppSpin->value();

    // Accumulation
    s.Accumulate = m_accumulateCheck->isChecked();
    s.FramesToAccumulate = m_accumFramesSpin->value();

    // Tone / Color
    s.HDREnabled = m_hdrCheck->isChecked();
    s.Exposure = static_cast<float>(m_exposureSpin->value());
    s.TonemapEnabled = m_tonemapCheck->isChecked();
    s.GammaCorrectionEnabled = m_gammaCheck->isChecked();
    s.Gamma = static_cast<float>(m_gammaSpin->value());

    // Bloom
    s.BloomEnabled = m_bloomCheck->isChecked();
    s.BloomThreshold = static_cast<float>(m_bloomThresholdSpin->value());
    s.BloomLevels = m_bloomLevelsSpin->value();
    s.BloomRadius = m_bloomRadiusSpin->value();
    s.BloomSigma = static_cast<float>(m_bloomSigmaSpin->value());
    s.BloomIntensity = static_cast<float>(m_bloomIntensitySpin->value());

    return s;
}

void RenderSettingsWidget::onAnyChanged() {
    emit settingsChanged(GetSettings());
}
