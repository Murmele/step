/* This file is part of Step.
   Copyright (C) 2007 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   Step is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Step is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Step; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef STEP_WORLDSCENE_H
#define STEP_WORLDSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHash>

class WorldModel;
class ItemCreator;
class QModelIndex;
class QGraphicsItem;
class QItemSelection;
class QAction;

namespace StepCore {
    class Item;
}

class WorldScene: public QGraphicsScene
{
    Q_OBJECT

public:
    WorldScene(WorldModel* worldModel, QObject* parent = 0);
    ~WorldScene();

    StepCore::Item* itemFromGraphics(QGraphicsItem* graphicsItem);
    QGraphicsItem* graphicsFromItem(QObject* item);

    void updateViewScale(); // Qt4.3 can help here
    double currentViewScale() { return _currentViewScale; }

    QRectF calcItemsBoundingRect() { return itemsBoundingRect(); } // XXX: exclude axes

public slots:
    void beginAddItem(const QString& name);

signals:
    void endAddItem(const QString& name, bool success);

protected slots:
    void worldModelReset();
    void worldCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
    void worldSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void worldDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    
    void worldRowsInserted(const QModelIndex& parent, int start, int end);
    void worldRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

protected:
    bool event(QEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void keyPressEvent(QKeyEvent* keyEvent);

protected:
    WorldModel* _worldModel;
    QHash<QObject*, QGraphicsItem*> _itemsHash;
    ItemCreator* _itemCreator;
    double _currentViewScale;
};

class WorldGraphicsView: public QGraphicsView
{
    Q_OBJECT

public:
    WorldGraphicsView(WorldScene* worldScene, QWidget* parent);

public slots:
    void zoomIn();
    void zoomOut();
    void fitToPage();
    void actualSize();

protected:
    static const int SCENE_LENGTH = 1000;
};

#endif

