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
	// enables focus and widget events
    //QApplication::setActiveWindow(&myBrewInstance);

	// set initial focus
    //myBrewInstance.mInputA->setFocus();
    //QVERIFY2(myBrewInstance.mInputA->hasFocus(), "Input A doesn't have focus");

	// write STR1
	//QTest::keyClicks(QApplication::focusWidget(), STR1);

	// move focus to next widget
    //QTest::keyClick(&myBrewInstance, Qt::Key_Tab);
    //QVERIFY2(myBrewInstance.mInputB->hasFocus(), "Input B doesn't have focus");

	// write STR2
	//QTest::keyClicks(QApplication::focusWidget(), STR2);

	// move focus to next widget
    //QTest::keyClick(&myBrewInstance, Qt::Key_Tab);
    //QVERIFY2(myBrewInstance.mButtonConcat->hasFocus(), "Button CONCAT doesn't have focus");

	// press button CONCAT using space
	//QTest::keyClick(QApplication::focusWidget(), Qt::Key_Space);
    //QCOMPARE(myBrewInstance.mLabelRes->text(), STR_RES);

	// move focus to next widget
    //QTest::keyClick(&myBrewInstance, Qt::Key_Tab);
    //QVERIFY2(myBrewInstance.mButtonCancel->hasFocus(), "Button CANCEL doesn't have focus");

	// press button CANCEL using space
	//QTest::keyClick(QApplication::focusWidget(), Qt::Key_Space);
    //QVERIFY2(myBrewInstance.mInputA->text().isEmpty(), "Cancel didn't work on input A");
    //QVERIFY2(myBrewInstance.mInputB->text().isEmpty(), "Cancel didn't work on input B");
    //QVERIFY2(myBrewInstance.mLabelRes->text().isEmpty(), "Cancel didn't work on res label");
}

void myBrewTests::TestSignals()
{
	// set input
    //myBrewInstance.mInputA->setText(STR1);
    //myBrewInstance.mInputB->setText(STR2);

	// create spy objects
    //QSignalSpy spy1(&myBrewInstance, &PanelConcat::DataAvailable);
    //QSignalSpy spy2(&myBrewInstance, &PanelConcat::DataCleared);

	// click button CONCAT
    //QTest::mouseClick(myBrewInstance.mButtonConcat, Qt::LeftButton);

	//QCOMPARE(spy1.count(), 1);
	//QCOMPARE(spy2.count(), 0);

	//QList<QVariant> args = spy1.takeFirst();
	//QCOMPARE(args.at(0).toString(), STR_RES);

	// click button CANCEL
    //QTest::mouseClick(myBrewInstance.mButtonCancel, Qt::LeftButton);

	//QCOMPARE(spy1.count(), 0);
	//QCOMPARE(spy2.count(), 1);

	//args = spy2.takeFirst();
	//QVERIFY2(args.empty(), "DataCleared signal has parameters now?!?");
}

QTEST_MAIN(myBrewTests)

#include "myBrewTests.moc"
