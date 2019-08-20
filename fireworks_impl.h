#ifndef FIREWORKS_IMPL_H
#define FIREWORKS_IMPL_H

#include "fireworks.h"
#include <QOpenGLFunctions_3_0>

class fireworks_impl: public QOpenGLFunctions_3_0
{
protected:
    fireworks_impl();
    void initVars(double cx, double cy);
    raster_point rasterizePoint(double x, double y, double z);
    void spawnSeed();
    void splode(double x, double y, double z);
    void doLogic();
    void draw(QPainter &painter);
    void frame();
	double getYaw() { return m_yaw; }
	double getPitch() { return m_pitch; }
	const point& getPlayer() const { return m_player; }
	bool getAutoMove() { return m_automove; }
	void setYaw(double yaw);
	void setPitch(double pitch);
	void setPlayer(point p);
	void set_automove(bool b);

private:
    double m_cx;
    double m_cy;
    point m_player;
    point m_playerV;
    double m_pitch;
    double m_pitchV;
    double m_yaw;
    double m_yawV;
    int m_scale;
    int m_seedTimer;
    int m_seedInterval;
    int m_seedLife;
    double m_gravity;
    int m_frames;
	bool m_automove;

    std::vector<spark> m_sparks;
    std::vector<seed> m_seeds;
    static std::vector<color> m_col;

    friend class fireworks;
};

#endif // FIREWORKS_IMPL_H
