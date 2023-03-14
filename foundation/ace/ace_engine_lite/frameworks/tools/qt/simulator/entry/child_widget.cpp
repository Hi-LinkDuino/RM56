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

#include <child_widget.h>

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <Qt>

#include "ability_manager_interface.h"
#include "simulator_config.h"

ChildWidget::ChildWidget(QWidget *parent, const QString jsBundlePath, const QString jsHeapSize) : QWidget(parent)
{
    InitPage(jsBundlePath, jsHeapSize);
    InitSignalSlots();
}

ChildWidget::~ChildWidget()
{
    mockServices_.QuitAll();
}

void ChildWidget::InitPage(const QString jsBundlePath, const QString jsHeapSize)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->setAlignment(Qt::AlignLeft);
    QLabel *jsBundleLabel = new QLabel();
    uint16_t jsBundleLabelWidth = 110;
    uint16_t height = 25;
    jsBundleLabel->setFixedSize(jsBundleLabelWidth, height);
    jsBundleLabel->setText("JS Bundle:");
    hLayout1->addWidget(jsBundleLabel);
    uint16_t jsBundleTextWidth = 300;
    jsBundleText.setFixedSize(jsBundleTextWidth, height);
    jsBundleText.setText(jsBundlePath);
    hLayout1->addWidget(&jsBundleText);
    uint16_t selectFolderBtnWidth = 70;
    selectFolderBtn.setFixedSize(selectFolderBtnWidth, height);
    selectFolderBtn.setText("...");
    hLayout1->addWidget(&selectFolderBtn);
    vLayout->addLayout(hLayout1);
    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->setAlignment(Qt::AlignLeft);
    QLabel *jsHeapLabel = new QLabel();
    jsHeapLabel->setFixedSize(jsBundleLabelWidth, height);
    jsHeapLabel->setText("Heap Size:");
    hLayout2->addWidget(jsHeapLabel);
    uint16_t jsHeapTextWidth = 100;
    jsHeapText.setFixedSize(jsHeapTextWidth, height);
    jsHeapText.setText(jsHeapSize);
    hLayout2->addWidget(&jsHeapText);
    QLabel *heapSizeLable = new QLabel();
    uint16_t jsHeapLabelWidth = 190;
    heapSizeLable->setFixedSize(jsHeapLabelWidth, height);
    heapSizeLable->setText("Range：48~512 KB");
    hLayout2->addWidget(heapSizeLable);
    configBtn.setFixedSize(selectFolderBtnWidth, height);
    configBtn.setText("Start");
    hLayout2->addWidget(&configBtn);
    vLayout->addLayout(hLayout2);
}

void ChildWidget::InitSignalSlots() const
{
    connect(&selectFolderBtn, SIGNAL(clicked()), this, SLOT(OpenFolderDialog()));
    connect(&configBtn, SIGNAL(clicked()), this, SLOT(RestartApp()));
}

void ChildWidget::OpenFolderDialog()
{
    QString dirpath = QFileDialog::getExistingDirectory(nullptr, "Select Folder", "./", QFileDialog::ShowDirsOnly);
    if (!dirpath.isNull() && !dirpath.isEmpty()) {
        jsBundleText.setText(dirpath);
    }
}

void ChildWidget::RestartApp() const
{
    SimulatorConfig::GetInstance().SetConfigValue(CONFIG_KEY_JSBUNDLE_PATH, jsBundleText.text());
    SimulatorConfig::GetInstance().SetConfigValue(CONFIG_KEY_JSHEAP_SIZE, jsHeapText.text());
    QString program = QApplication::applicationFilePath();
    QStringList arguments = QApplication::arguments();
    QProcess::startDetached(program, arguments, QDir::currentPath());
    QApplication::exit();
}

void ChildWidget::StartApp(const char *path)
{
    if (path == nullptr || (strlen(path) == 0)) {
        return;
    }
    // start all necessary mock threads
    mockServices_.StartAll();
    const uint32_t breakTime = 30;
    QThread::msleep(breakTime);
    StartAbility(path, "com.app.example");
}
