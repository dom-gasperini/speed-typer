// Speed Typer
// Dominic Gasperini

// includes
#include "MainWindow.h"
#include "ui_MainWindow.h"

// defines
#define WINDOW_UPDATE_INTERVAL      25      // in milliseconds

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/speed_typer_icon.ico"));
    ui->InputTbx->setEnabled(false);
    ui->GraphBtn->setEnabled(false);
    currentPalette = qApp->palette();

    // About Dialog
    aboutDlg = new AboutDlg();

    // Typer
    typer = new Typer();
    testStarted = false;
    testEnded = false;

    // Setup Timers
    updateWindowTimer = new QTimer(this);
    connect(updateWindowTimer, SIGNAL(timeout()), this, SLOT(UpdateDisplay()));
    updateWindowTimer->start(WINDOW_UPDATE_INTERVAL);

    testDuration = new QElapsedTimer();

    // File Dialog
    fileDialog = new QFileDialog();
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter("*.txt");
}


/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief MainWindow::UpdateWindow
 */
void MainWindow::UpdateDisplay()
{
    // Prompt length box
    switch (ui->PromptLengthCbx->currentIndex()) {
    case 0:
        typer->setPromptLength(SHORT);
        break;

    case 1:
        typer->setPromptLength(MEDIUM);
        break;

    case 2:
        typer->setPromptLength(LONG);
        break;

    default:
        typer->setPromptLength(MEDIUM);
        break;
    }

    // Checkbox
    if (ui->StartTestCkbx->isChecked()) {
        ui->InputTbx->setEnabled(true);
    }
    else {
        ui->InputTbx->setEnabled(false);
    }

    // Set Prompt
    ui->PromptTbx->setText(typer->getPromptText());

    // Update Results
    ui->TimeSbx->setValue(typer->getDuration());
    ui->AccuracySbx->setValue(typer->getAccuracy());
    ui->WPMSbx->setValue(typer->getWordsPerMinute());

    // Enable / Disable Graphing Button & Save Results Button
    if (testEnded) {
        ui->GraphBtn->setEnabled(true);
        ui->SaveResultsBtn->setEnabled(true);
    }
    else {
        ui->GraphBtn->setEnabled(false);
        ui->SaveResultsBtn->setEnabled(false);
    } 
}


/**
 * @brief MainWindow::on_InputTbx_textChanged
 */
void MainWindow::on_InputTbx_textChanged()
{
    // Check for length greater than 0 to start test
    if (ui->InputTbx->toPlainText().length() > 0 && !testStarted) {
        // start timer
        testDuration->start();
        qDebug() << "started timer";
        testStarted = true;
    }

    // save letter and elasped time on key press
    if (testStarted && !testEnded) {
        QChar tmpChar = ui->InputTbx->toPlainText().at(ui->InputTbx->toPlainText().length() - 1);
        float tmpTime = testDuration->elapsed();
        tmpTime /= 1000;
        qDebug() << "char: " << tmpChar << "time:" << tmpTime;
        typer->addKeystrokeData(tmpChar, tmpTime);
    }

    // Update progress bar
    if (testStarted && !testEnded) {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(typer->getPromptText().length());
        ui->progressBar->setValue(ui->InputTbx->toPlainText().length());
    }

    // Check for a complete test
    if (ui->InputTbx->toPlainText().length() >= typer->getPromptText().length() && !testEnded) {
        // update results
        float tmpDur = testDuration->elapsed();
        tmpDur /= 1000;
        typer->setDuration(tmpDur);
        testDuration->invalidate();
        qDebug() << "duration: " << typer->getDuration();

        typer->calculateAccuracy(ui->InputTbx->toPlainText());
        qDebug() << "accuracy:" << typer->getAccuracy();
        typer->calculateWordsPerMinute(ui->InputTbx->toPlainText());
        qDebug() << "wpm: " << typer->getWordsPerMinute();

        testEnded = true;
    }
}


/**
 * @brief MainWindow::on_NewPromptBtn_clicked
 */
void MainWindow::on_NewPromptBtn_clicked()
{
    typer->getRandomPrompt();
}


/**
 * @brief MainWindow::on_SaveResultsBtn_clicked
 */
void MainWindow::on_SaveResultsBtn_clicked()
{
    typer->saveFile(ui->InputTbx->toPlainText());
}


/**
 * @brief MainWindow::on_GraphBtn_clicked
 */
void MainWindow::on_GraphBtn_clicked()
{
    // Inits
    QBarCategoryAxis* charData = new QBarCategoryAxis();
    QLineSeries* timeData = new QLineSeries();
    QValueAxis* yAxis = new QValueAxis();

    // add character data to series
    std::vector<QChar> tmpCharVec = typer->getKeystrokeData();
    std::vector<float> tmpTimeVec = typer->getKeystrokeTime();

    // check for invalid data
    if (tmpCharVec.size() != tmpTimeVec.size()) {
        QMessageBox messageBox;
        QString message = "Invalid data detected!\n Cannot plot data!";
        messageBox.setText(message);
        messageBox.exec();
        return;
    }

    // add char data to series
    for (unsigned long i = 0; i < tmpCharVec.size(); ++i) {
        charData->append(QString(tmpCharVec.at(i)) + "("  + QString::number(i) + ")");
    }

    // add time data to series
    for (unsigned long i = 0; i < tmpTimeVec.size(); ++i) {
        timeData->append(QPointF(i, tmpTimeVec.at(i)));
    }

    // set char data axis label
    charData->setTitleText("Characters");

    // setup y axis
    yAxis->setMin(tmpTimeVec.at(0));
    yAxis->setMax(tmpTimeVec.at(tmpTimeVec.size() - 1));
    yAxis->setMinorTickCount((int)(tmpTimeVec.at(tmpTimeVec.size() - 1)));
    yAxis->setTitleText("Time (seconds)");

    // init chart
    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(timeData);
    chart->addAxis(yAxis, Qt::AlignLeft);
    chart->addAxis(charData, Qt::AlignBottom);
    chart->setTitle("Keystroke Time Data");

    // init chart view
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->show();
}


