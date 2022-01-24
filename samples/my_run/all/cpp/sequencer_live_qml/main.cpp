/*!
 * \file    main.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-24
 * \since   1.1.6
 *
 * \brief   This application demonstrates how to use the IDS peak API
 *          combined with a QML GUI to configure a camera with sequencer and
 *          display the image sequence.
 *
 * \version 1.0.1
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without
 * notice and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for any errors that may appear in this document.
 *
 * This document, or source code, is provided solely as an example
 * of how to utilize IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#include "backend.h"
#include "entrylistmodel.h"
#include "entrylistobject.h"
#include "imageitem.h"
#include "nodelistmodel.h"
#include "nodelistobject.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setApplicationName("IDS Sequencer Demo");
    QCoreApplication::setOrganizationName("IDS Imaging Development Systems GmbH");


    QGuiApplication app(argc, argv);

    // make the different list models and their objects available in QML
    qRegisterMetaType<NodeListModel*>("NodeListModel *");
    qRegisterMetaType<NodeListObject*>("NodeListObject *");
    qRegisterMetaType<EntryListModel*>("EntryListModel *");
    qRegisterMetaType<EntryListObject*>("EntryListObject *");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    // make the image item available in QML
    qRegisterMetaType<QImage*>("QImage *");
    qmlRegisterType<ImageItem>("Sequencer.ImageItem", 1, 0, "ImageItem");

    // make the buffer type available for signal slots
    qRegisterMetaType<std::shared_ptr<peak::core::Buffer>>();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    // create a backend object and make it available in QML
    BackEnd* backEnd = new BackEnd();
    engine.rootContext()->setContextProperty("backEnd", backEnd);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, backEnd, &QObject::deleteLater);

    engine.load(url);

    return app.exec();
}
