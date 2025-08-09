// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBox>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>

class WebItem : public QGraphicsItem
{
public:
    WebItem(const QString &type, QGraphicsItem *parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    QString toHtml() const;
    void setHtmlProperties(const QMap<QString, QString> &props);
    
    QString type() const { return m_type; }
    QMap<QString, QString> properties() const { return m_properties; }
    
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    QString m_type;
    QMap<QString, QString> m_properties;
    QRectF m_rect;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updatePreview();
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void itemSelected(QGraphicsItem *item);
    void updateItemProperties();
    void addItem(QAction *action);
    void toggleSourceView(bool checked);

private:
    void createActions();
    void createToolBars();
    void createToolBox();
    void createEditor();
    void setupConnections();
    void loadFile(const QString &fileName);
    void saveToFile(const QString &fileName);
    QString generateHtml() const;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QToolBox *toolBox;
    QWebEngineView *preview;
    QTabWidget *tabs;
    QString currentFile;
    
    // Property editor
    QDockWidget *propertyDock;
    QFormLayout *propertyLayout;
    QWidget *propertyWidget;
    QLineEdit *idEdit;
    QLineEdit *classEdit;
    QLineEdit *textEdit;
    QLineEdit *srcEdit;
    QLineEdit *hrefEdit;
    QSpinBox *widthSpin;
    QSpinBox *heightSpin;
    QComboBox *tagCombo;
    
    // Actions
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *deleteAct;
    QAction *sourceViewAct;
    
    // Current selected item
    WebItem *currentItem;
};

#endif // MAINWINDOW_H