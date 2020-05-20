#include "ddbase.h"
#include "widgets/widgetdeclare.h"


DDBase::DDBase(QWidget *parent) : DDialog(parent)
{
    setModal(true);
    setIcon(QIcon::fromTheme(app_name));
    mainFrame = new DFrame(this);
    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);
    addContent(mainFrame);
    // updateGeometry();
}
