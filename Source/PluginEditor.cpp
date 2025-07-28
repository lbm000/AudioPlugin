#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//==============================================================================
SampleAudioProcessorEditor::SampleAudioProcessorEditor (SampleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&customLookAndFeel);

    // << NOVO: Helper para configurar os knobs >>
    auto configureAsKnob = [](juce::Slider& slider, const juce::String& suffix = "")
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
        slider.setNumDecimalPlacesToDisplay(2);
        if (!suffix.isEmpty())
            slider.setTextValueSuffix(" " + suffix);
    };

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        // --- LOAD / PLAY ---
        loadSampleButtons[i].setButtonText("Load " + juce::String(i + 1));
        loadSampleButtons[i].onClick = [this, i]()
        {
            fileChooser = std::make_unique<juce::FileChooser>("Select a Sample", juce::File{}, "*.wav");
            fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this, i](const juce::FileChooser& chooser)
                {
                    auto file = chooser.getResult();
                    if (file.existsAsFile())
                        audioProcessor.loadSampleFile(file, i);
                });
        };
        addAndMakeVisible(loadSampleButtons[i]);

        playSampleButtons[i].setClickingTogglesState(true);
        playSampleButtons[i].setButtonText("▶");
        playSampleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        addAndMakeVisible(playSampleButtons[i]);

        playSampleButtons[i].onClick = [this, i]()
        {
            bool isPlaying = playSampleButtons[i].getToggleState();

            // Atualiza o botão visualmente
            playSampleButtons[i].setButtonText(isPlaying ? "■" : "▶");
            playSampleButtons[i].setColour(juce::TextButton::buttonColourId,
                                           isPlaying ? juce::Colours::green : juce::Colours::darkgrey);

            // Atualiza a lógica do sample correspondente
            audioProcessor.isSamplePlaying[i] = isPlaying;
        };

        // --- LPF ---
        setupToggleButton(filterToggleButtons[i], "LPF");
        configureAsKnob(filterCutoffSliders[i], "Hz");
        filterCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(filterCutoffSliders[i]);
        filterCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setFilterCutoff(i, filterCutoffSliders[i].getValue());
        };

        // --- HPF ---
        setupToggleButton(highpassToggleButtons[i], "HPF");
        configureAsKnob(highpassCutoffSliders[i], "Hz");
        highpassCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(highpassCutoffSliders[i]);
        highpassCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setHighpassCutoff(i, highpassCutoffSliders[i].getValue());
        };

        // --- BPF ---
        setupToggleButton(bandpassToggleButtons[i], "BPF");
        configureAsKnob(bandpassCutoffSliders[i], "Hz");
        configureAsKnob(bandpassBandwidthSliders[i], "Hz");
        addAndMakeVisible(bandpassCutoffSliders[i]);
        addAndMakeVisible(bandpassBandwidthSliders[i]);
        bandpassCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBandPassCutoff(i, bandpassCutoffSliders[i].getValue());
        };

        bandpassBandwidthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBandPassBandwidth(i, bandpassBandwidthSliders[i].getValue());
        };


        // --- Notch ---
        setupToggleButton(notchToggleButtons[i], "Notch");
        configureAsKnob(notchCutoffSliders[i], "Hz");
        configureAsKnob(notchBandwidthSliders[i], "Hz");
        addAndMakeVisible(notchCutoffSliders[i]);
        addAndMakeVisible(notchBandwidthSliders[i]);
        notchCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchCutoff(i, notchCutoffSliders[i].getValue());
        };

        notchBandwidthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchBandwidth(i, notchBandwidthSliders[i].getValue());
        };

        // --- Peak ---
        setupToggleButton(peakToggleButtons[i], "Peak");
        configureAsKnob(peakCutoffSliders[i], "Hz");
        configureAsKnob(peakGainSliders[i], "dB");
        configureAsKnob(peakQSliders[i], "Q");
        addAndMakeVisible(peakCutoffSliders[i]);
        addAndMakeVisible(peakGainSliders[i]);
        addAndMakeVisible(peakQSliders[i]);
        peakCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakCutoff(i, peakCutoffSliders[i].getValue());
        };

        peakGainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakGain(i, peakGainSliders[i].getValue());
        };

        peakQSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakQ(i, peakQSliders[i].getValue());
        };

        // === ONCLICK + LÓGICA MUTUAMENTE EXCLUSIVA ===

        filterToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = filterToggleButtons[i].getToggleState();
            audioProcessor.setFilterEnabled(i, enabled);
        };

        highpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = highpassToggleButtons[i].getToggleState();
            audioProcessor.setHighpassEnabled(i, enabled);
        };

        bandpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = bandpassToggleButtons[i].getToggleState();
            audioProcessor.setBandPassEnabled(i, enabled);
            handleFilterToggleLogic(i, bandpassToggleButtons[i]);
        };

        notchToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = notchToggleButtons[i].getToggleState();
            audioProcessor.setNotchEnabled(i, enabled);
            handleFilterToggleLogic(i, notchToggleButtons[i]);
        };

        peakToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = peakToggleButtons[i].getToggleState();
            audioProcessor.setPeakEnabled(i, enabled);
            handleFilterToggleLogic(i, peakToggleButtons[i]);
        };
    }


    // << MUDANÇA: Lógica dos Step Buttons >>
    for (int track = 0; track < NUM_TRACKS; ++track)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            auto& button = stepButtons[track][step];
            button.setClickingTogglesState(true);
            button.onClick = [this, track, step]
            {
                bool isOn = stepButtons[track][step].getToggleState();
                audioProcessor.setStepState(track, step, isOn);
                // Não precisa mudar a cor aqui, o paintButton já faz isso!
            };
            addAndMakeVisible(button);
        }
    }

    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setText(juce::String(step + 1), juce::dontSendNotification);
        stepLabels[step].setJustificationType(juce::Justification::centred);
        stepLabels[step].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(stepLabels[step]);
    }

    stepSequencerGroup.setText("Step Sequencer");
    stepSequencerGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    stepSequencerGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(stepSequencerGroup);

    globalBpmLabel.setText("Global BPM", juce::dontSendNotification);
    globalBpmLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(globalBpmLabel);

    // << MUDANÇA: Configurar slider de BPM como knob >>
    configureAsKnob(globalBpmSlider);
    globalBpmSlider.setRange(1.0, 300.0, 1.0);
    globalBpmSlider.setValue(audioProcessor.getGlobalBpm());
    globalBpmSlider.onValueChange = [this]() {
        audioProcessor.setGlobalBpm(globalBpmSlider.getValue());
    };
    addAndMakeVisible(globalBpmSlider);

    startTimerHz(10);

    // << MUDANÇA: Novo tamanho inicial da janela >>
    setSize(1370, 700);
}


