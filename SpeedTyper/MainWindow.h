#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Speed Typer
// Dominic Gasperini

// includes
#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QStyleFactory>
#include "Typer.h"
#include "AboutDlg.h"

// Set namespace
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor & Destructor
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Init UI
    Ui::MainWindow *ui;

    // Classes
    Typer* typer;
    AboutDlg* aboutDlg;

    // Variables
    bool testStarted;
    bool testEnded;

    // Palette
    QPalette currentPalette;

    // File Dialog
    QFileDialog* fileDialog;

    // Timers
    QTimer* updateWindowTimer;
    QElapsedTimer* testDuration;


signals:


private slots:

    void UpdateDisplay();

    void on_SaveResultsBtn_clicked();

    void on_GraphBtn_clicked();

    void on_ResetBtn_clicked();

    void on_actionDark_Mode_toggled(bool arg1);

    void on_InputTbx_textChanged();

    void on_NewPromptBtn_clicked();
    void on_actionImport_Prompt_File_triggered();
    void on_actionShow_Prompts_triggered();
    void on_actionAbout_triggered();
    void on_actionHow_to_use_Speed_Typer_triggered();
};


#endif // MAINWINDOW_H
