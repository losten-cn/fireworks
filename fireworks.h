#ifndef FIREWORKS_H
#define FIREWORKS_H

#include <QWidget>
#include <QPainter>
#include <vector>

struct color {
    int r;
    int g;
    int b;
};

struct point {
    double x;
    double y;
    double z;
};

struct raster_point {
    double x;
    double y;
    double d;
};

struct seed {
    point p;
    point vp;
    double born;
};

struct spark {
    point p;
    point vp;
    double radius;
    double alpha;
    unsigned col_index;
    std::vector<point> trail;
};

class QTimer;
class QPaintEvent;
class QResizeEvent;
#if 0
class QMouseEvent;
class QWheelEvent;
#endif
class QKeyEvent;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
class fireworks_impl;
class fireworks :
        public QWidget
{
    Q_OBJECT
public:
    fireworks(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~fireworks();
    void initVars(double cx, double cy);
    raster_point rasterizePoint(double x, double y, double z);
    void spawnSeed();
    void splode(double x, double y, double z);
    void doLogic();
    void draw(QPainter &painter);

protected:
    virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
#if 0
    virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
#endif
	virtual void keyPressEvent(QKeyEvent *event);

protected Q_SLOTS:
    void frame();

private:
    fireworks_impl* m_impl;
    QTimer* m_timer;

};

#endif // FIREWORKS_H
