#include "neuralnetgui.h"
#include "ui_neuralnetgui.h"

#include "neuralnetwork.h"

#include <QFileDialog>

const std::string DEFAULT_FILENAME = "samples.txt";

NeuralNetGUI::NeuralNetGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NeuralNetGUI)
{
    ui->setupUi(this);

    output_file_name = DEFAULT_FILENAME;
    ui->drawWidget->set_output_filename(output_file_name);

    QRegExpValidator* naturalNum_val = new QRegExpValidator(QRegExp("[1-9][0-9]*"), this);
    ui->inputNumofDistinctGestures->setValidator(naturalNum_val);
    ui->inputNumofSamplesPer      ->setValidator(naturalNum_val);
    ui->inputSampling             ->setValidator(naturalNum_val);

    ui->inputInputLayer           ->setValidator(naturalNum_val);
    ui->inputOutputLayer          ->setValidator(naturalNum_val);

    QRegExpValidator* hiddenLayers_val = new QRegExpValidator(QRegExp("[1-9][0-9]*(,[1-9][0-9]*)*"), this);
    ui->inputHiddenLayers->setValidator(hiddenLayers_val);

    ui->inputSampling->setFocus();
}

NeuralNetGUI::~NeuralNetGUI()
{
    delete neuralNet;
    delete ui;
}

void NeuralNetGUI::on_inputNumofDistinctGestures_textChanged(const QString&) { enableStart(); }

void NeuralNetGUI::on_inputNumofSamplesPer_textChanged(const QString&)       { enableStart(); }

void NeuralNetGUI::on_inputSampling_textChanged(const QString&)              { enableStart(); }

void NeuralNetGUI::on_buttonStart_clicked() {
    which_distinct = 0;

    ui->drawWidget->setNumofDotsPerSample(ui->inputSampling->text().toInt());

    start_restart(true);
    setGestureCode();
}

void NeuralNetGUI::on_buttonRestart_clicked() {
    ui->tab_NeuralNetParameters->setEnabled(false);

    ui->drawWidget->samplingPhase();
    ui->drawWidget->clear_file();

    output_file_name = DEFAULT_FILENAME; total_drawn = 0;
    ui->lineEditGestureCode->setAlignment(Qt::AlignRight);

    start_restart(false);
    statusBar()->showMessage("");
}

void NeuralNetGUI::on_drawWidget_gestureDrawn() {
    ++total_drawn;
    statusBar()->showMessage(QString::number(which_distinct) + ": sample #" + QString::number(total_drawn));

    if (total_drawn == ui->inputNumofSamplesPer->text().toInt()) {
        total_drawn = 0;
        setGestureCode();
    }
}

void NeuralNetGUI::enableStart() {
    if (    !ui->inputNumofDistinctGestures->text().isEmpty() &&
            !ui->inputNumofSamplesPer->text().isEmpty()       &&
            !ui->inputSampling->text().isEmpty()                    ) {
        ui->buttonStart->setEnabled(true);
    } else {
        ui->buttonStart->setEnabled(false);
    }
}

void NeuralNetGUI::start_restart(bool enabled) {
    ui->inputNumofDistinctGestures->setEnabled(!enabled);
    ui->inputNumofSamplesPer->setEnabled(!enabled);
    ui->inputSampling->setEnabled(!enabled);

    ui->labelGestureCode->setEnabled(enabled);
    ui->lineEditGestureCode->setEnabled(enabled);
    ui->drawWidget->setEnabled(enabled);

    ui->buttonStart->setEnabled(!enabled);
    ui->buttonRestart->setEnabled(enabled);
}

void NeuralNetGUI::setGestureCode() {
    ++which_distinct;
    if (which_distinct > ui->inputNumofDistinctGestures->text().toInt()) {
        ui->drawWidget->finished(); ui->drawWidget->setEnabled(false);

        ui->lineEditGestureCode->setText("");
        setupParameters();

        statusBar()->showMessage("Finished with sampling, go setup neural network parameters");
        return;
    }

    std::string code(static_cast<unsigned int>(ui->inputNumofDistinctGestures->text().toInt()), '0');
    code[static_cast<unsigned int>(which_distinct - 1)] = '1';

    ui->drawWidget->setCode(code);

    ui->lineEditGestureCode->setText(QString::fromStdString(code));
    if (which_distinct == 1) {
        statusBar()->showMessage("You can start sampling");
    } else {
        statusBar()->showMessage("Proceed with next gesture");
    }
}

void NeuralNetGUI::setupParameters() {
    ui->inputInputLayer->setText( QString::number(2 * ui->inputSampling->text().toInt()) );
    ui->inputOutputLayer->setText( QString::number(ui->inputNumofDistinctGestures->text().toInt()) );

    ui->tab_NeuralNetParameters->setEnabled(true);
    if (!ui->inputHiddenLayers->text().isEmpty()) {
        ui->buttonGO->setEnabled(true);
    } else {
        ui->buttonGO->setEnabled(false);
    }

    ui->inputHiddenLayers->setFocus();
}

void NeuralNetGUI::on_inputHiddenLayers_textChanged(const QString& text) {
    if (!text.isEmpty()) {
        ui->buttonGO->setEnabled(true);
    } else {
        ui->buttonGO->setEnabled(false);
    }
}

void NeuralNetGUI::on_buttonGO_clicked() {
    // if using existing data
        ui->drawWidget->setNumofDotsPerSample(ui->inputInputLayer->text().toInt() / 2);

    std::vector<int> layers;

    layers.push_back(ui->inputInputLayer->text().toInt());
    QStringList hidden_layers = ui->inputHiddenLayers->text().split(",", QString::SkipEmptyParts);
    for (QString layer : hidden_layers) {
        QString qs = layer.trimmed(); if (qs.isEmpty()) { continue; }
        layers.push_back(qs.toInt());
    }
    layers.push_back(ui->inputOutputLayer->text().toInt());

    statusBar()->showMessage("Training of neural network in progress...");

    delete neuralNet;
    neuralNet = new NeuralNetwork(layers, output_file_name, ui->comboBoxBackpropagationType->currentIndex() + 1);

    statusBar()->showMessage("Finised with training, you can now test neural network", 8000);
    testingPhase();
}

void NeuralNetGUI::on_buttonLoadExistingData_clicked() {
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Load samples",
                                                    QDir::currentPath(),
                                                    "Text files (*.txt)");
    if (filename.isEmpty()) return;

    output_file_name = filename.toStdString();

    ui->tab_NeuralNetParameters->setEnabled(true);
    ui->tabWidget->setCurrentIndex(1);
}

void NeuralNetGUI::testingPhase() {
    ui->drawWidget->testingPhase();
    ui->drawWidget->setEnabled(true);

    ui->lineEditGestureCode->setEnabled(true);
    ui->lineEditGestureCode->setAlignment(Qt::AlignHCenter);
}

void NeuralNetGUI::on_drawWidget_gestureSampled(std::vector<double> input) {
    std::vector<double> output = neuralNet->test(input);

    double max = output[0]; unsigned int max_i = 0;
    for (unsigned int i = 0; i < output.size(); ++i) {
        if (output[i] > max) {
            max = output[i]; max_i = i;
        }
    }
    QString classification(output.size(), '0'); classification[max_i] = '1';

    ui->lineEditGestureCode->setText(classification);
}
