
#ifndef CAMERA_MANIP_H
#define CAMERA_MANIP_H

#include <qevent.h>
#include <qvector2d.h>

#include "Camera.h"


class Camera_manip
{
public:
  Camera_manip(Camera& camera);
  virtual ~Camera_manip() {};

  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void resizeGL(int w, int h);

protected:
  void set_mouse_button_pressed_flag(QMouseEvent* e, bool flag);

  virtual void mouse_press_event(QMouseEvent* e) {}
  virtual void mouse_move_event(QMouseEvent* e) {}
  virtual void mouse_release_event(QMouseEvent* e) {}
  virtual void resize(int w, int h) {}


  Camera& m_camera;

  bool m_left_mouse_button_down = false;
  bool m_middle_mouse_button_down = false;
  QVector2D m_current_mouse_pos;
  QVector2D m_last_mouse_pos;
  QVector2D m_mouse_press_pos;
  QVector2D m_diff;
};


#endif
