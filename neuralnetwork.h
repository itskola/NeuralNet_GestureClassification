#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <matrix.h>
#include <function1d.h>

#include <string>
#include <vector>

class NeuralNetwork
{
    typedef std::vector<Matrix> Layers;
    typedef std::vector<Matrix> Weights;

    typedef std::vector<std::vector<double>> Errors;
    typedef std::vector<std::vector<double>> Samples;

    Layers layers;
    Weights weights;

    Function1D* f;

    public:
        NeuralNetwork(std::vector<int> sizeof_layers, std::string samples, int type = BATCH);

        ~NeuralNetwork() { delete f; }

        static int BATCH, STOCHASTIC, MINI_BATCH;

        static double ETA, ERROR_TRESHOLD;
        static unsigned int MAX_EPOCHS;

        std::vector<double> test(std::vector<double> input);

    private:
        int getData(std::string samples, std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& outputs);

        void train(std::string samples, int type);
        void propagateInput(const std::vector<double>& input);
        void updateWeights(const std::vector<Matrix>& updates);

        double squaredError(const std::vector<double>& desired, const Matrix& calculated);
};

#endif // NEURALNETWORK_H
