#pragma once

#include "GUIServer.hpp"
#include "GUISink.hpp"

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QScrollArea>
#include <QSplitter>
#include <QFrame>
#include <QStatusBar>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMessageReceived(const QString &app, const QString &ctx,
                           const QString &sev, const QString &time,
                           const QString &text);
    void onPollTimer();
    void onToggleServer();
    void onApplyRestart();
    void onClearLogs();
    void onFilterChanged(int index);
    void onServerStopped();
    void onServerError(const QString &msg);
    void onConsoleSinkToggled(bool checked);
    void onFileSinkToggled(bool checked);
    void onBufferSizeChanged(int val);

private:
    void setupStyles();
    void setupUI();
    QWidget* buildSidebar();
    QWidget* buildConfigPanel();
    QWidget* buildBufferPanel();
    QWidget* buildStatsRow();
    QWidget* buildLogPanel();
    QWidget* buildInfoPanel();

    void addLogRow(const QString &app, const QString &ctx,
                   const QString &sev, const QString &time,
                   const QString &text);
    void updateBufferViz();
    void updateStats();
    void setServerRunning(bool running);

    // Backend
    GUISink   *m_guiSink{nullptr};
    GUIServer *m_server{nullptr};
    QTimer    *m_pollTimer{nullptr};
    bool       m_serverRunning{false};

    // Counters
    int m_total=0, m_critical=0, m_warning=0;

    // Config widgets
    QLineEdit  *m_vsomeipConfigEdit;
    QSlider    *m_bufferSlider;
    QLabel     *m_bufferValLabel;
    QCheckBox  *m_consoleSinkCb;
    QCheckBox  *m_fileSinkCb;
    QLineEdit  *m_filePathEdit;

    // Buffer panel
    QProgressBar *m_bufferBar;
    QLabel       *m_bufUsedLabel;
    QLabel       *m_bufPctLabel;
    QLabel       *m_totalLoggedLabel;

    // Stat cards
    QLabel *m_statTotal;
    QLabel *m_statCritical;
    QLabel *m_statWarning;
    QLabel *m_statCpuLatest;

    // Log table
    QTableWidget *m_logTable;
    QComboBox    *m_filterCombo;

    // Header status
    QLabel      *m_statusDot;
    QLabel      *m_statusLabel;
    QPushButton *m_toggleBtn;

    // Info panel
    QLabel *m_serviceIdLabel;
    QLabel *m_clientConnLabel;
};
