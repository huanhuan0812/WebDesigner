// mainwindow.cpp
#include "mainwindow.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QFormLayout>
#include <QInputDialog>
#include <QDir>
#include <QTextStream>

WebItem::WebItem(const QString &type, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_type(type), m_rect(0, 0, 100, 50)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    
    // Default properties
    m_properties["id"] = "";
    m_properties["class"] = "";
    
    if (type == "Label") {
        m_properties["text"] = "Label";
    } else if (type == "Button") {
        m_properties["text"] = "Button";
    } else if (type == "Image") {
        m_properties["src"] = "";
        m_properties["alt"] = "Image";
        m_rect.setSize(QSize(100, 100));
    } else if (type == "TextEdit") {
        m_properties["text"] = "Edit me";
        m_rect.setSize(QSize(150, 100));
    } else if (type == "Input") {
        m_properties["type"] = "text";
        m_properties["value"] = "";
        m_rect.setSize(QSize(120, 25));
    } else if (type == "Link") {
        m_properties["href"] = "#";
        m_properties["text"] = "Link";
    }
}

QRectF WebItem::boundingRect() const
{
    return m_rect;
}

void WebItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    if (isSelected()) {
        painter->setPen(QPen(Qt::blue, 2, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(m_rect);
    }
    
    painter->setPen(Qt::black);
    
    if (m_type == "Label") {
        painter->drawText(m_rect, Qt::AlignCenter, m_properties.value("text"));
    } else if (m_type == "Button") {
        painter->setBrush(QColor(240, 240, 240));
        painter->drawRect(m_rect);
        painter->drawText(m_rect, Qt::AlignCenter, m_properties.value("text"));
    } else if (m_type == "Image") {
        painter->setBrush(QColor(200, 200, 255));
        painter->drawRect(m_rect);
        painter->drawText(m_rect, Qt::AlignCenter, "Image: " + m_properties.value("src"));
    } else if (m_type == "TextEdit") {
        painter->setBrush(Qt::white);
        painter->drawRect(m_rect);
        painter->drawText(m_rect.adjusted(5, 5, -5, -5), Qt::AlignLeft | Qt::TextWordWrap, 
                         m_properties.value("text"));
    } else if (m_type == "Input") {
        painter->setBrush(Qt::white);
        painter->drawRect(m_rect);
        painter->drawText(m_rect.adjusted(5, 0, -5, 0), Qt::AlignVCenter, 
                         m_properties.value("value").isEmpty() ? 
                         m_properties.value("type") + " input" : m_properties.value("value"));
    } else if (m_type == "Link") {
        painter->setPen(QPen(Qt::blue));
        painter->drawText(m_rect, Qt::AlignCenter, m_properties.value("text"));
    }
}

QString WebItem::toHtml() const
{
    QString html;
    QString style = QString("position:absolute;left:%1px;top:%2px;width:%3px;height:%4px;")
                   .arg(pos().x()).arg(pos().y())
                   .arg(m_rect.width()).arg(m_rect.height());
    
    if (m_type == "Label") {
        html = QString("<div style='%1'>%2</div>")
               .arg(style, m_properties.value("text"));
    } else if (m_type == "Button") {
        html = QString("<button style='%1'>%2</button>")
               .arg(style, m_properties.value("text"));
    } else if (m_type == "Image") {
        html = QString("<img src='%1' alt='%2' style='%3' />")
               .arg(m_properties.value("src"), m_properties.value("alt"), style);
    } else if (m_type == "TextEdit") {
        html = QString("<textarea style='%1'>%2</textarea>")
               .arg(style, m_properties.value("text"));
    } else if (m_type == "Input") {
        html = QString("<input type='%1' value='%2' style='%3' />")
               .arg(m_properties.value("type"), m_properties.value("value"), style);
    } else if (m_type == "Link") {
        html = QString("<a href='%1' style='%2'>%3</a>")
               .arg(m_properties.value("href"), style, m_properties.value("text"));
    }
    
    return html;
}

void WebItem::setHtmlProperties(const QMap<QString, QString> &props)
{
    m_properties = props;
    update();
}

void WebItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    // In a more complete implementation, this could open an editor for the item
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentItem(nullptr)
{
    setWindowTitle(tr("Qt Web Designer"));
    resize(1200, 800);
    
    createEditor();
    createToolBox();
    createActions();
    createToolBars();
    setupConnections();
    
    newFile();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createEditor()
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    preview = new QWebEngineView();
    
    tabs = new QTabWidget(this);
    tabs->addTab(view, tr("Design"));
    tabs->addTab(preview, tr("Preview"));
    
    setCentralWidget(tabs);
    
    // Create property editor
    propertyDock = new QDockWidget(tr("Properties"), this);
    propertyWidget = new QWidget();
    propertyLayout = new QFormLayout(propertyWidget);
    
    tagCombo = new QComboBox();
    tagCombo->addItems({"div", "span", "section", "article"});
    propertyLayout->addRow(tr("Tag:"), tagCombo);
    
    idEdit = new QLineEdit();
    propertyLayout->addRow(tr("ID:"), idEdit);
    
    classEdit = new QLineEdit();
    propertyLayout->addRow(tr("Class:"), classEdit);
    
    textEdit = new QLineEdit();
    propertyLayout->addRow(tr("Text:"), textEdit);
    
    srcEdit = new QLineEdit();
    propertyLayout->addRow(tr("Source:"), srcEdit);
    
    hrefEdit = new QLineEdit();
    propertyLayout->addRow(tr("Href:"), hrefEdit);
    
    widthSpin = new QSpinBox();
    widthSpin->setRange(10, 2000);
    propertyLayout->addRow(tr("Width:"), widthSpin);
    
    heightSpin = new QSpinBox();
    heightSpin->setRange(10, 2000);
    propertyLayout->addRow(tr("Height:"), heightSpin);
    
    QPushButton *applyButton = new QPushButton(tr("Apply"));
    propertyLayout->addRow(applyButton);
    
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::updateItemProperties);
    
    propertyWidget->setLayout(propertyLayout);
    propertyDock->setWidget(propertyWidget);
    addDockWidget(Qt::RightDockWidgetArea, propertyDock);
    
    // Initially disable property editor
    propertyWidget->setEnabled(false);
}