SampleAudioProcessorEditor::~SampleAudioProcessorEditor()
{
    setLookAndFeel(nullptr); // << ADICIONAR ESTA LINHA
}

//==============================================================================
void SampleAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Preenche o fundo da janela
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    // Ou uma cor fixa: g.fillAll(juce::Colour(0xff282c34));

    const int stepSize = 25; // Largura de cada step
    const int rowHeight = 35; // Altura de cada linha de track

    // Calcula a posição X da barra amarela com base no step atual e na posição inicial do sequencer
    const int x = stepStartX + audioProcessor.getCurrentStep() * stepSize;
    // A posição Y da barra começa na mesma altura dos botões do sequencer
    const int y = stepYStart;

    // Desenha a barra amarela
    g.setColour(juce::Colours::yellow.withAlpha(0.3f)); // Cor e transparência da barra
    g.fillRect(x, y, stepSize, NUM_TRACKS * rowHeight); // Retângulo da barra
}



void SampleAudioProcessorEditor::resized()
{
    // Reduz as bordas da janela principal em 10 pixels de cada lado
    auto bounds = getLocalBounds().reduced(10);

    // --- 1. Defina as áreas principais: Controles (esquerda) e Step Sequencer (direita) ---

    // Largura calculada para o Step Sequencer.
    // (NUM_STEPS * largura_por_step) + padding_extra_para_o_grupo
    int sequencerWidth = NUM_STEPS * 25 + 40;

    // Largura mínima necessária para todos os controles do lado esquerdo.
    // 150 (Load/Play) + 80 (LPF) + 80 (HPF) + 160 (BPF) + 160 (Notch) + 240 (Peak)
    int minControlsWidth = 150 + 80 + 80 + 160 + 160 + 240; // Total: 870 pixels

    // 'controlsArea' recebe a largura mínima necessária para todos os knobs e botões de filtro.
    auto controlsArea = bounds.removeFromLeft(minControlsWidth);

    // 'sequencerArea' recebe o restante da largura disponível no 'bounds' original,
    // que agora contém apenas a parte direita após 'controlsArea' ser removida.
    auto sequencerArea = bounds;

    // --- 2. Posicione o Grupo do Step Sequencer e seus elementos internos ---

    // Define os limites do componente GroupComponent do Step Sequencer
    stepSequencerGroup.setBounds(sequencerArea);

    int buttonSize = 20; // Tamanho dos botões de step
    int stepSize = 25;   // Largura reservada para cada coluna de step
    int rowHeight = 35;  // Altura reservada para cada linha (track)
    int groupBorder = 15; // Borda interna do GroupComponent (ajuste se a borda da GroupComponent for diferente)

    // Atualiza as coordenadas de início X e Y para a barra amarela e os steps.
    // Isso é CRUCIAL para garantir que a barra amarela seja desenhada no local correto,
    // independentemente do redimensionamento da janela.
    stepStartX = stepSequencerGroup.getX() + groupBorder;        // X inicial dos botões do step sequencer
    stepYStart = stepSequencerGroup.getY() + groupBorder + 20; // Y inicial dos botões (20 para labels de step)

    // Ajusta a área interna do Step Sequencer (dentro do GroupComponent)
    auto sequencerContent = sequencerArea.reduced(groupBorder);

    // Posiciona os labels de número dos steps (1 a 16)
    auto labelsArea = sequencerContent.removeFromTop(20); // 20 pixels de altura para os labels
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setBounds(labelsArea.removeFromLeft(stepSize));
    }

    // Posiciona os botões individuais do Step Sequencer para cada track
    for (int i = 0; i < NUM_TRACKS; ++i)
    {
        auto trackArea = sequencerContent.removeFromTop(rowHeight); // Área para uma linha de steps
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            // Centraliza o botão dentro da sua célula de step
            stepButtons[i][step].setBounds(trackArea.removeFromLeft(stepSize).withSizeKeepingCentre(buttonSize, buttonSize));
        }
    }

    // --- 3. Posicione os Controles Globais (BPM) no topo da área de controles ---

    auto globalArea = controlsArea.removeFromTop(100); // 100 pixels de altura para os controles globais
    auto bpmArea = globalArea.removeFromLeft(100);     // 100 pixels de largura para o slider de BPM

    globalBpmLabel.setBounds(bpmArea.removeFromTop(20)); // Label acima do slider
    globalBpmSlider.setBounds(bpmArea);                   // Slider de BPM

    // --- 4. Posicione os Controles por Faixa (Sample e Filtros) ---

    int trackControlHeight = 120; // Altura reservada para cada faixa de controle de sample/filtro

    // Itera sobre cada faixa de sample para posicionar seus controles
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        // Define a área para a faixa de controle atual DENTRO do loop
        auto trackBounds = controlsArea.removeFromTop(trackControlHeight);

        // **AQUI: Mova a definição da lambda `createKnobSection` para DENTRO do loop.**
        // Agora, 'trackBounds' está no escopo correto para ser capturada pela lambda.
        auto createKnobSection = [&](juce::TextButton& toggle, juce::Slider& knob1, juce::Slider* knob2 = nullptr, juce::Slider* knob3 = nullptr)
        {
            // Ajusta a largura da seção baseada no número de knobs que ela contém
            int sectionWidth = 80; // Largura padrão para 1 knob (e o toggle)
            if (knob2) sectionWidth += 80; // Adiciona 80 se houver um segundo knob
            if (knob3) sectionWidth += 80; // Adiciona 80 se houver um terceiro knob

            // Remove a seção da área de trackBounds e define seus limites
            auto section = trackBounds.removeFromLeft(sectionWidth);
            toggle.setBounds(section.removeFromTop(25).reduced(5)); // Posição do botão de toggle
            auto knobArea = section; // O restante da seção é para os knobs

            // Posiciona os knobs individualmente
            knob1.setBounds(knobArea.removeFromLeft(80)); // Posição do 1º knob
            if (knob2) knob2->setBounds(knobArea.removeFromLeft(80)); // Posição do 2º knob
            if (knob3) knob3->setBounds(knobArea.removeFromLeft(80)); // Posição do 3º knob
        };

        // Seção de botões Load e Play Sample
        auto sampleControls = trackBounds.removeFromLeft(150); // 150 pixels de largura para Load/Play
        loadSampleButtons[i].setBounds(sampleControls.removeFromTop(30).reduced(5));
        playSampleButtons[i].setBounds(sampleControls.removeFromTop(30).reduced(5));

        // Aplica o helper para posicionar os diferentes tipos de filtro
        createKnobSection(filterToggleButtons[i], filterCutoffSliders[i]);
        createKnobSection(highpassToggleButtons[i], highpassCutoffSliders[i]);
        createKnobSection(bandpassToggleButtons[i], bandpassCutoffSliders[i], &bandpassBandwidthSliders[i]);
        createKnobSection(notchToggleButtons[i], notchCutoffSliders[i], &notchBandwidthSliders[i]);
        // O Peak Filter agora usa 3 knobs, passando os ponteiros para a lambda
        createKnobSection(peakToggleButtons[i], peakCutoffSliders[i], &peakGainSliders[i], &peakQSliders[i]);
    }
}


void SampleAudioProcessorEditor::timerCallback()
{
    currentStep = audioProcessor.getCurrentStep();
    repaint();
}

void SampleAudioProcessorEditor::handleFilterToggleLogic(int i, juce::TextButton& clickedButton)
{
    if (&clickedButton == &bandpassToggleButtons[i])
    {
        peakToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        audioProcessor.setPeakEnabled(i, false);
    }
    else if (&clickedButton == &peakToggleButtons[i])
    {
        bandpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        audioProcessor.setBandPassEnabled(i, false);
    }

    if (&clickedButton == &notchToggleButtons[i])
    {
        filterToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        highpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        bandpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        peakToggleButtons[i].setToggleState(false, juce::dontSendNotification);

        audioProcessor.setFilterEnabled(i, false);
        audioProcessor.setHighpassEnabled(i, false);
        audioProcessor.setBandPassEnabled(i, false);
        audioProcessor.setPeakEnabled(i, false);
    }
}

void SampleAudioProcessorEditor::setupToggleButton(juce::TextButton& button, const juce::String& text)
{
    button.setButtonText(text);
    button.setClickingTogglesState(true);
    addAndMakeVisible(button);
}
