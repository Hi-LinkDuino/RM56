/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRAPHIC_LITE_MAIN_WIDGET_H
#define GRAPHIC_LITE_MAIN_WIDGET_H

#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>

#include "common/graphic_startup.h"
#include "gui_thread.h"
#include "key_input.h"
#include "monitor.h"
#include "mouse_input.h"
#include "mousewheel_input.h"
#include "task_thread.h"
#include "ui_mainwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

namespace OHOS {
class MainWidget : public QWidget {
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    ~MainWidget();
    void CreateGUIThread();
    void CreateTaskThread();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    Ui::MainWidget* ui_;
    QImage img_;
    uint32_t width_;
    uint32_t height_;
    GUIThread* guiThread_;
    TaskThread* taskThread_;

public slots:
    void UpdatePaintSlot(uint32_t* tftFb, uint32_t imgWidth, uint32_t imgHeight)
    {
        img_ = QImage(imgWidth, imgHeight, QImage::Format_RGB32);
        uint32_t* p = tftFb;
        for (uint32_t i = 0; i < imgHeight; i++) {
            for (uint32_t j = 0; j < imgWidth; j++) {
                img_.setPixel(j, i, *p++);
            }
        }
        update();
    };
};
} // namespace OHOS

#endif // GRAPHIC_LITE_MAIN_WIDGET_H
