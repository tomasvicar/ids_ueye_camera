/*!
 * \file    imagescene.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
 *
 * \version 1.0.0
 *
 * Copyright (C) 2021, IDS Imaging Development Systems GmbH.
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

#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QString>

class ImageView;

class ImageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ImageScene(ImageView* parent);

private:
    ImageView* m_parent = nullptr;
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;

signals:
    void messageBoxTrigger(QString messageTitle, QString messageText);
};

#endif // IMAGESCENE_H
