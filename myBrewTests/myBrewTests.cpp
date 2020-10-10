#include "myBrew.h"
#include "ui_myBrew.h"

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
    void TestVorlaufTimer();
    void TestClockUpdate();
    void TestChecklist();

private:
	myBrew myBrewInstance;
};

void myBrewTests::init()
{

}

void myBrewTests::TestMashTimer()
{
    QApplication::setActiveWindow(&myBrewInstance);

    myBrewInstance.on_PB_MLT_MashTimer_Start_clicked();

    QVERIFY2(myBrewInstance.mash_timer->isActive(), "timer isn't active");

    myBrewInstance.on_PB_MLT_MashTimer_Stop_clicked();

    QVERIFY2(!myBrewInstance.mash_timer->isActive(), "timer didn't stop");
}

void myBrewTests::TestVorlaufTimer()
{
    QApplication::setActiveWindow(&myBrewInstance);

    myBrewInstance.on_PB_MLT_VorlaufTimer_Start_clicked();

    QVERIFY2(myBrewInstance.vorlauf_timer->isActive(), "timer isn't active");

    myBrewInstance.on_PB_MLT_VorlaufTimer_Stop_clicked();

    QVERIFY2(!myBrewInstance.mash_timer->isActive(), "timer didn't stop");
}

void myBrewTests::TestClockUpdate()
{
    QString first_timestamp = myBrewInstance.ui->clock->text();
    myBrewInstance.updateClockDisplay();
    QVERIFY2(myBrewInstance.ui->clock->text() != first_timestamp, "clock text didn't change!");
}

void myBrewTests::TestChecklist()
{
    QCOMPARE(myBrewInstance.ui->L_Checklist_StartTime->text(), QString("<Time>"));
    myBrewInstance.on_PB_Checklist_Start_clicked();
    QVERIFY2(myBrewInstance.ui->L_Checklist_StartTime->text() != "<Time>", "Checklist start label didn't update");
}

QTEST_MAIN(myBrewTests)

#include "myBrewTests.moc"
