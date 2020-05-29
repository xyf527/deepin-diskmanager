/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangkai <zhangkai@uniontech.com>
* Maintainer: zhangkai <zhangkai@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PARTITIONDIALOG_H
#define PARTITIONDIALOG_H

#include <QWidget>
#include "customcontrol/ddbase.h"

DWIDGET_USE_NAMESPACE
class PartitionDialog : public DDBase
{
    Q_OBJECT
public:
    explicit PartitionDialog(QWidget *parent = nullptr);
    void initUi();
    void initConnection();
signals:
    void showPartWidget();
public slots:
    void sendSignal(int index);
};

#endif // PARTITIONDIALOG_H
