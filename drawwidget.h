#ifndef DRAWWIDGET_H
#define DRAWTWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QString>

#include <vector>
#include <iostream>
#include <fstream>

#include <QDebug>

class DrawWidget : public QWidget
{
        Q_OBJECT

    QPixmap pixmap;
    QPoint lastPosition;
    bool left_pressed;

    std::string filename;
    std::ofstream write_to;

    int M; std::string code;

    std::vector<double> x_coords, y_coords;
    double mean_x = 0, mean_y = 0;

    bool sampling = true;

    public:
        DrawWidget(QWidget* parent) : QWidget(parent) {}

        ~DrawWidget() override {}

    signals:
        void gestureDrawn();
        void gestureSampled(std::vector<double> input);

    public:
        void setEnabled(bool enabled) { pixmap.fill(); QWidget::setEnabled(enabled); }

        void set_output_filename(std::string filename) { this->filename = filename; }

        void setNumofDotsPerSample(int how_much) { M = how_much; }

        void setCode(const std::string& code) {
            if (!this->code.empty()) { this->code.clear(); }

            for (unsigned int i = 0, n = code.size(); i < n; ++i) {
                this->code.push_back(code[i]);
                if (i < n - 1) { this->code.push_back(' '); }
            }
        }

        void clear_file() { write_to.close(); open_file(); }
        void finished()   { write_to.close();              }

        void samplingPhase() { sampling = true;  }
        void testingPhase()  { sampling = false; }

    private:
        void inline resetMemebers() {
            x_coords.clear(); y_coords.clear();
            mean_x = mean_y = 0;
        }

        void open_file() {
            if (write_to.is_open()) return;

            write_to.open(filename);
            if (!write_to) {
                QMessageBox::warning(this,
                                     "PaintWidget",
                                     "Cannot open " + QString::fromStdString(filename));
            }
        }

        void writeto_file() {
            open_file();

            for (unsigned int i = 0, n = x_coords.size(); i < n; ++i) {
                write_to << x_coords[i] << " " << y_coords[i];
                if (i < n - 1) { write_to << " "; }
            }
            write_to << " , " << code << std::endl;
        }

        std::vector<double> create_sample() {
            std::vector<double> input;
            for (unsigned int i = 0, n = x_coords.size(); i < n; ++i) {
                input.push_back(x_coords[i]);
                input.push_back(y_coords[i]);
            }
            return input;
        }

        void paintEvent(QPaintEvent*) override;

        void resizeEvent(QResizeEvent*) override;

        void mousePressEvent(QMouseEvent* event) override;

        void mouseMoveEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent*) override;

        void draw(const QPoint& position);

        void saveCoordinates(const QPoint& p);

        void processCoordinates();

        void sampleCoordinates();
};

#endif // DRAWWIDGET_H
