#include "fireworks_impl.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>

#ifdef _DEBUG || DEBUG
#include <QDebug>
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON 0.000001
#endif

#define IS_GREATER_THAN(a, b) (a > b + DBL_EPSILON)

#define IS_EQUAL(a, b) (fabs(a - b) < DBL_EPSILON)

#define N 999   //精度为小数点后三位

#define random() ((double)(rand() % (N + 1)) / (N + 1))

#ifndef M_PIl
# define M_PIl      3.141592653589793238462643383279502884L /* pi */
#endif

std::vector<color> fireworks_impl::m_col;

fireworks_impl::fireworks_impl():
    m_frames(0),
	m_automove(true)
{
}

void fireworks_impl::initVars(double cx, double cy)
{
    m_cx = cx;
    m_cy = cy;
    m_player.x = 0;
    m_player.y = 0;
    m_player.z = -25;
    m_playerV.x = 0;
    m_playerV.y = 0;
    m_playerV.z = 0;
    m_pitch = m_pitchV = 0;
    m_yaw = m_yawV = 0;
    m_scale = 600;
    m_seedTimer = 0;
    m_seedInterval = 5;
    m_seedLife = 100;
    m_gravity = 0.02;
    srand((int)time(NULL));
    if (m_col.empty()) {
        m_col.push_back(color{ 255, 136,  67});
        m_col.push_back(color{ 136, 67,  255});
        m_col.push_back(color{ 136, 255,  255});
        m_col.push_back(color{ 255, 255,  255});
        m_col.push_back(color{ 67, 255,  136});
        m_col.push_back(color{ 255, 67,  67});
        m_col.push_back(color{ 255, 136,  67});
        m_col.push_back(color{ 136, 67,  255});
        m_col.push_back(color{ 255, 255,  255});
        m_col.push_back(color{ 67, 67,  255});
    }
}

raster_point fireworks_impl::rasterizePoint(double x, double y, double z)
{
	raster_point result;
    double p, d;
    x -= m_player.x;
    y -= m_player.y;
    z -= m_player.z;
    p = atan2(x, z);
    d = sqrt(x*x + z*z);
    x = sin(p - m_yaw) * d;
    z = cos(p - m_yaw) * d;
    p = atan2(y, z);
    d = sqrt(y*y + z*z);
    y = sin(p - m_pitch) * d;
    z = cos(p - m_pitch) * d;

    double rx1 = -1000, ry1 = 1;
    double rx2 = 1000, ry2 = 1;
    double rx3 = 0, ry3 = 0;
    double rx4 = x, ry4 = z;
    double uc=(ry4-ry3)*(rx2-rx1)-(rx4-rx3)*(ry2-ry1);

	// if (uc == 0) {
    if (IS_EQUAL(uc, 0)) {
		result.x = 0;
		result.y = 0;
		result.d = -1;
        return result;
    }

    double ua=((rx4-rx3)*(ry1-ry3)-(ry4-ry3)*(rx1-rx3))/uc;
    double ub=((rx2-rx1)*(ry1-ry3)-(ry2-ry1)*(rx1-rx3))/uc;

    //if (z == 0) {
	if (IS_EQUAL(z, 0)) {
        z = 0.000000001;
    }

	if (IS_GREATER_THAN(ua, 0) && IS_GREATER_THAN(1, ua) &&
		IS_GREATER_THAN(ub, 0) && IS_GREATER_THAN(1, ub)) {
		result.x = m_cx + (rx1 + ua * (rx2 - rx1)) * m_scale;
		result.y = m_cy + y / z * m_scale;
		result.d = sqrt(x * x + y * y + z * z);
		return result;
	} else {
		result.x = m_cx + (rx1 + ua * (rx2 - rx1)) * m_scale;
		result.y = m_cy + y / z * m_scale;
		result.d = -1;
		return result;
	}
}

void fireworks_impl::spawnSeed()
{
    seed s;
    s.p.x = -50 + random() * 100;
    s.p.y = 25;
    s.p.z = -50 + random() * 100;
    s.vp.x = 0.1 - random() * 0.2;
    s.vp.y = - 1.5;
    s.vp.z = 0.1 - random() * 0.2;
    s.born = m_frames;
    m_seeds.push_back(s);
}

