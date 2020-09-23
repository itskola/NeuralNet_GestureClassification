#include "matrix.h"

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <iomanip>

bool Matrix::is_seeded = false;

int Matrix::ZEROES = 0;
int Matrix::RANDOM = 1;

Matrix::Matrix(int rows, int columns, int fill_with) :
    rows(rows), columns(columns)
{
    if (!is_seeded) {
        std::srand(std::time(nullptr)); is_seeded = true;
    }

    this->elements = new double*[rows];
    for (int i = 0; i < rows; ++i) {
        elements[i] = new double[columns];
        for (int j = 0; j < columns; ++j) {
            elements[i][j] = fill_with == 0 ? 0 : -1 + (static_cast<double>(std::rand()) / RAND_MAX) * (1 - (-1));
        }
    }
}

Matrix::~Matrix()
{
    for (int i = 0; i < rows; ++i) delete[] elements[i];
    delete[] elements;
}

Matrix Matrix::transpose() {
    Matrix t(this->columns, this->rows);
    for (int i = 0; i < t.rows; ++i) {
        for (int j = 0; j < t.columns; ++j) {
            t[i][j] = this->elements[j][i];
        }
    }
    return t;
}

void Matrix::print() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << std::setw(12) << elements[i][j];
            if (j < columns - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void Matrix::copy(const Matrix& toCopy) {
    this->rows = toCopy.rows;
    this->columns = toCopy.columns;

    this->elements = new double*[rows];
    for (int i = 0; i < rows; ++i) {
        elements[i] = new double[columns];
        for (int j = 0; j < columns; ++j) {
            elements[i][j] = toCopy[i][j];
        }
    }
}

// ============================================================================

#define ERROR_MSG "Matrices are not compatible";

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    if (lhs.columnCount() != rhs.rowCount()) throw ERROR_MSG;

    Matrix result(lhs.rowCount(), rhs.columnCount());
    for (int i = 0, n = lhs.rowCount(); i < n; ++i) {
        for (int k = 0; k < result.columnCount(); ++k) {
            for (int j = 0, m = lhs.columnCount(); j < m; ++j) {
                result[i][k] += lhs[i][j] * rhs[j][k];
            }
        }
    }
    return result;
}
Matrix operator*(double scalar, Matrix matrix) {
    for (int i = 0, n = matrix.rowCount(); i < n; ++i) {
        for (int j = 0, m = matrix.columnCount(); j < m; ++j) {
            matrix[i][j] *= scalar;
        }
    }
    return matrix;
}
Matrix operator*(Matrix matrix, double scalar) {
    return operator*(scalar, matrix);
}

Matrix operator+(Matrix lhs, const Matrix& rhs) {
    if (!Matrix::are_of_same_dim(lhs, rhs)) throw ERROR_MSG;

    Matrix::add_or_subtract(lhs, rhs, 1);
    return lhs;
}
Matrix operator-(Matrix lhs, const Matrix& rhs) {
    if (!Matrix::are_of_same_dim(lhs, rhs)) throw ERROR_MSG;

    Matrix::add_or_subtract(lhs, rhs, -1);
    return lhs;
}

// ============================================================================
