/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2006 Olivier Goffart <ogoffart at kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kconfigskeletontest.h"

#include <kconfig.h>
#include <QFont>
#include <QtTestGui>

QTEST_MAIN(KConfigSkeletonTest)

#define DEFAULT_SETTING1 false
#define DEFAULT_SETTING2 QColor(1,2,3)
#define DEFAULT_SETTING3 QFont("helvetica",12)
#define DEFAULT_SETTING4 QString("Hello World")

#define WRITE_SETTING1 true
#define WRITE_SETTING2 QColor(3,2,1)
#define WRITE_SETTING3 QFont("helvetica",14)
#define WRITE_SETTING4 QString("KDE")

void KConfigSkeletonTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void KConfigSkeletonTest::init()
{
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/kconfigskeletontestrc");
    s = new KConfigSkeleton(QStringLiteral("kconfigskeletontestrc"));
    s->setCurrentGroup(QStringLiteral("MyGroup"));
    itemBool = s->addItemBool(QStringLiteral("MySetting1"), mMyBool, DEFAULT_SETTING1);
    s->addItemColor(QStringLiteral("MySetting2"), mMyColor, DEFAULT_SETTING2);

    s->setCurrentGroup(QStringLiteral("MyOtherGroup"));
    s->addItemFont(QStringLiteral("MySetting3"), mMyFont, DEFAULT_SETTING3);
    s->addItemString(QStringLiteral("MySetting4"), mMyString, DEFAULT_SETTING4);

    QCOMPARE(mMyBool, DEFAULT_SETTING1);
    QCOMPARE(mMyColor, DEFAULT_SETTING2);
    QCOMPARE(mMyFont, DEFAULT_SETTING3);
    QCOMPARE(mMyString, DEFAULT_SETTING4);

    QVERIFY(s->isDefaults());
    QVERIFY(!s->isSaveNeeded());
}

void KConfigSkeletonTest::cleanup()
{
    delete s;
}

void KConfigSkeletonTest::testSimple()
{
    mMyBool = WRITE_SETTING1;
    mMyColor = WRITE_SETTING2;
    mMyFont = WRITE_SETTING3;
    mMyString = WRITE_SETTING4;

    QVERIFY(s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    s->save();

    QVERIFY(!s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    mMyBool = false;
    mMyColor = QColor();
    mMyString.clear();
    mMyFont = QFont();

    QVERIFY(s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    s->read();

    QVERIFY(!s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    QCOMPARE(mMyBool, WRITE_SETTING1);
    QCOMPARE(mMyColor, WRITE_SETTING2);
    QCOMPARE(mMyFont, WRITE_SETTING3);
    QCOMPARE(mMyString, WRITE_SETTING4);
}

void KConfigSkeletonTest::testRemoveItem()
{
    QVERIFY(s->findItem("MySetting1"));
    s->removeItem(QStringLiteral("MySetting1"));
    QVERIFY(!s->findItem("MySetting1"));
}

void KConfigSkeletonTest::testClear()
{
    QVERIFY(s->findItem("MySetting2"));
    QVERIFY(s->findItem("MySetting3"));
    QVERIFY(s->findItem("MySetting4"));

    s->clearItems();

    QVERIFY(!s->findItem("MySetting2"));
    QVERIFY(!s->findItem("MySetting3"));
    QVERIFY(!s->findItem("MySetting4"));
}

void KConfigSkeletonTest::testDefaults()
{
    mMyBool = WRITE_SETTING1;
    mMyColor = WRITE_SETTING2;
    mMyFont = WRITE_SETTING3;
    mMyString = WRITE_SETTING4;

    QVERIFY(s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    s->save();

    QVERIFY(!s->isSaveNeeded());
    QVERIFY(!s->isDefaults());

    s->setDefaults();

    QVERIFY(s->isSaveNeeded());
    QVERIFY(s->isDefaults());

    QCOMPARE(mMyBool, DEFAULT_SETTING1);
    QCOMPARE(mMyColor, DEFAULT_SETTING2);
    QCOMPARE(mMyFont, DEFAULT_SETTING3);
    QCOMPARE(mMyString, DEFAULT_SETTING4);

    s->save();

    QVERIFY(!s->isSaveNeeded());
    QVERIFY(s->isDefaults());
}

void KConfigSkeletonTest::testKConfigDirty()
{
    itemBool->setValue(true);
    itemBool->writeConfig(s->sharedConfig().data());
    QVERIFY(s->sharedConfig()->isDirty());
    s->save();
    QVERIFY(!s->sharedConfig()->isDirty());

    itemBool->setValue(false);
    itemBool->writeConfig(s->sharedConfig().data());
    QVERIFY(s->sharedConfig()->isDirty());
    s->save();
    QVERIFY(!s->sharedConfig()->isDirty());
}

void KConfigSkeletonTest::testSaveRead()
{
    itemBool->setValue(true);
    s->save();

    itemBool->setValue(false);
    s->save();

    mMyBool = true;

    s->read();
    QCOMPARE(mMyBool, false);
}