void MainWindow::createToolBox()
{
    toolBox = new QToolBox();
    QWidget *basicWidget = new QWidget();
    QVBoxLayout *basicLayout = new QVBoxLayout(basicWidget);
    
    QToolButton *labelButton = new QToolButton();
    labelButton->setText(tr("Label"));
    labelButton->setToolTip(tr("Add Label"));
    labelButton->setIcon(QIcon(":/icons/label.png"));
    labelButton->setIconSize(QSize(32, 32));
    labelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(labelButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("Label");
        scene->addItem(item);
    });
    
    QToolButton *buttonButton = new QToolButton();
    buttonButton->setText(tr("Button"));
    buttonButton->setToolTip(tr("Add Button"));
    buttonButton->setIcon(QIcon(":/icons/button.png"));
    buttonButton->setIconSize(QSize(32, 32));
    buttonButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(buttonButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("Button");
        scene->addItem(item);
    });
    
    QToolButton *imageButton = new QToolButton();
    imageButton->setText(tr("Image"));
    imageButton->setToolTip(tr("Add Image"));
    imageButton->setIcon(QIcon(":/icons/image.png"));
    imageButton->setIconSize(QSize(32, 32));
    imageButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(imageButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("Image");
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), "", 
                                                      tr("Image Files (*.png *.jpg *.jpeg *.gif);;All Files (*)"));
        if (!fileName.isEmpty()) {
            QMap<QString, QString> props = item->properties();
            props["src"] = fileName;
            item->setHtmlProperties(props);
        }
        scene->addItem(item);
    });
    
    QToolButton *inputButton = new QToolButton();
    inputButton->setText(tr("Input"));
    inputButton->setToolTip(tr("Add Input"));
    inputButton->setIcon(QIcon(":/icons/input.png"));
    inputButton->setIconSize(QSize(32, 32));
    inputButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(inputButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("Input");
        scene->addItem(item);
    });
    
    QToolButton *textEditButton = new QToolButton();
    textEditButton->setText(tr("Text Area"));
    textEditButton->setToolTip(tr("Add Text Area"));
    textEditButton->setIcon(QIcon(":/icons/textedit.png"));
    textEditButton->setIconSize(QSize(32, 32));
    textEditButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(textEditButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("TextEdit");
        scene->addItem(item);
    });
    
    QToolButton *linkButton = new QToolButton();
    linkButton->setText(tr("Link"));
    linkButton->setToolTip(tr("Add Link"));
    linkButton->setIcon(QIcon(":/icons/link.png"));
    linkButton->setIconSize(QSize(32, 32));
    linkButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(linkButton, &QToolButton::clicked, [this]() {
        WebItem *item = new WebItem("Link");
        scene->addItem(item);
    });
    
    basicLayout->addWidget(labelButton);
    basicLayout->addWidget(buttonButton);
    basicLayout->addWidget(imageButton);
    basicLayout->addWidget(inputButton);
    basicLayout->addWidget(textEditButton);
    basicLayout->addWidget(linkButton);
    basicLayout->addStretch();
    
    basicWidget->setLayout(basicLayout);
    toolBox->addItem(basicWidget, tr("Basic"));
    
    QDockWidget *toolDock = new QDockWidget(tr("Toolbox"), this);
    toolDock->setWidget(toolBox);
    addDockWidget(Qt::LeftDockWidgetArea, toolDock);
}

void MainWindow::createActions()
{
    // File actions
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(QKeySequence::New);
    
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    
    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(QKeySequence::Save);
    
    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    
    // Edit actions
    deleteAct = new QAction(tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence::Delete);
    
    // View actions
    sourceViewAct = new QAction(tr("&Source View"), this);
    sourceViewAct->setCheckable(true);
}

void MainWindow::createToolBars()
{
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);
    
    QToolBar *editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAct);
    
    QToolBar *viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(sourceViewAct);
}

