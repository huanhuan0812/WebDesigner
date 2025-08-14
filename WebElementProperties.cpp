#include "WebElementProperties.h"
#include "WebElementItem.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>

WebElementProperties::WebElementProperties(QWidget *parent)
    : QWidget(parent), m_currentElement(nullptr)
{
    setupUi();
}

void WebElementProperties::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Element properties
    QGroupBox *elementGroup = new QGroupBox(tr("Element Properties"));
    QFormLayout *formLayout = new QFormLayout;
    
    m_typeCombo = new QComboBox;
    m_typeCombo->addItems({"div", "section", "article", "h1", "h2", "h3", "p", "button", "img", "a", "ul", "input", "textarea", "form"});
    formLayout->addRow(tr("Tag:"), m_typeCombo);
    
    m_idEdit = new QLineEdit;
    formLayout->addRow(tr("ID:"), m_idEdit);
    
    m_classEdit = new QLineEdit;
    formLayout->addRow(tr("Class:"), m_classEdit);
    
    m_textEdit = new QTextEdit;
    m_textEdit->setMaximumHeight(100);
    formLayout->addRow(tr("Text:"), m_textEdit);
    
    m_styleEdit = new QTextEdit;
    m_styleEdit->setMaximumHeight(100);
    formLayout->addRow(tr("Style:"), m_styleEdit);
    
    elementGroup->setLayout(formLayout);
    layout->addWidget(elementGroup);
    
    // Global CSS
    QGroupBox *cssGroup = new QGroupBox(tr("Global CSS"));
    QVBoxLayout *cssLayout = new QVBoxLayout;
    
    m_globalCssEdit = new QTextEdit;
    m_globalCssEdit->setPlaceholderText(tr("Add global CSS rules here..."));
    cssLayout->addWidget(m_globalCssEdit);
    
    cssGroup->setLayout(cssLayout);
    layout->addWidget(cssGroup);
    
    // Connect signals
    connect(m_typeCombo, &QComboBox::currentTextChanged, this, &WebElementProperties::onPropertyChanged);
    connect(m_idEdit, &QLineEdit::textEdited, this, &WebElementProperties::onPropertyChanged);
    connect(m_classEdit, &QLineEdit::textEdited, this, &WebElementProperties::onPropertyChanged);
    connect(m_textEdit, &QTextEdit::textChanged, this, &WebElementProperties::onPropertyChanged);
    connect(m_styleEdit, &QTextEdit::textChanged, this, &WebElementProperties::onPropertyChanged);
    connect(m_globalCssEdit, &QTextEdit::textChanged, this, &WebElementProperties::onPropertyChanged);
}

void WebElementProperties::setCurrentElement(QGraphicsItem *item)
{
    m_currentElement = dynamic_cast<WebElementItem*>(item);
    updateForm();
}

void WebElementProperties::clear()
{
    m_currentElement = nullptr;
    m_typeCombo->setCurrentIndex(0);
    m_idEdit->clear();
    m_classEdit->clear();
    m_textEdit->clear();
    m_styleEdit->clear();
}

void WebElementProperties::updateForm()
{
    if (!m_currentElement) {
        m_typeCombo->setEnabled(false);
        m_idEdit->setEnabled(false);
        m_classEdit->setEnabled(false);
        m_textEdit->setEnabled(false);
        m_styleEdit->setEnabled(false);
        return;
    }
    
    m_typeCombo->setEnabled(true);
    m_idEdit->setEnabled(true);
    m_classEdit->setEnabled(true);
    m_textEdit->setEnabled(true);
    m_styleEdit->setEnabled(true);
    
    // Map element type to HTML tag
    QString type = m_currentElement->elementType();
    QString tag = "div";
    if (type.startsWith("Heading")) tag = "h" + QString(type.back());
    else if (type == "Text") tag = "p";
    else if (type == "Button") tag = "button";
    else if (type == "Image") tag = "img";
    else if (type == "Link") tag = "a";
    else if (type == "List") tag = "ul";
    else if (type == "Input") tag = "input";
    else if (type == "Textarea") tag = "textarea";
    else if (type == "Form") tag = "form";
    
    m_typeCombo->setCurrentText(tag);
    m_idEdit->setText(m_currentElement->elementId());
    m_classEdit->setText(m_currentElement->elementClass());
    m_textEdit->setPlainText(m_currentElement->elementText());
    m_styleEdit->setPlainText(m_currentElement->elementStyle());
}

void WebElementProperties::onPropertyChanged()
{
    if (!m_currentElement) return;
    
    m_currentElement->setId(m_idEdit->text());
    m_currentElement->setClass(m_classEdit->text());
    m_currentElement->setText(m_textEdit->toPlainText());
    m_currentElement->setStyle(m_styleEdit->toPlainText());
    
    emit propertiesChanged();
}

QJsonObject WebElementProperties::getGlobalProperties() const
{
    QJsonObject props;
    props["global_css"] = m_globalCssEdit->toPlainText();
    return props;
}

QString WebElementProperties::getGlobalCss() const
{
    return m_globalCssEdit->toPlainText();
}

QString WebElementProperties::generateHtml(const QJsonObject &element) const
{
    QString tag = element["type"].toString();
    QString id = element["id"].toString();
    QString cls = element["class"].toString();
    QString text = element["text"].toString();
    QString style = element["style"].toString();
    
    QString html = "<" + tag;
    
    if (!id.isEmpty()) html += " id=\"" + id + "\"";
    if (!cls.isEmpty()) html += " class=\"" + cls + "\"";
    if (!style.isEmpty()) html += " style=\"" + style + "\"";
    
    if (tag == "img") {
        html += " src=\"placeholder.png\"";
        html += " alt=\"" + text + "\"";
        html += " />";
    } else if (tag == "input") {
        html += " type=\"text\"";
        html += " value=\"" + text + "\"";
        html += " />";
    } else {
        html += ">";
        html += text;
        html += "</" + tag + ">";
    }
    
    return html;
}

void WebElementProperties::setGlobalProperties(const QJsonObject &props) {
    if (props.contains("global_css")) {
        m_globalCssEdit->setPlainText(props["global_css"].toString());
    }
}