/**
 * @brief MainWindow::on_pushButton_clicked
 */
void MainWindow::on_ResetBtn_clicked()
{
    // reset test data
    typer->setAccuracy(0);
    typer->setDuration(0);
    typer->setWordsPerMinute(0);
    typer->clearKeystrokeData();

    // update UI
    ui->InputTbx->setText("");
    ui->StartTestCkbx->setChecked(false);
    ui->progressBar->setValue(0);

    // update flags
    testStarted = false;
    testEnded = false;
}


/**
 * @brief MainWindow::on_actionImport_Prompt_File_triggered
 */
void MainWindow::on_actionImport_Prompt_File_triggered()
{
    // inits
    QStringList fileList;

    // open file dialog
    if (fileDialog->exec()) {
        // get selected file
        fileList = fileDialog->selectedFiles();

        // Get the file from the directory and save
        typer->setPromptsFilename(fileList[0]);
        typer->readFile();
    }
}


/**
 * @brief MainWindow::on_actionShow_Prompts_triggered
 */
void MainWindow::on_actionShow_Prompts_triggered()
{
    // inits & setup
    QMessageBox messageBox;
    QString message = "Available Prompts:\n\n";
    std::vector<QString> tmpVec = typer->getPrompts();

    // concatenate all the prompts into the message
    for (unsigned long i = 0; i < tmpVec.size(); ++i) {
        message += "\"" + tmpVec.at(i) + "\"\n\n";
    }

    // show pop-up
    messageBox.setText(message);
    messageBox.exec();
}


/**
 * @brief MainWindow::on_actionAbout_triggered
 */
void MainWindow::on_actionAbout_triggered()
{
    aboutDlg->show();
}


/**
 * @brief MainWindow::on_actionHow_to_use_Speed_Typer_triggered
 */
void MainWindow::on_actionHow_to_use_Speed_Typer_triggered()
{
    // inits
    QMessageBox messageBox;
    QString message = "How to use Speed Typer:\n\n\n";

    // setup message
    message += "1. Generate a prompt! Prompts vary in length, the options are: short, medium, or long\n\n";
    message += "2. When you're ready to start the typing test, check the ready box, this will enable the typing box\n\n";
    message += "3. Type away! The program will gather information about your typing test and provide you with the results\n\n";
    message += "4. Optionally, you can also save your results and even plot your key press timing distribution!";

    // show pop-up
    messageBox.setText(message);
    messageBox.exec();
}


/**
 * @brief MainWindow::on_actionDark_Mode_toggled
 * @param arg1
 */
void MainWindow::on_actionDark_Mode_toggled(bool checked)
{
    if (checked)
    {
        // Set Style
        qApp->setStyle(QStyleFactory::create("Fusion"));
        // Init a Dark Mode Palette
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
        darkPalette.setColor(QPalette::Base,QColor(42,42,42));
        darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
        darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
        darkPalette.setColor(QPalette::ToolTipText,Qt::white);
        darkPalette.setColor(QPalette::Text,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
        darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
        darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
        darkPalette.setColor(QPalette::Button,QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
        darkPalette.setColor(QPalette::BrightText,Qt::red);
        darkPalette.setColor(QPalette::Link,QColor(42,130,218));
        darkPalette.setColor(QPalette::Highlight, QColor(142,45,197));
        darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
        darkPalette.setColor(QPalette::HighlightedText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

        // Set to Dark Mode
        qApp->setPalette(darkPalette);

        // Change Progress Bar Color to Green
        QPalette progressBarPalette;
        progressBarPalette.setColor(QPalette::Highlight, Qt::green);
    }

    else
    {
        // Reset Style to Windows Vista
        qApp->setStyle(QStyleFactory::create("windowsvista"));

        // Reset to Light Mode
        currentPalette.setColor(QPalette::Text, Qt::black);
        currentPalette.setColor(QPalette::HighlightedText, Qt::black);
        currentPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::black);
        currentPalette.setColor(QPalette::ButtonText,Qt::black);
        currentPalette.setColor(QPalette::HighlightedText, Qt::black);
        currentPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::black);
        currentPalette.setColor(QPalette::WindowText, Qt::black);
        currentPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::black);
        currentPalette.setColor(QPalette::ToolTipBase,Qt::black);
        currentPalette.setColor(QPalette::ToolTipText,Qt::black);
        currentPalette.setColor(QPalette::Disabled,QPalette::Highlight, Qt::black);
        currentPalette.setColor(QPalette::HighlightedText,Qt::black);
        currentPalette.setColor(QPalette::Disabled,QPalette::HighlightedText, Qt::black);
        currentPalette.setColor(QPalette::All, QPalette::ButtonText, Qt::black);
        currentPalette.setColor(QPalette::All, QPalette::Text, Qt::black);
        qApp->setPalette(currentPalette);
    }
}
