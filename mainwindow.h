#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QGraphicsView>
#include <QTextEdit>
#include <QToolBar>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class WebDesignScene;
class WebElementProperties;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateHtmlPreview();
    void saveDesign();
    void loadDesign();
    void exportHtml();
    void clearCanvas();
    void showAbout();
    void onElementSelected(QGraphicsItem *item);

private:
    void createToolBar();
    void createWidgets();
    void createConnections();
    void setupElementsList();

    Ui::MainWindow *ui;
    WebDesignScene *designScene;
    WebElementProperties *propertiesPanel;
};

#endif // MAINWINDOW_H