#include "rviz2/panel.hpp"
#include <pluginlib/class_list_macros.hpp>

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTimer>

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
  rviz_common::Panel::showEvent(event);

  if (is_first_show_) {
    is_first_show_ = false; 
    
    // Increased delay to 500ms to ensure RViz is completely finished loading its layout
    QTimer::singleShot(100, this, [this]() {
      
      // 1. Fullscreen Logic
      QMainWindow* main_window = qobject_cast<QMainWindow*>(this->window());
      if (main_window && !main_window->isFullScreen()) {
        if (main_window->menuBar()) main_window->menuBar()->hide();
        if (main_window->statusBar()) main_window->statusBar()->hide();
        main_window->showFullScreen();
      }

      // 2. Remove the Close (X) Button from the Dock Widget
      QDockWidget* my_dock = nullptr;
      QWidget* p = this->parentWidget();
      
      while (p) {
        my_dock = qobject_cast<QDockWidget*>(p);
        if (my_dock) {
          // Remove the Closable feature so the X button disappears
          my_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
          
          // NEW FAILSAFE: Install an event filter directly on the dock widget
          my_dock->installEventFilter(this);
          
          break; // Found it, stop searching
        }
        p = p->parentWidget();
      }
    });
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

bool MyCustomPanel::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::Close) {
    event->ignore();
    return true; // Swallow the event entirely so the panel refuses to close
  }
  
  // Pass all other normal events through
  return rviz_common::Panel::eventFilter(obj, event);
}

}  // namespace my_custom_rviz_panel

PLUGINLIB_EXPORT_CLASS(my_custom_rviz_panel::MyCustomPanel, rviz_common::Panel)
