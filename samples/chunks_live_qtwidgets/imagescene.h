/*!
 * \file    imagescene.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.0.0
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

#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QObject>
#include <QString>
#include <QGraphicsScene>

class ImageView;

class ImageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ImageScene(ImageView* parent);
    void setText(QString text);

private:
    ImageView* m_parent = nullptr;
    QGraphicsTextItem* m_text = nullptr;
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;

signals:
    void messageBoxTrigger(QString messageTitle, QString messageText);
};

#endif // IMAGESCENE_H
