#ifndef NEURALNETGUI_H
#define NEURALNETGUI_H

#include <QMainWindow>

class NeuralNetwork;

namespace Ui {
    class NeuralNetGUI;
}

class NeuralNetGUI : public QMainWindow
{
        Q_OBJECT

    Ui::NeuralNetGUI *ui;

    NeuralNetwork* neuralNet = nullptr;

    int which_distinct = 0, total_drawn = 0;
    std::string output_file_name;

    public:
        NeuralNetGUI(QWidget *parent = nullptr);
        ~NeuralNetGUI();

    private slots:
        void on_inputNumofDistinctGestures_textChanged(const QString& text);
        void on_inputNumofSamplesPer_textChanged(const QString& text);
        void on_inputSampling_textChanged(const QString &arg1);

        void on_buttonStart_clicked();
        void on_buttonRestart_clicked();

        void on_drawWidget_gestureDrawn();

        void on_inputHiddenLayers_textChanged(const QString& text);

        void on_buttonGO_clicked();

        void on_buttonLoadExistingData_clicked();

        void on_drawWidget_gestureSampled(std::vector<double> input);

    private:
        void enableStart();
        void start_restart(bool enabled);

        void setGestureCode();

        void setupParameters();

        void testingPhase();
};

#endif // NEURALNETGUI_H
