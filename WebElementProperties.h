#ifndef WEBELEMENTPROPERTIES_H
#define WEBELEMENTPROPERTIES_H

#include <QWidget>
#include <QJsonObject>
#include <QGraphicsItem>

class WebElementItem;
class QLineEdit;
class QComboBox;
class QTextEdit;

class WebElementProperties : public QWidget
{
    Q_OBJECT

public:
    explicit WebElementProperties(QWidget *parent = nullptr);
    void setGlobalProperties(const QJsonObject &props);
    void setCurrentElement(QGraphicsItem *item);
    void clear();

    QJsonObject getGlobalProperties() const;
    QString getGlobalCss() const;

    QString generateHtml(const QJsonObject &element) const;

    signals:
        void propertiesChanged();

private slots:
    void onPropertyChanged();

private:
    void setupUi();
    void updateForm();

    WebElementItem *m_currentElement;

    QComboBox *m_typeCombo;
    QLineEdit *m_idEdit;
    QLineEdit *m_classEdit;
    QTextEdit *m_textEdit;
    QTextEdit *m_styleEdit;
    QTextEdit *m_globalCssEdit;
};

#endif // WEBELEMENTPROPERTIES_H