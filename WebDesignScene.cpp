#include "WebDesignScene.h"
#include "WebElementItem.h"
#include <QMimeData>
#include <QGraphicsView>
#include <QJsonObject>
#include <QGraphicsSceneEvent>
#include <QMouseEvent>

WebDesignScene::WebDesignScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 1200, 800);
    setBackgroundBrush(QColor(240, 240, 240));
}

void WebDesignScene::clear()
{
    QGraphicsScene::clear();
}

QJsonObject WebDesignScene::toJson() const {
    QJsonObject project;
    QJsonArray elements;
    // 填充 elements 数组...
    project["elements"] = elements;
    return project;
}

void WebDesignScene::fromJson(const QJsonArray &elements)
{
    clear();
    for (const QJsonValue &element : elements) {
        QJsonObject obj = element.toObject();
        WebElementItem *item = createElement(obj["type"].toString(),
                                           QPointF(obj["x"].toDouble(), obj["y"].toDouble()));
        item->fromJson(obj);
    }
}

void WebDesignScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void WebDesignScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->acceptProposedAction();
}

void WebDesignScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (!event->mimeData()->hasText()) {
        event->ignore();
        return;
    }

    QString elementType = event->mimeData()->text();
    QPointF dropPos = event->scenePos();

    createElement(elementType, dropPos);
    event->acceptProposedAction();
}

void WebDesignScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        emit elementSelected(item);
    }
}

WebElementItem* WebDesignScene::createElement(const QString &type, const QPointF &pos)
{
    WebElementItem *item = new WebElementItem(type);
    item->setPos(pos);
    addItem(item);

    // Select the new item
    clearSelection();
    item->setSelected(true);
    emit elementSelected(item);
    
    return item;
}