void fireworks_impl::splode(double x, double y, double z)
{
    int t = 5 + rand() % 150;
    double sparkV = 1 + random() * 2.5;
    int type = rand() % 3;
    int pic1 = 0, pic2 = 0, pic3 = 0;
    switch (type) {
    case 0:
        pic1 = rand() % 10;
        break;
    case 1:
        pic1 = rand() % 10;
        do {pic2 = rand() % 10;} while(pic1 == pic2);
        break;
    case 2:
        pic1 = rand() % 10;
        do {pic2 = rand() % 10;} while(pic1 == pic2);
        do {pic3 = rand() % 10;} while(pic3 == pic1 || pic3 == pic2);
        break;
    }

    for (int i = 0; i < t; ++i) {
        spark sp;
        sp.p.x = x;
        sp.p.y = y;
        sp.p.z = z;
        double p1 = M_PIl * 2 * random();
        double p2 = M_PIl * random();
        double v = sparkV * (1 + random() / 6);
        sp.vp.x = sin(p1) * sin(p2) * v;
        sp.vp.z = cos(p1) * sin(p2) * v;
        sp.vp.y = cos(p2) * v;

        switch (type) {
		case 0:
			sp.col_index = pic1;
			break;
        case 1:
            sp.col_index = rand() % 2 ? pic1 : pic2;
            break;
        case 2:
            switch (rand() % 3) {
            case 0: sp.col_index = pic1; break;
            case 1: sp.col_index = pic2; break;
            case 2: sp.col_index = pic3; break;
            }
            break;
        }
        sp.radius = 25 + random() * 50;
        sp.alpha = 1;
        m_sparks.push_back(sp);
    }
}

void fireworks_impl::doLogic()
{
    if (m_seedTimer < m_frames) {
		int r = rand() % 10;
        m_seedTimer = m_frames + m_seedInterval * r;
        spawnSeed();
    }

    for (std::vector<seed>::iterator it = m_seeds.begin();
         it != m_seeds.end();) {
        (*it).vp.y += m_gravity;
        (*it).p.x += (*it).vp.x;
        (*it).p.y += (*it).vp.y;
        (*it).p.z += (*it).vp.z;
        if (m_frames - (*it).born > m_seedLife) {
			splode((*it).p.x, (*it).p.y, (*it).p.z);
            it = m_seeds.erase(it);
        } else {
            ++it;
        }
    }

    for (std::vector<spark>::iterator it = m_sparks.begin();
         it != m_sparks.end();) {
        //if ((*it).alpha > 0 && (*it).radius > 5) {
		if (IS_GREATER_THAN((*it).alpha, 0) &&
			IS_GREATER_THAN((*it).radius, 5)) {
            (*it).alpha -= 0.01;
            (*it).radius /=1.02;
            (*it).vp.y += m_gravity;
            point p = (*it).p;
            if (!(*it).trail.empty()) {
                double x = (*it).trail[(*it).trail.size() - 1].x;
                double y = (*it).trail[(*it).trail.size() - 1].y;
                double z = (*it).trail[(*it).trail.size() - 1].z;
                double d = ((p.x-x)*(p.x-x)+(p.y-y)*(p.y-y)+(p.z-z)*(p.z-z));
                if (d > 9) {
                    (*it).trail.push_back(p);
                }
            } else {
                (*it).trail.push_back(p);
            }
            if ((*it).trail.size() > 5)
                (*it).trail.erase((*it).trail.begin());
            (*it).p.x += (*it).vp.x;
            (*it).p.y += (*it).vp.y;
            (*it).p.z += (*it).vp.z;
            (*it).vp.x /= 1.075;
            (*it).vp.y /= 1.075;
            (*it).vp.z /= 1.075;
            ++it;
        } else {
            it = m_sparks.erase(it);
        }
    }

	if (m_automove) {
		double p = atan2(m_player.x, m_player.z);
		double d = sqrt(m_player.x * m_player.x + m_player.z * m_player.z);
        d += sin(m_frames / 80) / 1.25;
        double t = sin(m_frames / 200) / 40;
		m_player.x = sin(p + t) * d;
		m_player.z = cos(p + t) * d;
		m_yaw = M_PIl + p + t;
	}
}

