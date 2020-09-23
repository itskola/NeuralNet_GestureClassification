#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <cstddef>

class Matrix
{
    static bool is_seeded;

    int rows, columns;
    double** elements;

    class RowHelper
    {
        double* row;

        public:
            RowHelper(double* row) {
                this->row = row;
            }

            double& operator[](int col_i) const { return row[col_i]; }
    };

    public:
        static int ZEROES, RANDOM;

        Matrix(int rows, int columns, int fill_with = ZEROES);
        Matrix() : Matrix(0, 0, ZEROES) {}
        Matrix(const Matrix& toCopy) { copy(toCopy); }

        ~Matrix();

        int rowCount() const    { return rows   ; }
        int columnCount() const { return columns; }

        Matrix transpose();

        void print();

        RowHelper operator[](const int row) const { return RowHelper(elements[row]); }

        Matrix& operator=(const Matrix& toCopy) {
            this->~Matrix();
            copy(toCopy);

            return *this;
        }

        static bool are_of_same_dim(const Matrix& lhs, const Matrix& rhs) {
            if ((lhs.rows == rhs.rows) && (lhs.columns == rhs.columns)) { return true ; }
                else                                                    { return false; }
        }

        static void add_or_subtract(Matrix& lhs, const Matrix& rhs, int sign) {
            for (int i = 0; i < lhs.rowCount(); ++i) {
                for (int j = 0; j < lhs.columnCount(); ++j) { lhs[i][j] += (sign*rhs[i][j]); }
            }
        }

    private:
        void copy(const Matrix& toCopy);
};

#endif // MATRIX_H

// ============================================================================

Matrix operator*(const Matrix& lhs, const Matrix& rhs);
Matrix operator*(double scalar, Matrix matrix);
Matrix operator*(Matrix matrix, double scalar);

Matrix operator+(Matrix lhs, const Matrix& rhs);
Matrix operator-(Matrix lhs, const Matrix& rhs);

// ============================================================================
