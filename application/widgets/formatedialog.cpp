/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file formatedialog.cpp
 *
 * @brief 格式化窗口类
 *
 * @date 2020-09-10 17:22
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "formatedialog.h"
#include "partedproxy/dmdbushandler.h"

#include <DFontSizeManager>
#include <DWindowCloseButton>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QTimer>

FormateDialog::FormateDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void FormateDialog::initUi()
{
    setFixedSize(450, 355);

    m_curWipeMethod = WipeType::Fast;
    int fileSystemType = 11;
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::Partition) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        m_pathInfo = info.m_path;
        fileSystemType = info.m_fileSystemType;

        DeviceInfo diskInfo = DMDbusHandler::instance()->getCurDeviceInfo();
        m_curDiskMediaType = diskInfo.m_mediaType;
    } else {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        m_pathInfo = info.m_path;
        m_curDiskMediaType = info.m_mediaType;
    }

    DPalette palette1;
    QColor color("#000000");
    color.setAlphaF(0.7);
    palette1.setColor(DPalette::ToolTipText, color);

    DPalette palette2;
    palette2.setColor(DPalette::Text, QColor("#526a7f"));

    DPalette palette3;
    palette3.setColor(DPalette::TextTitle, QColor("#001a2e"));

    DPalette palette4;
    QColor color4("#000000");
    color4.setAlphaF(0.9);
    palette4.setColor(DPalette::ToolTipText, color4);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    m_titleLabel = new DLabel(tr("Wipe %1").arg(m_pathInfo), this);
    QFont fontTitle/* = DFontSizeManager::instance()->get(DFontSizeManager::T6)*/;
    fontTitle.setWeight(QFont::Medium);
    fontTitle.setFamily("Source Han Sans");
    fontTitle.setPixelSize(14);
    m_titleLabel->setFont(fontTitle);
    m_titleLabel->setPalette(palette4);
    mainLayout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
    DLabel *tipLabel = new DLabel(tr("It will erase all data on this disk, which will not be recovered"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setFixedHeight(50);
    tipLabel->setAlignment(Qt::AlignCenter);
    QFont fontTip/* = DFontSizeManager::instance()->get(DFontSizeManager::T8)*/;
    fontTip.setWeight(QFont::Normal);
    fontTip.setFamily("Source Han Sans");
    fontTip.setPixelSize(12);
    tipLabel->setFont(fontTip);
    tipLabel->setPalette(palette1);

    DLabel *fileName = new DLabel(tr("Name:"), this);
    fileName->setFont(fontTip);
    fileName->setPalette(palette3);
    fileName->setFixedHeight(36);
    m_fileNameEdit = new DLineEdit(this);
    m_fileNameEdit->setAccessibleName("partName");
    QRegExp re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegExpValidator *validator = new QRegExpValidator(re, this);
    m_fileNameEdit->lineEdit()->setValidator(validator );
    m_fileNameEdit->setFixedHeight(36);
    if (m_fileNameEdit->text().isEmpty())
        m_fileNameEdit->lineEdit()->setPlaceholderText(tr("Name"));

    DLabel *formatName = new DLabel(tr("File system:"), this);
    formatName->setFont(fontTip);
    formatName->setPalette(palette3);
    formatName->setFixedHeight(36);
    m_formatComboBox = new DComboBox(this);
    m_formatComboBox->setAccessibleName("File system");
    m_formatComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QStringList fslist = DMDbusHandler::instance()->getAllSupportFileSystem();
    fslist.removeOne("linux-swap");
    m_formatComboBox->addItems(fslist);
    m_formatComboBox->setFixedHeight(36);

    if (-1 == fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(fileSystemType)))) {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(11))));
    } else {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(fileSystemType))));
    }

    DLabel *securityLabel = new DLabel(tr("Security:"), this);
    securityLabel->setFont(fontTip);
    securityLabel->setPalette(palette3);
    securityLabel->setFixedHeight(36);
    m_labelTmp = new DLabel(this);
    m_label = new DLabel(this);
    m_label->setFixedHeight(36);
    m_securityComboBox = new DComboBox(this);
    m_securityComboBox->setAccessibleName("Security");
    m_securityComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    if ("SSD" == m_curDiskMediaType) {
        QStringList securitylist;
        securitylist << tr("Fast") << tr("Secure");
        m_securityComboBox->addItems(securitylist);
    } else {
        QStringList securitylist;
        securitylist << tr("Fast") << tr("Secure") << tr("Advanced");
        m_securityComboBox->addItems(securitylist);
    }
    m_securityComboBox->setFixedHeight(36);

    m_describeInfo = new DLabel(this);
    QFont font/* = DFontSizeManager::instance()->get(DFontSizeManager::T10)*/;
    font.setWeight(QFont::Normal);
    font.setFamily("Source Han Sans");
    font.setPixelSize(10);
    m_describeInfo->setFont(font);
    m_describeInfo->setPalette(palette2);
    m_describeInfo->adjustSize();
    m_describeInfo->setWordWrap(true);
    m_describeInfo->setText(tr("It only deletes the partition info without erasing the files on the disk. "
                               "Disk recovery tools may recover the files at a certain probability."));
    m_describeInfo->setFixedHeight(30);
    m_labelTmp->setFixedHeight(29);

    DLabel *wipingLabel = new DLabel(tr("Wiping method:"), this);
    QFont fontWipingMethod/* = DFontSizeManager::instance()->get(DFontSizeManager::T8)*/;
    fontWipingMethod.setWeight(QFont::Normal);
    fontWipingMethod.setFamily("Source Han Sans");
    fontWipingMethod.setPixelSize(12);
    wipingLabel->setFont(fontWipingMethod);
    wipingLabel->setPalette(palette1);

    m_wipingMethodComboBox = new DComboBox(this);
    m_wipingMethodComboBox->setAccessibleName("Wiping method");
    m_wipingMethodComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QStringList wipingMethodlist;
    wipingMethodlist << tr("DoD 5220.22-M, 7 passes") << tr("Gutmann, 35 passes");
    m_wipingMethodComboBox->addItems(wipingMethodlist);
    m_wipingMethodComboBox->setFixedHeight(36);

    QHBoxLayout *wipingLayout = new QHBoxLayout;
    wipingLayout->addWidget(wipingLabel);
    wipingLayout->addSpacing(10);
    wipingLayout->addWidget(m_wipingMethodComboBox);
    wipingLayout->setSpacing(0);
    wipingLayout->setContentsMargins(0, 5, 0, 0);

    m_wipingMethodWidget = new QWidget(this);
    m_wipingMethodWidget->setLayout(wipingLayout);
    m_label->hide();
    m_wipingMethodWidget->hide();

    QVBoxLayout *layoutName = new QVBoxLayout;
    layoutName->addWidget(fileName);
    layoutName->addSpacing(10);
    layoutName->addWidget(formatName);
    layoutName->addSpacing(10);
    layoutName->addWidget(securityLabel);
    layoutName->addSpacing(5);
    layoutName->addWidget(m_labelTmp);
    layoutName->addSpacing(5);
    layoutName->addWidget(m_label);
    layoutName->setSpacing(0);
    layoutName->setContentsMargins(10, 0, 0, 0);

    QVBoxLayout *layoutFormat = new QVBoxLayout;
    layoutFormat->addWidget(m_fileNameEdit);
    layoutFormat->addSpacing(10);
    layoutFormat->addWidget(m_formatComboBox);
    layoutFormat->addSpacing(10);
    layoutFormat->addWidget(m_securityComboBox);
    layoutFormat->addSpacing(5);
    layoutFormat->addWidget(m_describeInfo);
    layoutFormat->addWidget(m_wipingMethodWidget);
    layoutFormat->setSpacing(0);
    layoutFormat->setContentsMargins(0, 0, 10, 0);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(layoutName);
    layout->addLayout(layoutFormat);
    layout->setContentsMargins(0, 0, 0, 0);

    m_pushButton = new QPushButton(tr("Cancel", "button"), this);
    m_pushButton->setAccessibleName("cancel");
    m_pushButton->setFixedHeight(36);
    m_warningButton = new DWarningButton(this);
    m_warningButton->setText(tr("Wipe", "button"));
    m_warningButton->setAccessibleName("wipeButton");
    m_warningButton->setFixedHeight(36);

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addWidget(m_pushButton);
    m_buttonLayout->addSpacing(10);
    m_buttonLayout->addWidget(m_warningButton);
    m_buttonLayout->setSpacing(0);
    m_buttonLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *tipLayout = new QHBoxLayout;
    tipLayout->addSpacing(50);
    tipLayout->addWidget(tipLabel);
    tipLayout->addSpacing(50);
    tipLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *wipeLayout = new QVBoxLayout;
    wipeLayout->addLayout(tipLayout);
    wipeLayout->addLayout(layout);
    wipeLayout->addLayout(m_buttonLayout);
    wipeLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *wipeWidget = new QWidget(this);
    wipeWidget->setLayout(wipeLayout);

    QWidget *spinnerWidget = new QWidget(this);
    m_spinner = new DSpinner(spinnerWidget);
    m_spinner->setFixedSize(28, 28);
    m_spinner->start();
    m_spinner->hide();

    QVBoxLayout *spinnerLayout = new QVBoxLayout;
    spinnerLayout->addSpacing(80);
    spinnerLayout->addWidget(m_spinner, 0, Qt::AlignCenter);
    spinnerLayout->addStretch();
    spinnerWidget->setLayout(spinnerLayout);

    DLabel *failPicture = new DLabel(this);
    QIcon icon = QIcon::fromTheme("://icons/deepin/builtin/fail.svg");
    failPicture->setPixmap(icon.pixmap(128, 128));

    m_failLabel = new DLabel(this);
    m_failLabel->setObjectName("failreason");
    QFont failFont/* = DFontSizeManager::instance()->get(DFontSizeManager::T8)*/;
    failFont.setWeight(QFont::Normal);
    failFont.setFamily("Source Han Sans");
    failFont.setPixelSize(12);
    m_failLabel->setFont(failFont);
    m_failLabel->setPalette(palette3);
    m_failLabel->setText(tr("Failed to find the disk"));

    QVBoxLayout *failLayout = new QVBoxLayout;
    failLayout->addWidget(failPicture, 0, Qt::AlignCenter);
    failLayout->addSpacing(10);
    failLayout->addWidget(m_failLabel, 0, Qt::AlignCenter);
    failLayout->addStretch();
    failLayout->setSpacing(0);
    failLayout->setContentsMargins(0, 20, 0, 0);

    QWidget *failWidget = new QWidget(this);
    failWidget->setLayout(failLayout);

    m_stackedWidget = new DStackedWidget(this);
    m_stackedWidget->addWidget(wipeWidget);
    m_stackedWidget->addWidget(spinnerWidget);
    m_stackedWidget->addWidget(failWidget);

    mainLayout->addWidget(m_stackedWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void FormateDialog::initConnection()
{
    connect(m_pushButton, &DPushButton::clicked, this, &FormateDialog::onCancelButtonClicked);
    connect(m_warningButton, &DWarningButton::clicked, this, &FormateDialog::onWipeButtonClicked);
    connect(m_fileNameEdit, &DLineEdit::textChanged, this, &FormateDialog::onTextChanged);
    connect(m_formatComboBox, &DComboBox::currentTextChanged, this, &FormateDialog::onComboxFormatTextChange);
    connect(m_securityComboBox, static_cast<void (DComboBox:: *)(const int)>(&DComboBox::currentIndexChanged),
            this, &FormateDialog::onSecurityCurrentIndexChanged);
    connect(m_wipingMethodComboBox, static_cast<void (DComboBox:: *)(const int)>(&DComboBox::currentIndexChanged),
            this, &FormateDialog::onWipingMethodCurrentIndexChanged);
    connect(DMDbusHandler::instance(), &DMDbusHandler::wipeMessage, this, &FormateDialog::onWipeResult);
}

void FormateDialog::onTextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        QByteArray byteArray = text.toUtf8();
        if (m_formatComboBox->currentText() == "fat32") {
            if (byteArray.size() > 11) {
                m_fileNameEdit->setAlert(true);
                m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

                m_warningButton->setEnabled(false);
            } else {
                m_fileNameEdit->setAlert(false);
                m_fileNameEdit->hideAlertMessage();

                m_warningButton->setEnabled(true);
            }
        } else {
            if (byteArray.size() > 16) {
                m_fileNameEdit->setAlert(true);
                m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

                m_warningButton->setEnabled(false);
            } else {
                m_fileNameEdit->setAlert(false);
                m_fileNameEdit->hideAlertMessage();

                m_warningButton->setEnabled(true);
            }
        }
    }
}

