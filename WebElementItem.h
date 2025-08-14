#ifndef WEBELEMENTITEM_H
#define WEBELEMENTITEM_H

#include <QGraphicsRectItem>
#include <QJsonObject>

class WebElementItem : public QGraphicsRectItem
{
public:
    WebElementItem(const QString &type, QGraphicsItem *parent = nullptr);

    QString elementType() const { return m_type; }
    QString elementId() const { return m_id; }
    QString elementClass() const { return m_class; }
    QString elementText() const { return m_text; }
    QString elementStyle() const { return m_style; }

    void setId(const QString &id) { m_id = id; updateDisplay(); }
    void setClass(const QString &cls) { m_class = cls; updateDisplay(); }
    void setText(const QString &text) { m_text = text; updateDisplay(); }
    void setStyle(const QString &style) { m_style = style; updateDisplay(); }

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateDisplay();
    QColor typeToColor(const QString &type) const;

    QString m_type;
    QString m_id;
    QString m_class;
    QString m_text;
    QString m_style;
    QGraphicsTextItem *m_textItem;
};

#endif // WEBELEMENTITEM_H