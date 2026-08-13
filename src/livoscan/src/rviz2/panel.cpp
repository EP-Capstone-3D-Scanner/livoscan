#include "rviz2/panel.hpp"
#include <pluginlib/class_list_macros.hpp>

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>

namespace my_custom_rviz_panel {

MyCustomPanel::MyCustomPanel(QWidget* parent) : rviz_common::Panel(parent) {
  auto* layout = new QVBoxLayout;

  fullscreen_button_ = new QPushButton("Toggle Fullscreen GUI", this);
  layout->addWidget(fullscreen_button_);
  setLayout(layout);

  connect(fullscreen_button_, &QPushButton::clicked, this, &MyCustomPanel::onFullscreenToggle);
}

MyCustomPanel::~MyCustomPanel() = default;

void MyCustomPanel::showEvent(QShowEvent* event) {
  // Call the base class implementation first
  rviz_common::Panel::showEvent(event);

  // Traverse up the widget tree to find the QDockWidget wrapper
  QWidget* p = this->parentWidget();
  while (p) {
    QDockWidget* dock = qobject_cast<QDockWidget*>(p);
    if (dock) {
      // Keep it movable and floatable, but remove the Closable feature
      dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
      break; // Found it, stop searching
    }
    p = p->parentWidget();
  }
}

void MyCustomPanel::onFullscreenToggle() {
  QMainWindow* main_window = qobject_cast<QMainWindow*>(this->window());

  if (main_window) {
    if (main_window->isFullScreen()) {
      main_window->showNormal();
      if (main_window->menuBar()) main_window->menuBar()->show();
      if (main_window->statusBar()) main_window->statusBar()->show();
    } else {
      if (main_window->menuBar()) main_window->menuBar()->hide();
      if (main_window->statusBar()) main_window->statusBar()->hide();
      main_window->showFullScreen();
    }
  }
}

}  // namespace my_custom_rviz_panel

PLUGINLIB_EXPORT_CLASS(my_custom_rviz_panel::MyCustomPanel, rviz_common::Panel)
