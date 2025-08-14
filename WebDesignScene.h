#ifndef WEBDESIGNSCENE_H
#define WEBDESIGNSCENE_H

#include <QGraphicsScene>
#include <QJsonArray>

class WebElementItem;

class WebDesignScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit WebDesignScene(QObject *parent = nullptr);

    void clear();
    QJsonObject toJson() const;
    void fromJson(const QJsonArray &elements);

    signals:
        void elementSelected(QGraphicsItem *item);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    WebElementItem* createElement(const QString &type, const QPointF &pos);
};

#endif // WEBDESIGNSCENE_H