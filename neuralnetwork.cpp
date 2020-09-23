#include "neuralnetwork.h"

#include "sigmoidfunction.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

using namespace std;

namespace help {
    void values_based_on_function(Matrix& y, Function1D* f, Matrix& x) {
        for (int i = 0; i < y.rowCount(); ++i) {
            for (int j = 0; j < y.columnCount(); ++j) {
                y[i][j] = (*f)(x[i][j]);
            }
        }
    }

    void set_values(Matrix& m, vector<double> vec) {
        for (int i = 0, r = m.rowCount(); i < r; ++i) {
            for (int j = 0, c = m.columnCount(); j < c; ++j) {
                m[i][j] = r > c ? vec[i] : vec[j];
            }
        }
    }

    void prepare_for_mini_batch(vector<vector<double>>& inputs, vector<vector<double>>& outputs, int samples_per_gesture) {
        vector<vector<double>> mb_inputs, mb_outputs;

        int numof_per_gesture = 0;
        while (outputs[0] == outputs[numof_per_gesture]) { ++numof_per_gesture; }

        int numof_distinct = 0;
        for (unsigned int i = 0; i < outputs.size(); i += numof_per_gesture) { ++numof_distinct; }

        int offset = 0; int count = 0;
        while (mb_outputs.size() != outputs.size()) {
            for (int i = offset; i < offset + samples_per_gesture; ++i) {
                mb_inputs.push_back(inputs[i]);
                mb_outputs.push_back(outputs[i]);
            }
            offset += numof_per_gesture;

            ++count;
            if (count == numof_distinct) {
                count = 0;
                offset = mb_outputs.size() / numof_distinct;
            }
        }

        inputs = mb_inputs; outputs = mb_outputs;
    }
}

int NeuralNetwork::BATCH      = 1;
int NeuralNetwork::STOCHASTIC = 2;
int NeuralNetwork::MINI_BATCH = 3;

double NeuralNetwork::ETA              = 0.065;
double NeuralNetwork::ERROR_TRESHOLD   = 1e-9;
unsigned int NeuralNetwork::MAX_EPOCHS = static_cast<unsigned int>(1e5);

NeuralNetwork::NeuralNetwork(vector<int> sizeof_layers, string samples, int type)
{
    for (int l = 0, n = sizeof_layers.size(); l < n; ++l) {
        layers.push_back(Matrix(1,
                                l < n - 1 ? sizeof_layers[l] + 1 : sizeof_layers[l]));

        if (l < n - 1) {
            weights.push_back(Matrix(sizeof_layers[l] + 1,
                                     l + 1 == n - 1 ? sizeof_layers[l + 1] : sizeof_layers[l + 1] + 1,
                                     Matrix::RANDOM));
        }
    }
    f = new SigmoidFunction();

    train(samples, type);
}

int NeuralNetwork::getData(string samples,
                           vector<vector<double>>& inputs, vector<vector<double>>& outputs) {
    ifstream read_from(samples);
    if (!read_from) {
        cout << samples << " doesn't exist/can't open " << samples << endl;
        return -1;
    }

    string line;
    while (getline(read_from, line)) {
        istringstream line_stream(line);

        vector<double> input, output;

        double value; input.push_back(1);
        for (int i = 0, n = layers[0].columnCount() - 1; i < n; ++i) {
            line_stream >> value; input.push_back(value);
        }

        string sep; line_stream >> sep;

        for (int i = 0, n = layers[layers.size() - 1].columnCount(); i < n; ++i) {
            line_stream >> value; output.push_back(value);
        }

        inputs.push_back(input); outputs.push_back(output);
    }
    return 1;
}

void NeuralNetwork::train(string samples, int type) {
    Samples inputs, outputs;
    if (getData(samples, inputs, outputs) == -1) return;

    int when_to_update = -1;
    if (type == BATCH) {
        when_to_update = inputs.size();
    }
    else if (type == STOCHASTIC) {
        when_to_update = 1;
    }
    else if (type == MINI_BATCH) {
        when_to_update = 10;
        if (when_to_update > inputs.size()) { when_to_update = inputs.size(); }
        help::prepare_for_mini_batch(inputs, outputs, 2);
    }

    unsigned int epoch = 0;
    while (epoch < MAX_EPOCHS) {
        int update_counter = 0;

        vector<Matrix> updates; double error = 0;
        for (unsigned int s = 0, N = inputs.size(); s < N; ++s) {
            vector<double> input = inputs[s], output = outputs[s];

            propagateInput(input);
            error += squaredError(output, layers[layers.size() - 1]);

            Errors layerErrors(layers.size() - 1); vector<Matrix> sampleUpdates;
            for (unsigned int l = layers.size() - 1; l >= 1; --l) {
                Matrix& layer = layers[l];

                vector<double> delta_err;
                if (l == layers.size() - 1) {
                    for (int c = 0; c < layer.columnCount(); ++c) {
                        delta_err.push_back(f->gradient(layer[0][c]) * (output[c] - layer[0][c]));
                    }
                } else {
                    Matrix& w = weights[l];
                    for (int c = 0; c < layer.columnCount(); ++c) {
                        double next_err = 0;
                        for (int next_c = 0; next_c < layers[l + 1].columnCount(); ++next_c) {
                            next_err += layerErrors[l][next_c] * w[c][next_c];
                        }
                        delta_err.push_back(f->gradient(layer[0][c]) * next_err);
                    }
                }
                layerErrors[l - 1] = delta_err;

                Matrix temp(1, delta_err.size());
                for (int j = 0; j < temp.columnCount(); ++j) { temp[0][j] = delta_err[j]; }

                sampleUpdates.insert(sampleUpdates.begin(), layers[l - 1].transpose() * temp);
            }

            if (updates.empty()) {
                for (unsigned int i = 0; i < sampleUpdates.size(); ++i) { updates.push_back(sampleUpdates[i]); }
            } else {
                for (unsigned int i = 0; i < updates.size(); ++i) { updates[i] = updates[i] + sampleUpdates[i]; }
            }

            ++update_counter;
            if (update_counter == when_to_update) {
                update_counter = 0;

                updateWeights(updates);
                updates.clear();

                ++epoch;
                if (epoch >= MAX_EPOCHS) { break; }
            }
        }

        cout << "epoch " << epoch << ", learning error = " << error / inputs.size() << endl;

        if (error <= ERROR_TRESHOLD) return;
    }
}

void NeuralNetwork::propagateInput(const vector<double>& input) {
    help::set_values(layers[0], input);
    for (unsigned int w = 0; w < weights.size(); ++w) {
        Matrix net = layers[w] * weights[w];
        help::values_based_on_function(layers[w + 1], f, net);
    }
}

void NeuralNetwork::updateWeights(const vector<Matrix>& updates) {
    for (unsigned int k = 0; k < updates.size(); ++k) {
        weights[k] = weights[k] + (ETA * updates[k]);
    }
}

double NeuralNetwork::squaredError(const vector<double>& desired, const Matrix& calculated) {
    double total = 0;
    for (int i = 0, n = calculated.columnCount(); i < n; ++i) {
        total += pow(desired[i] - calculated[0][i], 2);
    }
    return total;
}

vector<double> NeuralNetwork::test(vector<double> input) {
    input.insert(input.begin(), 1); propagateInput(input);

    vector<double> v_out;
        Matrix& output = layers[layers.size() - 1];
        for (int i = 0; i < output.columnCount(); ++i) { v_out.push_back(output[0][i]); }
    return v_out;
}
