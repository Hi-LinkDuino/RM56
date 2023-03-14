/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "main_widget.h"
#include <QMetaType>

// NOTE: this is the dup of the main widget implementation from graphic, keep this file updated with
// the same file under graphic/ui/tools/qt/simulator/uitest folder.
namespace OHOS {
MainWidget::MainWidget(QWidget* parent) : QWidget(parent), guiThread_(nullptr), taskThread_(nullptr)
{
    ui_->setupUi(this);
    CreateGUIThread();
}

MainWidget::~MainWidget()
{
    delete ui_;
    if (guiThread_ != nullptr) {
        guiThread_->Quit();
        guiThread_->wait();
        delete guiThread_;
    }
    if (taskThread_ != nullptr) {
        taskThread_->Quit();
        taskThread_->wait();
        delete taskThread_;
    }
}

void MainWidget::CreateGUIThread()
{
    guiThread_ = new GUIThread(nullptr);
    if (guiThread_ != nullptr) {
        guiThread_->start();
        qRegisterMetaType<uint32_t>("uint32_t");
        connect(guiThread_->GetMonitor(), SIGNAL(UpdatePaintSignal(uint32_t*, uint32_t, uint32_t)), this,
                SLOT(UpdatePaintSlot(uint32_t*, uint32_t, uint32_t)));
    }
}

void MainWidget::CreateTaskThread()
{
    taskThread_ = new TaskThread();
    if (taskThread_ != nullptr) {
        taskThread_->start();
    }
}

void MainWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.drawImage(0, 0, img_);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event)
{
    MouseInput::GetInstance()->MouseHandler(event);
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{
#if USE_MOUSE != 0
    MouseInput::GetInstance()->MouseHandler(event);
#endif
}

void MainWidget::mouseReleaseEvent(QMouseEvent* event)
{
#if USE_MOUSE != 0
    MouseInput::GetInstance()->MouseHandler(event);
#endif
}
void MainWidget::wheelEvent(QWheelEvent* event)
{
#if USE_MOUSEWHEEL && ENABLE_ROTATE_INPUT
    MousewheelInput::GetInstance()->MousewheelHandler(event);
#endif
}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
#if USE_KEY != 0
    KeyInput::GetInstance()->KeyHandler(event);
#endif
}
void MainWidget::keyReleaseEvent(QKeyEvent* event)
{
#if USE_KEY != 0
    KeyInput::GetInstance()->KeyHandler(event);
#endif
}
} // namespace OHOS
