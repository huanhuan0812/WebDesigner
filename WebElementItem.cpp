#include "WebElementItem.h"
#include <QPainter>
#include <QGraphicsScene>

WebElementItem::WebElementItem(const QString &type, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), m_type(type)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // Default properties
    m_id = "";
    m_class = "";
    m_text = type;
    m_style = "";
    
    // Set initial size based on type
    if (type == "Container" || type == "Section" || type == "Article") {
        setRect(0, 0, 300, 200);
    } else if (type.startsWith("Heading")) {
        setRect(0, 0, 400, 60);
    } else if (type == "Text" || type == "Paragraph") {
        setRect(0, 0, 400, 100);
    } else if (type == "Button") {
        setRect(0, 0, 120, 40);
    } else if (type == "Image") {
        setRect(0, 0, 200, 150);
    } else {
        setRect(0, 0, 200, 80);
    }
    
    m_textItem = new QGraphicsTextItem(m_text, this);
    m_textItem->setPos(10, 10);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    
    updateDisplay();
}

QJsonObject WebElementItem::toJson() const
{
    QJsonObject json;
    json["type"] = m_type;
    json["x"] = pos().x();
    json["y"] = pos().y();
    json["width"] = rect().width();
    json["height"] = rect().height();
    json["id"] = m_id;
    json["class"] = m_class;
    json["text"] = m_text;
    json["style"] = m_style;
    return json;
}

void WebElementItem::fromJson(const QJsonObject &json)
{
    setPos(json["x"].toDouble(), json["y"].toDouble());
    setRect(0, 0, json["width"].toDouble(), json["height"].toDouble());
    m_id = json["id"].toString();
    m_class = json["class"].toString();
    m_text = json["text"].toString();
    m_style = json["style"].toString();
    
    updateDisplay();
}

void WebElementItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    
    QColor fillColor = typeToColor(m_type);
    if (isSelected()) {
        fillColor = fillColor.lighter(110);
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
    } else {
        painter->setPen(QPen(Qt::black, 1));
    }
    
    painter->setBrush(fillColor);
    painter->drawRect(rect());
    
    // Draw resize handles if selected
    if (isSelected()) {
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black, 1));
        
        const qreal handleSize = 8.0;
        QRectF rect = this->rect();
        
        // Top-left
        painter->drawRect(QRectF(rect.topLeft(), QSizeF(handleSize, handleSize)));
        // Top-right
        painter->drawRect(QRectF(rect.topRight() - QPointF(handleSize, 0), 
                         QSizeF(handleSize, handleSize)));
        // Bottom-left
        painter->drawRect(QRectF(rect.bottomLeft() - QPointF(0, handleSize), 
                         QSizeF(handleSize, handleSize)));
        // Bottom-right
        painter->drawRect(QRectF(rect.bottomRight() - QPointF(handleSize, handleSize), 
                         QSizeF(handleSize, handleSize)));
    }
}

void WebElementItem::updateDisplay()
{
    m_textItem->setPlainText(m_text);
    update();
}

QColor WebElementItem::typeToColor(const QString &type) const
{
    if (type == "Container") return QColor(230, 230, 250);
    if (type == "Section") return QColor(230, 250, 230);
    if (type == "Article") return QColor(250, 230, 230);
    if (type.startsWith("Heading")) return QColor(255, 200, 200);
    if (type == "Text" || type == "Paragraph") return QColor(200, 230, 255);
    if (type == "Button") return QColor(200, 255, 200);
    if (type == "Image") return QColor(255, 255, 200);
    if (type == "Form") return QColor(220, 220, 220);
    return QColor(240, 240, 240);
}