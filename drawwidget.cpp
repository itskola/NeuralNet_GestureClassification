#include "drawwidget.h"

#include <QPainter>
#include <cmath>

void DrawWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}

void DrawWidget::resizeEvent(QResizeEvent*) {
    auto newRect = pixmap.rect().united(rect());
    if (newRect == pixmap.rect()) return;

    QPixmap newPixmap(newRect.size());

    QPainter painter(&newPixmap);
    painter.fillRect(newPixmap.rect(), Qt::white);
    painter.drawPixmap(0, 0, pixmap);

    pixmap = newPixmap;
}

void DrawWidget::mousePressEvent(QMouseEvent* event) {
    if (!(event->button() == Qt::LeftButton)) return;

    left_pressed = true;

    lastPosition = event->pos();
    saveCoordinates(lastPosition);

    draw(lastPosition);
}

void DrawWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!left_pressed) return;

    saveCoordinates(event->pos());

    draw(event->pos());
}

void DrawWidget::mouseReleaseEvent(QMouseEvent*) {
    processCoordinates();

    left_pressed = false;

    pixmap.fill();
}

void DrawWidget::draw(const QPoint& position) {
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen({Qt::black, 1.0});
    painter.drawLine(lastPosition, position);

    lastPosition = position;
    update();
}

void DrawWidget::saveCoordinates(const QPoint& p) {
    double x = p.x(), y = p.y();

    x_coords.push_back(x);
    y_coords.push_back(y);

    mean_x += x;
    mean_y += x;
}

void DrawWidget::processCoordinates() {
    if (x_coords.empty() || y_coords.empty()) return;

    mean_x /= x_coords.size();
    mean_y /= y_coords.size();

    double max_x = 0, max_y = 0;
    for (unsigned int i = 0, n = x_coords.size(); i < n; ++i) {
        x_coords[i] -= mean_x;
        if (std::abs(x_coords[i]) > max_x) { max_x = std::abs(x_coords[i]); }

        y_coords[i] -= mean_y;
        if (std::abs(y_coords[i]) > max_y) { max_y = std::abs(y_coords[i]); }
    }

    double max = std::max(max_x, max_y);
    for (unsigned int i = 0, n = x_coords.size(); i < n; ++i) {
        x_coords[i] /= max;
        y_coords[i] /= max;
    }

    sampleCoordinates();

    if (sampling == 1) {
        writeto_file();
        emit gestureDrawn();
    } else {
        std::vector<double> input = create_sample();
        emit gestureSampled(input);
    }

    resetMemebers();
}

void DrawWidget::sampleCoordinates() {
    double dist = 0;
    for (unsigned int i = 0, n = x_coords.size(); i < n - 1; ++i) {
        double x_dist = pow(x_coords[i + 1] - x_coords[i], 2);
        double y_dist = pow(y_coords[i + 1] - y_coords[i], 2);
        dist += sqrt(x_dist + y_dist);
    }

    std::vector<double> sampled_x, sampled_y;

    unsigned int index = 0; double temp_dist = 0;
    for (int k = 0; k < M; ++k) {
        double dist_from_1st = k * (dist / (M - 1));

        while (temp_dist < dist_from_1st) {
            temp_dist += sqrt(pow(x_coords[index + 1] - x_coords[index], 2) + pow(y_coords[index + 1] - y_coords[index], 2));
            ++index;

            if (index >= x_coords.size()) { break; }
        }

        sampled_x.push_back(x_coords[index == 0 ? 0 : index - 1]);
        sampled_y.push_back(y_coords[index == 0 ? 0 : index - 1]);
    }

    x_coords = sampled_x;
    y_coords = sampled_y;
}
