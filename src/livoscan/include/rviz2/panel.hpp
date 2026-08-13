#ifndef MY_PANEL_HPP
#define MY_PANEL_HPP

#ifndef Q_MOC_RUN
#include <rviz_common/panel.hpp>
#include <QPushButton>
#include <QVBoxLayout>
#include <QShowEvent>
#endif

namespace my_custom_rviz_panel {

class MyCustomPanel : public rviz_common::Panel {
  Q_OBJECT
public:
  MyCustomPanel(QWidget* parent = nullptr);
  ~MyCustomPanel() override;

protected:
    void showEvent(QShowEvent* event) override;

private Q_SLOTS:
  void onFullscreenToggle();

private:
  QPushButton* fullscreen_button_;
};

}  // namespace my_custom_rviz_panel
#endif  // MY_PANEL_HPP
