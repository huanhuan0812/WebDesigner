#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "WebDesignScene.h"
#include "WebElementProperties.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Qt Web Designer 6.8"));
    resize(1400, 900);

    createToolBar();
    createWidgets();
    createConnections();
    setupElementsList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar(tr("Tools"));

    QAction *saveAction = toolBar->addAction(tr("Save Design"));
    saveAction->setIcon(QIcon::fromTheme("document-save"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveDesign);

    QAction *loadAction = toolBar->addAction(tr("Load Design"));
    loadAction->setIcon(QIcon::fromTheme("document-open"));
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadDesign);

    QAction *exportAction = toolBar->addAction(tr("Export HTML"));
    exportAction->setIcon(QIcon::fromTheme("text-html"));
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportHtml);

    QAction *clearAction = toolBar->addAction(tr("Clear"));
    clearAction->setIcon(QIcon::fromTheme("edit-clear"));
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearCanvas);

    toolBar->addSeparator();

    QAction *aboutAction = toolBar->addAction(tr("About"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::createWidgets()
{
    designScene = new WebDesignScene(this);
    ui->designView->setScene(designScene);
    ui->designView->setRenderHint(QPainter::Antialiasing);
    ui->designView->setDragMode(QGraphicsView::RubberBandDrag);

    propertiesPanel = new WebElementProperties(this);
    ui->rightPanel->layout()->addWidget(propertiesPanel);
}

void MainWindow::createConnections()
{
    connect(designScene, &WebDesignScene::elementSelected,
            this, &MainWindow::onElementSelected);

    connect(propertiesPanel, &WebElementProperties::propertiesChanged,
            this, &MainWindow::updateHtmlPreview);
}

void MainWindow::setupElementsList()
{
    QStringList elements = {
        "Container", "Text", "Heading 1", "Heading 2", "Heading 3",
        "Image", "Button", "Link", "List", "Input", "Textarea", "Form",
        "Section", "Article", "Footer", "Navigation"
    };

    ui->elementsList->addItems(elements);
    ui->elementsList->setDragEnabled(true);
    ui->elementsList->setDefaultDropAction(Qt::CopyAction);
}

void MainWindow::onElementSelected(QGraphicsItem *item)
{
    propertiesPanel->setCurrentElement(item);
}

void MainWindow::updateHtmlPreview()
{
    QJsonObject project = designScene->toJson();
    QString html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Generated Page</title>\n";

    // Add CSS
    html += "<style>\n";
    html += "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html += propertiesPanel->getGlobalCss();
    html += "\n</style>\n</head>\n<body>\n";

    // Add elements
    QJsonArray elements = project["elements"].toArray();
    for (const QJsonValue &element : elements) {
        html += propertiesPanel->generateHtml(element.toObject()) + "\n";
    }

    html += "\n</body>\n</html>";

    ui->htmlPreview->setPlainText(html);
}

void MainWindow::saveDesign()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Design"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Web Design Files (*.webdesign)"));

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save file"));
        return;
    }

    QJsonObject project = designScene->toJson();
    project["properties"] = propertiesPanel->getGlobalProperties();

    file.write(QJsonDocument(project).toJson());
    file.close();
}

void MainWindow::loadDesign()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Load Design"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Web Design Files (*.webdesign)"));

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file"));
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid design file"));
        return;
    }

    QJsonObject project = doc.object();
    designScene->fromJson(project["elements"].toArray());
    propertiesPanel->setGlobalProperties(project["properties"].toObject());

    updateHtmlPreview();
}

void MainWindow::exportHtml()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export HTML"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("HTML Files (*.html)"));

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save file"));
        return;
    }

    file.write(ui->htmlPreview->toPlainText().toUtf8());
    file.close();
}

void MainWindow::clearCanvas()
{
    designScene->clear();
    propertiesPanel->clear();
    ui->htmlPreview->clear();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About Qt Web Designer"),
        tr("A modern web design tool using Qt 6.8\n\n"
           "Features:\n"
           "- Drag & drop interface\n"
           "- Responsive design elements\n"
           "- Real-time HTML/CSS preview\n"
           "- Project saving/loading\n\n"
           "Version 2.0"));
}