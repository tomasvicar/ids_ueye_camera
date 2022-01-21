/*!
 * \file    main.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-24
 * \since   1.1.6
 *
 * \brief   This application demonstrates how to use chunks
 *          combined with a QML GUI.
 *
 * \version 1.0.1
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "backend.h"
#include "imageitem.h"


int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
#endif

    QGuiApplication app(argc, argv);

    qRegisterMetaType<QImage*>("QImage *");
    qmlRegisterType<BackEnd>("BackEnd", 1, 0, "BackEnd");
    qmlRegisterType<ImageItem>("ImageItem", 1, 0, "ImageItem");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
