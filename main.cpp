#include <QApplication>

#include "neuralnetgui.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    NeuralNetGUI neuralNetGUI;
    neuralNetGUI.setWindowTitle("Neural Network");
    neuralNetGUI.show();

    return app.exec();
}
