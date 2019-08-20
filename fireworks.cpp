#include "fireworks.h"
#include "fireworks_impl.h"

#include <QPaintEvent>
#include <QResizeEvent>
#if 0
#include <QMouseEvent>
#include <QWheelEvent>
#endif
#include <QKeyEvent>
#include <QEvent>
#include <QTimer>
#include <math.h>

#ifdef _DEBUG || DEBUG
#include <QDebug>
#endif // _DEBUG || DEBUG


fireworks::fireworks(QWidget* parent, Qt::WindowFlags f):
    QWidget (parent, f),
    m_impl(new fireworks_impl()),
    m_timer(new QTimer)
{
    m_timer->connect(this->m_timer, SIGNAL(timeout()), this, SLOT(frame()));
    m_timer->start(15);
	initVars(width() / 2, height() / 2);

	// 设置背景黑色
	QPalette pal(palette());
	pal.setColor(QPalette::Background, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	// 捕获按键事件
	grabKeyboard();

	// 设置为不按下鼠标键触发moveEvent
	setMouseTracking(true);

	// 不显示鼠标
    setCursor(QCursor(Qt::BlankCursor));
}

fireworks::~fireworks()
{
    delete m_impl;
	delete m_timer;
}

void fireworks::initVars(double cx, double cy)
{
    m_impl->initVars(cx, cy);
}

raster_point fireworks::rasterizePoint(double x, double y, double z)
{
    return m_impl->rasterizePoint(x, y, z);
}

void fireworks::spawnSeed()
{
    m_impl->spawnSeed();
}

void fireworks::splode(double x, double y, double z)
{
    m_impl->splode(x, y, z);
}

void fireworks::doLogic()
{
    m_impl->doLogic();
}

void fireworks::draw(QPainter &painter)
{
    m_impl->draw(painter);
}

void fireworks::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    draw(painter);
    doLogic();
}

void fireworks::resizeEvent(QResizeEvent * event)
{
	QSize size = event->size();
	m_impl->initVars(size.width() / 2, size.height() / 2);
	QWidget::resizeEvent(event);
}

#if 0
void fireworks::mouseMoveEvent(QMouseEvent * event)
{
	static QPoint old = event->globalPos();

	QPoint now = event->globalPos();
	int dx = (now.x() - old.x()) >> 2;
	int dy = (now.y() - old.y()) >> 2;
	old = now;
	
	if (abs(dx) > 20 || abs(dy) > 20) {
		return;
	}
	
	int Y = (int)(m_impl->getYaw() * 180 / 3.14 + dx) % 360;
	int P = (int)(m_impl->getPitch() * 180 / 3.14 + dy) % 180;

	double yaw = Y * 3.14 / 180;
	double pitch = P * 3.14 / 180;
	
	m_impl->setYaw(yaw);
	m_impl->setPitch(pitch);

#ifdef _DEBUG || DEBUG
	qDebug() << "yaw: " << Y << "pitch: " << P << endl;
#endif
}

void fireworks::wheelEvent(QWheelEvent * event)
{
	QPoint nowDegrees = event->angleDelta() / 120;
	point p = m_impl->getPlayer();
	double yaw = m_impl->getYaw();
	double dz = cos(yaw) * nowDegrees.y();
	double dx = sin(yaw) * nowDegrees.y();

	p.z = p.z + dz;
	p.x = p.x + dx;
	p.y += nowDegrees.y();

	m_impl->setPlayer(p);
	QWidget::wheelEvent(event);

#ifdef _DEBUG || DEBUG
	qDebug() << "angle: " << event->angleDelta() << endl;
#endif
}

#endif

void fireworks::keyPressEvent(QKeyEvent * event)
{
	Qt::Key k = (Qt::Key)event->key();
	point p = m_impl->getPlayer();
	double yaw = m_impl->getYaw();
	double dz = 0; 
	double dx = 0;
    double dy = 0;

	switch (k)
	{
	case Qt::Key_F1:
		m_impl->set_automove(!m_impl->getAutoMove());
		return;
	case Qt::Key_W:
        dz = cos(yaw);
        dx = sin(yaw);
		break;
	case Qt::Key_S:
		dz = -cos(yaw);
		dx = -sin(yaw);
		break;
	case Qt::Key_A:
		dz = sin(yaw);
		dx = -cos(yaw);
		break;
	case Qt::Key_D:
		dz = -sin(yaw);
		dx = cos(yaw);
		break;
    case Qt::Key_Up:
        m_impl->setPitch(m_impl->getPitch() - 0.03);
        return;
    case Qt::Key_Down:
        m_impl->setPitch(m_impl->getPitch() + 0.03);
        return;
    case Qt::Key_Left:
        m_impl->setYaw(m_impl->getYaw() - 0.03);
        return;
    case Qt::Key_Right:
        m_impl->setYaw(m_impl->getYaw() + 0.03);
        return;
    case Qt::Key_Space:
        --dy;
        break;
    case Qt::Key_Z:
        ++dy;
		break;
	default:
		return;
	}

    p.z += dz * 2;
    p.y += dy * 2;
    p.x += dx * 2;

	m_impl->setPlayer(p);
	QWidget::keyPressEvent(event);
}

void fireworks::frame()
{
    m_impl->frame();
    update();
}

