#include "myBrew.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalSpy>
#include <QString>
#include <QtTest>

class myBrewTests : public QObject
{
	Q_OBJECT

private slots:
	void init();

    void TestMashTimer();
	void TestFocusUsage();

	void TestSignals();

private:
	myBrew myBrewInstance;
};

void myBrewTests::init()
{

}

void myBrewTests::TestMashTimer()
{
    QApplication::setActiveWindow(&myBrewInstance);

    myBrewInstance.mash_timer->start();

    QVERIFY2(myBrewInstance.mash_timer->isActive(), "timer isn't active");

    myBrewInstance.on_PB_MLT_MashTimer_Stop_clicked();

    QVERIFY2(!myBrewInstance.mash_timer->isActive(), "timer didn't stop");
}

void myBrewTests::TestFocusUsage()
{

}

void myBrewTests::TestSignals()
{

}

QTEST_MAIN(myBrewTests)

#include "myBrewTests.moc"