void MainWindow::setupConnections()
{
    // File actions
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    
    // Edit actions
    connect(deleteAct, &QAction::triggered, [this]() {
        if (currentItem) {
            scene->removeItem(currentItem);
            delete currentItem;
            currentItem = nullptr;
            propertyWidget->setEnabled(false);
            updatePreview();
        }
    });
    
    // View actions
    connect(sourceViewAct, &QAction::toggled, this, &MainWindow::toggleSourceView);
    
    // Scene connections
    connect(scene, &QGraphicsScene::selectionChanged, [this]() {
        QList<QGraphicsItem *> items = scene->selectedItems();
        if (items.isEmpty()) {
            currentItem = nullptr;
            propertyWidget->setEnabled(false);
        } else {
            currentItem = dynamic_cast<WebItem *>(items.first());
            if (currentItem) {
                propertyWidget->setEnabled(true);
                itemSelected(currentItem);
            }
        }
    });
}

void MainWindow::newFile()
{
    scene->clear();
    currentFile.clear();
    setWindowTitle(tr("Qt Web Designer"));
    updatePreview();
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Web Design File"), "", 
                                                   tr("Web Design Files (*.wd);;All Files (*)"));
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

void MainWindow::saveFile()
{
    if (currentFile.isEmpty()) {
        saveAsFile();
    } else {
        saveToFile(currentFile);
    }
}

void MainWindow::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Web Design File"), "", 
                                                   tr("Web Design Files (*.wd);;All Files (*)"));
    if (!fileName.isEmpty()) {
        saveToFile(fileName);
        currentFile = fileName;
        setWindowTitle(tr("%1 - Qt Web Designer").arg(currentFile));
    }
}

void MainWindow::itemSelected(QGraphicsItem *item)
{
    WebItem *webItem = dynamic_cast<WebItem *>(item);
    if (!webItem) return;
    
    currentItem = webItem;
    QMap<QString, QString> props = webItem->properties();
    
    tagCombo->setCurrentText("div"); // Default for now
    idEdit->setText(props.value("id"));
    classEdit->setText(props.value("class"));
    textEdit->setText(props.value("text"));
    srcEdit->setText(props.value("src"));
    hrefEdit->setText(props.value("href"));
    widthSpin->setValue(webItem->boundingRect().width());
    heightSpin->setValue(webItem->boundingRect().height());
    
    // Enable/disable relevant fields based on item type
    QString type = webItem->type();
    textEdit->setEnabled(type == "Label" || type == "Button" || type == "Link" || type == "TextEdit");
    srcEdit->setEnabled(type == "Image");
    hrefEdit->setEnabled(type == "Link");
}

void MainWindow::updateItemProperties()
{
    if (!currentItem) return;
    
    QMap<QString, QString> props = currentItem->properties();
    
    props["id"] = idEdit->text();
    props["class"] = classEdit->text();
    
    if (textEdit->isEnabled()) {
        props["text"] = textEdit->text();
    }
    
    if (srcEdit->isEnabled()) {
        props["src"] = srcEdit->text();
    }
    
    if (hrefEdit->isEnabled()) {
        props["href"] = hrefEdit->text();
    }
    
    currentItem->setHtmlProperties(props);
    
    // Update size
    QRectF rect = currentItem->boundingRect();
    rect.setWidth(widthSpin->value());
    rect.setHeight(heightSpin->value());
    currentItem->prepareGeometryChange();
    currentItem->setData(0, rect); // Store new rect
    
    updatePreview();
}

void MainWindow::toggleSourceView(bool checked)
{
    if (checked) {
        // Show HTML source
        preview->setHtml(generateHtml());
        tabs->setTabText(1, tr("HTML Source"));
    } else {
        // Show rendered preview
        updatePreview();
        tabs->setTabText(1, tr("Preview"));
    }
}

void MainWindow::updatePreview()
{
    if (!sourceViewAct->isChecked()) {
        QString html = "<!DOCTYPE html><html><head><title>Preview</title></head><body>";
        html += generateHtml();
        html += "</body></html>";
        preview->setHtml(html);
    } else {
        preview->setHtml(generateHtml());
    }
}

QString MainWindow::generateHtml() const
{
    QString html;
    if (sourceViewAct->isChecked()) {
        html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Generated Page</title>\n</head>\n<body>\n";
        
        foreach (QGraphicsItem *item, scene->items()) {
            WebItem *webItem = dynamic_cast<WebItem *>(item);
            if (webItem) {
                html += webItem->toHtml() + "\n";
            }
        }
        
        html += "</body>\n</html>";
    } else {
        foreach (QGraphicsItem *item, scene->items()) {
            WebItem *webItem = dynamic_cast<WebItem *>(item);
            if (webItem) {
                html += webItem->toHtml();
            }
        }
    }
    
    return html;
}

void MainWindow::loadFile(const QString &fileName)
{
    // In a complete implementation, this would load the saved design
    QMessageBox::information(this, tr("Not Implemented"), 
                            tr("Loading design files is not implemented in this example."));
}

void MainWindow::saveToFile(const QString &fileName)
{
    // In a complete implementation, this would save the design
    QMessageBox::information(this, tr("Not Implemented"), 
                            tr("Saving design files is not implemented in this example."));
}