void FormateDialog::onComboxFormatTextChange(const QString &text)
{
    QByteArray byteArray = m_fileNameEdit->text().toUtf8();
    if (text == "fat32") {
        if (byteArray.size() > 11) {
            m_fileNameEdit->setAlert(true);
            m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

            m_warningButton->setEnabled(false);
        } else {
            m_fileNameEdit->setAlert(false);
            m_fileNameEdit->hideAlertMessage();

            m_warningButton->setEnabled(true);
        }
    } else {
        if (byteArray.size() > 16) {
            m_fileNameEdit->setAlert(true);
            m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

            m_warningButton->setEnabled(false);
        } else {
            m_fileNameEdit->setAlert(false);
            m_fileNameEdit->hideAlertMessage();

            m_warningButton->setEnabled(true);
        }
    }
}

void FormateDialog::onSecurityCurrentIndexChanged(int index)
{
    switch (index) {
    case 0: {
        setFixedSize(450, 355);
        m_describeInfo->setFixedHeight(30);
        m_labelTmp->setFixedHeight(29);
        m_buttonLayout->setContentsMargins(0, 0, 0, 0);
        m_label->hide();
        m_wipingMethodWidget->hide();
        m_curWipeMethod = WipeType::Fast;
        m_describeInfo->setText(tr("It only deletes the partition info without erasing the files on the disk. "
                                   "Disk recovery tools may recover the files at a certain probability."));
        break;
    }
    case 1: {
        setFixedSize(450, 365);
        m_describeInfo->setFixedHeight(40);
        m_labelTmp->setFixedHeight(40);
        m_buttonLayout->setContentsMargins(0, 0, 0, 0);
        m_label->hide();
        m_wipingMethodWidget->hide();
        m_curWipeMethod = WipeType::Secure;
        m_describeInfo->setText(tr("It is a one-time secure wipe that complies with NIST 800-88 and writes 0, 1, "
                                   "and random data to the entire disk once. You will not be able to recover files, "
                                   "and the process will be slow."));
        break;
    }
    case 2: {
        setFixedSize(450, 412);
        m_describeInfo->setFixedHeight(40);
        m_labelTmp->setFixedHeight(40);
        m_buttonLayout->setContentsMargins(0, 10, 0, 0);
        m_label->show();
        m_wipingMethodWidget->show();
        m_wipingMethodComboBox->setCurrentIndex(0);
        m_curWipeMethod = WipeType::DoD;
        m_describeInfo->setText(tr("It writes 0, 1, and random data to the entire disk several times. You can set the "
                                   "number of times to erase disks and overwrite data, but the process will be very slow."));
        break;
    }
    default:
        break;
    }
}