void fireworks_impl::draw(QPainter &painter)
{
    // [*] 保存 QPainter 的状态
    QPen pen = painter.pen();
    QBrush brush = painter.brush();
    QFont font = painter.font();

    double size = 0;
    double d = 0;
    double x = 0, y = 25, z = 0;
    int a = 0;
    for (int i = -100; i < 100; i += 3) {
        for (int j = -100; j < 100; j+= 4) {
            x = i;
            z = j;
            raster_point rp =rasterizePoint(x, y, z);
            //if (rp.d != -1) {
			if (!IS_EQUAL(rp.d, -1)) {
                size = 250 / (1 + rp.d);
                d = sqrt(x * x + z * z);
                a = (0.75 - pow(d / 100, 6) * 0.75) * 255;
                if (IS_GREATER_THAN(a, 0) &&
                    IS_GREATER_THAN(rp.x - size / 2, 0) && IS_GREATER_THAN(2 * m_cx, rp.x - size / 2) &&
                    IS_GREATER_THAN(rp.y - size / 2, 0) && IS_GREATER_THAN(2 * m_cy, rp.y - size / 2)) {
                    painter.setBrush(QColor(255, 255, 136, a));
                    painter.drawRect(rp.x - size / 2, rp.y - size / 2, size, size);
                    //glColor4f(255, 255, 136, a);
                    //glRectf(rp.x - size / 2, rp.y - size / 2, rp.x + size / 2, rp.y + size / 2);
                }
            }
        }
    }

    //glColor4f(255, 255, 136, 255);
    painter.setBrush(QColor(255, 255, 136, 255));
    for (int i = 0; i < m_seeds.size(); ++i) {
        raster_point rp =rasterizePoint(m_seeds[i].p.x, m_seeds[i].p.y, m_seeds[i].p.z);
        //if (rp.d !=  -1) {
        if (!IS_EQUAL(rp.d, -1)) {
            size = 200 / (1 + rp.d);
            //glRectf(rp.x - size / 2, rp.y - size / 2, rp.x + size / 2, rp.y + size / 2);
             painter.drawRect(rp.x - size / 2, rp.y - size / 2, size, size);
        }
    }

    point point1;
    color col;

    QPen pen1;
    pen1.setCapStyle(Qt::RoundCap);
    pen1.setStyle(Qt::SolidLine);
    pen1.setJoinStyle(Qt::RoundJoin);

    for (int i = 0; i < m_sparks.size(); ++i) {
        raster_point rp =rasterizePoint(m_sparks[i].p.x, m_sparks[i].p.y, m_sparks[i].p.z);
        //if (rp.d !=  -1) {
		if (!IS_EQUAL(rp.d, -1)) {
            size = m_sparks[i].radius * 200 / (1 + rp.d);
            //if (m_sparks[i].alpha < 0)
			if (IS_GREATER_THAN(0, m_sparks[i].alpha))
                m_sparks[i].alpha = 0;
            if (!m_sparks[i].trail.empty()) {
                point1.x = rp.x;
                point1.y = rp.y;
				if (m_sparks[i].col_index >= m_col.size()) {
#ifdef _DEBUG || DEBUG
					qDebug() << "m_sparks[" << i << "].col_index: " << m_sparks[i].col_index << endl;
#endif
				}
				else {
					col = m_col[m_sparks[i].col_index];
				}
                for (int j = m_sparks[i].trail.size() - 1; j >=0; --j) {
                    raster_point point2 = rasterizePoint(m_sparks[i].trail[j].x, m_sparks[i].trail[j].y, m_sparks[i].trail[j].z);
                    //if (point2.d != -1) {
					if (!IS_EQUAL(point2.d, -1)) {
						//a = j / m_sparks[i].trail.size() * 255 * m_sparks[i].alpha / 2;
						a = 255 * j / m_sparks[i].trail.size() * m_sparks[i].alpha / 2;
                        pen1.setColor(QColor(col.r, col.g, col.b, a));
						int width = 1 + m_sparks[i].radius * 10 / (1 + point2.d) / (m_sparks[i].trail.size() - j);
						pen1.setWidth(width);
                        painter.setPen(pen1);
                        painter.drawLine(point1.x, point1.y, point2.x, point2.y);
                    }
                }

            }
            painter.setBrush(QColor(col.r, col.g, col.b, m_sparks[i].alpha * 255));
            painter.drawRect(rp.x - size / 32, rp.y - size / 32, size / 16, size / 16);
        }
    }
    // [*] 恢复 QPainter 的状态
	painter.setPen(pen);
	painter.setBrush(brush);
    painter.setFont(font);   
    //glFlush();
}

void fireworks_impl::frame()
{
    if (m_frames > 100000) {
        m_seedTimer = 0;
        m_frames = 0;
    }
    m_frames++;
}

void fireworks_impl::setYaw(double yaw)
{
	if (m_automove)
		return;
	m_yaw = yaw;
}

void fireworks_impl::setPitch(double pitch)
{
	if (m_automove)
		return;
	m_pitch = pitch;
}

void fireworks_impl::setPlayer(point p)
{
	if (m_automove)
		return;
	m_player = p;
}

void fireworks_impl::set_automove(bool b)
{
	static point p = m_player;
	static double yaw = m_yaw;
	static double pitch = m_pitch;

	if (b) {
		m_player = p;
		m_yaw = yaw;
		m_pitch = pitch;
	}
	else {
		p = m_player;
		yaw = m_yaw;
		m_pitch = pitch;
	}
	
	m_automove = b;
}