void FormateDialog::onWipingMethodCurrentIndexChanged(int index)
{
    switch (index) {
    case 0: {
        m_curWipeMethod = WipeType::DoD;
        break;
    }
    case 1: {
        m_curWipeMethod = WipeType::Gutmann;
        break;
    }
    default:
        break;
    }
}

void FormateDialog::onCancelButtonClicked()
{
    close();
}

void FormateDialog::onWipeButtonClicked()
{
    QString userName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    userName.remove(0, 6);

    if (m_fileNameEdit->text().isEmpty()) {
        DMDbusHandler::instance()->clear(m_formatComboBox->currentText(), m_pathInfo, " ",
                                         userName, DMDbusHandler::instance()->getCurLevel(), m_curWipeMethod);
    } else {
        DMDbusHandler::instance()->clear(m_formatComboBox->currentText(), m_pathInfo, m_fileNameEdit->text(),
                                         userName, DMDbusHandler::instance()->getCurLevel(), m_curWipeMethod);
    }

    // 擦除等待动画
    m_titleLabel->setText(tr("Wiping %1").arg(m_pathInfo) + "...");
    m_stackedWidget->setCurrentIndex(1);
    DWindowCloseButton *button = findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    button->setDisabled(true);
    button->hide();
    m_spinner->show();
}

void FormateDialog::onWipeResult(const QString &info)
{
    QStringList infoList = info.split(":");

    if (infoList.count() <= 1) {
        return;
    }

    m_spinner->hide();
    if (infoList.at(0) == "1") {
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                 QIcon::fromTheme("://icons/deepin/builtin/ok.svg"),
                                                 tr("\"%1\" wiped").arg(m_pathInfo));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                       QMargins(0, 0, 0, 20));

        close();
    } else {
        m_titleLabel->setText(tr("Failed to wipe %1").arg(m_pathInfo));
        m_stackedWidget->setCurrentIndex(2);

        DWindowCloseButton *button = findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        button->setDisabled(false);
        button->show();

        switch (infoList.at(1).toInt()) {
        case 1: {
            m_failLabel->setText(tr("Failed to find the disk"));
            break;
        }
        case 2: {
            m_failLabel->setText(tr("Failed to get the partition info"));
            break;
        }
        case 3: {
            m_failLabel->setText(tr("Failed to delete the partition"));
            break;
        }
        case 4: {
            m_failLabel->setText(tr("Failed to submit the request to the kernel"));
            break;
        }
        default:
            break;
        }
    }
}

