#include "MainWindow.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QStatusBar>   // required for statusBar() to be complete

// ─── Helpers ──────────────────────────────────────────────────────────────────

static QFrame* makeCard(const QString &title = {}) {
    auto *f = new QFrame; f->setObjectName("card");
    auto *lay = new QVBoxLayout(f);
    lay->setContentsMargins(16,14,16,14); lay->setSpacing(10);
    if (!title.isEmpty()) {
        auto *l = new QLabel(title.toUpper());
        l->setObjectName("cardTitle"); lay->addWidget(l);
    }
    return f;
}

static QFrame* makeSep() {
    auto *s = new QFrame; s->setFrameShape(QFrame::HLine);
    s->setStyleSheet("border-color:#EEC8D8;"); return s;
}

// ─── Constructor / Destructor ─────────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("TelemetryLogger — Server Dashboard");
    setMinimumSize(1100,700); resize(1300,800);

    m_guiSink = new GUISink(this);
    m_server  = new GUIServer(this);

    connect(m_guiSink, &GUISink::messageReceived,
            this,      &MainWindow::onMessageReceived,
            Qt::QueuedConnection);   // crosses the vsomeip thread → GUI thread

    connect(m_server, &GUIServer::stopped,
            this,     &MainWindow::onServerStopped);
    connect(m_server, &GUIServer::errorOccurred,
            this,     &MainWindow::onServerError);

    setupStyles();
    setupUI();

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, this, &MainWindow::onPollTimer);
    m_pollTimer->start(500);
}

MainWindow::~MainWindow() { m_server->stop(); }

// ─── Stylesheet ───────────────────────────────────────────────────────────────

void MainWindow::setupStyles() {
    setStyleSheet(R"(
        QMainWindow, QWidget#root { background:#FDF0F5; }
        QFrame#card { background:#FFFFFF; border:1.5px solid #EEC8D8; border-radius:14px; }
        QLabel#cardTitle { color:#9A7080; font-size:9px; letter-spacing:2px; font-weight:600; }
        QWidget#header { background:#FFFFFF; border-bottom:1.5px solid #EEC8D8; }
        QLabel#appName { font-size:20px; font-weight:800; color:#D4607E; }
        QLabel#appSub  { font-size:9px; color:#9A7080; letter-spacing:2px; }
        QLabel#statValue      { font-size:30px; font-weight:800; color:#D4607E; }
        QLabel#statValueRed   { font-size:30px; font-weight:800; color:#C94040; }
        QLabel#statValueAmber { font-size:30px; font-weight:800; color:#C4873A; }
        QLabel#statValueGreen { font-size:30px; font-weight:800; color:#5DB87A; }
        QLabel#statLabel { font-size:9px; color:#9A7080; letter-spacing:2px; font-weight:600; }
        QLabel#statUnit  { font-size:10px; color:#BBA0AA; }
        QPushButton#btnPrimary {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #D4607E,stop:1 #E8889E);
            color:white; border:none; border-radius:9px;
            padding:8px 20px; font-size:11px; font-weight:700; letter-spacing:1px; }
        QPushButton#btnPrimary:hover {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #C05070,stop:1 #D4789E); }
        QPushButton#btnStop {
            background:#FFF0F0; color:#C94040; border:1.5px solid #F4B0B0;
            border-radius:9px; padding:8px 20px; font-size:11px; font-weight:700; }
        QPushButton#btnStop:hover { background:#FFE0E0; }
        QPushButton#btnGhost {
            background:transparent; color:#D4607E; border:1.5px solid #EEC8D8;
            border-radius:9px; padding:7px 14px; font-size:11px; }
        QPushButton#btnGhost:hover { background:#FCE8F0; border-color:#E8889E; }
        QPushButton#btnDanger {
            background:#FFF0F0; color:#C94040; border:1.5px solid #F4B0B0;
            border-radius:9px; padding:6px 14px; font-size:11px; }
        QPushButton#btnDanger:hover { background:#FFE0E0; }
        QLineEdit {
            background:#FDE8F0; border:1.5px solid #EEC8D8; border-radius:8px;
            color:#3A2030; padding:6px 10px; font-size:11px; }
        QLineEdit:focus    { border-color:#E8889E; }
        QLineEdit:disabled { background:#F5E8ED; color:#BBA0AA; }
        QSlider::groove:horizontal { height:4px; background:#EEC8D8; border-radius:2px; }
        QSlider::handle:horizontal { width:14px; height:14px; background:#D4607E; border-radius:7px; margin:-5px 0; }
        QSlider::sub-page:horizontal { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #D4607E,stop:1 #E8889E); border-radius:2px; }
        QCheckBox { color:#3A2030; font-size:11px; spacing:8px; }
        QCheckBox::indicator { width:16px; height:16px; border-radius:4px; border:1.5px solid #EEC8D8; background:#FFFFFF; }
        QCheckBox::indicator:checked { background:#D4607E; border-color:#D4607E; }
        QProgressBar { background:#FDE8F0; border:none; border-radius:5px; height:10px; color:transparent; }
        QProgressBar::chunk { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #D4607E,stop:1 #F2B0C0); border-radius:5px; }
        QTableWidget {
            background:#FFFAFC; alternate-background-color:#FDF0F5;
            border:1.5px solid #EEC8D8; border-radius:10px;
            gridline-color:#F5D8E8; font-size:11px; color:#3A2030;
            selection-background-color:#FCE8F0; selection-color:#3A2030; }
        QTableWidget::item { padding:4px 8px; }
        QHeaderView::section {
            background:#FDE8F0; color:#9A7080; border:none;
            border-bottom:1.5px solid #EEC8D8; padding:6px 8px;
            font-size:9px; letter-spacing:1.5px; font-weight:700; }
        QScrollBar:vertical { background:#FDE8F0; width:6px; border-radius:3px; }
        QScrollBar::handle:vertical { background:#E8889E; border-radius:3px; min-height:20px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
        QComboBox {
            background:#FDE8F0; border:1.5px solid #EEC8D8; border-radius:8px;
            color:#3A2030; padding:5px 10px; font-size:11px; min-width:110px; }
        QComboBox::drop-down { border:none; width:20px; }
        QComboBox QAbstractItemView {
            background:#FFFFFF; border:1.5px solid #EEC8D8;
            selection-background-color:#FCE8F0; color:#3A2030; }
        QLabel#sectionLabel { color:#9A7080; font-size:9px; letter-spacing:2px; font-weight:700; }
        QLabel#infoKey   { color:#9A7080; font-size:11px; }
        QLabel#infoValue { color:#D4607E; font-size:11px; font-weight:700; }
        QStatusBar { background:#FFFFFF; color:#9A7080; font-size:10px; border-top:1px solid #EEC8D8; }
    )");
}

// ─── UI Construction ──────────────────────────────────────────────────────────

void MainWindow::setupUI() {
    auto *root = new QWidget; root->setObjectName("root"); setCentralWidget(root);
    auto *rootLay = new QVBoxLayout(root);
    rootLay->setContentsMargins(0,0,0,0); rootLay->setSpacing(0);

    // Header
    auto *header = new QWidget; header->setObjectName("header"); header->setFixedHeight(64);
    auto *hLay = new QHBoxLayout(header); hLay->setContentsMargins(24,0,24,0);
    auto *logoCol = new QVBoxLayout; logoCol->setSpacing(1);
    auto *nameLbl = new QLabel("🌸  TelemetryLogger Server"); nameLbl->setObjectName("appName");
    auto *subLbl  = new QLabel("SOMEIP SERVER DASHBOARD — LAPTOP"); subLbl->setObjectName("appSub");
    logoCol->addWidget(nameLbl); logoCol->addWidget(subLbl);
    hLay->addLayout(logoCol); hLay->addStretch();

    auto *pill = new QWidget;
    pill->setStyleSheet("background:#FCE8F0;border:1.5px solid #E8889E;border-radius:14px;");
    auto *pillLay = new QHBoxLayout(pill);
    pillLay->setContentsMargins(10,4,10,4); pillLay->setSpacing(6);
    m_statusDot   = new QLabel("●"); m_statusDot->setStyleSheet("color:#BBA0AA;font-size:10px;");
    m_statusLabel = new QLabel("STOPPED");
    m_statusLabel->setStyleSheet("color:#9A7080;font-size:10px;letter-spacing:1.5px;font-weight:700;");
    pillLay->addWidget(m_statusDot); pillLay->addWidget(m_statusLabel);
    hLay->addWidget(pill); hLay->addSpacing(12);

    m_toggleBtn = new QPushButton("▶  Start Server");
    m_toggleBtn->setObjectName("btnPrimary"); m_toggleBtn->setCursor(Qt::PointingHandCursor);
    connect(m_toggleBtn, &QPushButton::clicked, this, &MainWindow::onToggleServer);
    hLay->addWidget(m_toggleBtn);
    rootLay->addWidget(header);

    // Content
    auto *content = new QWidget;
    auto *cLay = new QHBoxLayout(content);
    cLay->setContentsMargins(20,20,20,20); cLay->setSpacing(16);

    auto *sideScroll = new QScrollArea;
    sideScroll->setWidgetResizable(true); sideScroll->setFrameShape(QFrame::NoFrame);
    sideScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScroll->setFixedWidth(288); sideScroll->setStyleSheet("background:transparent;");
    sideScroll->setWidget(buildSidebar());
    cLay->addWidget(sideScroll);

    auto *rPanel = new QVBoxLayout; rPanel->setSpacing(16);
    rPanel->addWidget(buildStatsRow());

    auto *split = new QSplitter(Qt::Horizontal);
    split->setStyleSheet("QSplitter::handle{background:#EEC8D8;width:2px;}");
    split->addWidget(buildLogPanel());
    split->addWidget(buildInfoPanel());
    split->setSizes({780,260});
    rPanel->addWidget(split, 1);
    cLay->addLayout(rPanel, 1);
    rootLay->addWidget(content, 1);

    statusBar()->showMessage("  Ready — set the server.json path and press ▶ Start Server");
}

QWidget* MainWindow::buildSidebar() {
    auto *w = new QWidget; w->setStyleSheet("background:transparent;");
    auto *lay = new QVBoxLayout(w); lay->setContentsMargins(0,0,4,0); lay->setSpacing(14);
    lay->addWidget(buildConfigPanel());
    lay->addWidget(buildBufferPanel());
    lay->addStretch();
    return w;
}

QWidget* MainWindow::buildConfigPanel() {
    auto *card = makeCard("⚙  Server Configuration");
    auto *lay  = qobject_cast<QVBoxLayout*>(card->layout());
    auto addSec = [&](const QString &t) {
        auto *l = new QLabel(t); l->setObjectName("sectionLabel"); lay->addWidget(l);
    };

    addSec("VSOMEIP CONFIG (server.json)");
    m_vsomeipConfigEdit = new QLineEdit(
        "/home/yasmine/Desktop/Linux_Project/Server/server.json");
    lay->addWidget(m_vsomeipConfigEdit);
    auto *browseBtn = new QPushButton("Browse…");
    browseBtn->setObjectName("btnGhost"); browseBtn->setCursor(Qt::PointingHandCursor);
    connect(browseBtn, &QPushButton::clicked, this, [this]() {
        QString p = QFileDialog::getOpenFileName(this, "Select server.json", "", "JSON (*.json)");
        if (!p.isEmpty()) m_vsomeipConfigEdit->setText(p);
    });
    lay->addWidget(browseBtn);
    lay->addWidget(makeSep());

    addSec("LOG SINKS");
    m_consoleSinkCb = new QCheckBox("Console Sink"); m_consoleSinkCb->setChecked(true);
    m_fileSinkCb    = new QCheckBox("File Sink");    m_fileSinkCb->setChecked(false);
    connect(m_consoleSinkCb, &QCheckBox::toggled, this, &MainWindow::onConsoleSinkToggled);
    connect(m_fileSinkCb,    &QCheckBox::toggled, this, &MainWindow::onFileSinkToggled);
    lay->addWidget(m_consoleSinkCb);
    lay->addWidget(m_fileSinkCb);
    addSec("LOG FILE PATH");
    m_filePathEdit = new QLineEdit("/home/yasmine/Desktop/server_logs.txt");
    m_filePathEdit->setEnabled(false);
    lay->addWidget(m_filePathEdit);
    lay->addWidget(makeSep());

    addSec("RING BUFFER SIZE");
    auto *slRow = new QHBoxLayout;
    m_bufferSlider = new QSlider(Qt::Horizontal);
    m_bufferSlider->setRange(64,4096); m_bufferSlider->setSingleStep(64); m_bufferSlider->setValue(1024);
    connect(m_bufferSlider, &QSlider::valueChanged, this, &MainWindow::onBufferSizeChanged);
    m_bufferValLabel = new QLabel("1024");
    m_bufferValLabel->setStyleSheet("color:#D4607E;font-weight:700;font-size:11px;min-width:36px;");
    slRow->addWidget(m_bufferSlider); slRow->addWidget(m_bufferValLabel);
    lay->addLayout(slRow);
    lay->addWidget(makeSep());

    auto *applyBtn = new QPushButton("Apply && Restart");
    applyBtn->setObjectName("btnPrimary"); applyBtn->setCursor(Qt::PointingHandCursor);
    connect(applyBtn, &QPushButton::clicked, this, &MainWindow::onApplyRestart);
    lay->addWidget(applyBtn);
    return card;
}

QWidget* MainWindow::buildBufferPanel() {
    auto *card = makeCard("🔄  Log Ring Buffer");
    auto *lay  = qobject_cast<QVBoxLayout*>(card->layout());

    m_bufferBar = new QProgressBar;
    m_bufferBar->setRange(0,100); m_bufferBar->setValue(0); m_bufferBar->setFixedHeight(10);
    lay->addWidget(m_bufferBar);

    auto *pctRow = new QHBoxLayout;
    m_bufUsedLabel = new QLabel("0 / 1024");
    m_bufUsedLabel->setStyleSheet("color:#D4607E;font-size:11px;font-weight:700;");
    m_bufPctLabel = new QLabel("0%");
    m_bufPctLabel->setStyleSheet("color:#9A7080;font-size:11px;");
    pctRow->addWidget(m_bufUsedLabel); pctRow->addStretch(); pctRow->addWidget(m_bufPctLabel);
    lay->addLayout(pctRow);
    lay->addWidget(makeSep());

    auto *lbl = new QLabel("TOTAL LOGGED"); lbl->setObjectName("sectionLabel"); lay->addWidget(lbl);
    m_totalLoggedLabel = new QLabel("0");
    m_totalLoggedLabel->setStyleSheet("color:#D4607E;font-size:22px;font-weight:800;");
    lay->addWidget(m_totalLoggedLabel);
    return card;
}

QWidget* MainWindow::buildStatsRow() {
    auto *w = new QWidget; w->setStyleSheet("background:transparent;");
    auto *lay = new QHBoxLayout(w); lay->setContentsMargins(0,0,0,0); lay->setSpacing(12);

    auto makeStatCard = [&](const QString &label, QLabel *&ref,
                             const QString &obj, const QString &unit, const QString &icon) {
        auto *card = makeCard();
        auto *cl   = qobject_cast<QVBoxLayout*>(card->layout());
        auto *topRow = new QHBoxLayout;
        auto *lbl = new QLabel(label.toUpper()); lbl->setObjectName("statLabel");
        topRow->addWidget(lbl); topRow->addStretch();
        topRow->addWidget(new QLabel(icon));
        cl->addLayout(topRow);
        ref = new QLabel("0"); ref->setObjectName(obj); cl->addWidget(ref);
        auto *u = new QLabel(unit); u->setObjectName("statUnit"); cl->addWidget(u);
        lay->addWidget(card, 1);
    };

    makeStatCard("Log Messages", m_statTotal,     "statValue",      "entries",          "📋");
    makeStatCard("Critical",     m_statCritical,  "statValueRed",   "alerts",           "🚨");
    makeStatCard("Warnings",     m_statWarning,   "statValueAmber", "warnings",         "⚠️");
    makeStatCard("Latest CPU",   m_statCpuLatest, "statValueGreen", "most recent read", "📡");
    m_statCpuLatest->setText("–");
    return w;
}

QWidget* MainWindow::buildLogPanel() {
    auto *card = makeCard("📜  Live Log Stream");
    auto *lay  = qobject_cast<QVBoxLayout*>(card->layout());

    auto *toolbar = new QHBoxLayout; toolbar->setSpacing(8);
    m_filterCombo = new QComboBox;
    m_filterCombo->addItems({"All Levels","CRITICAL","WARNING","INFO"});
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);
    toolbar->addWidget(new QLabel("Filter:"));
    toolbar->addWidget(m_filterCombo);
    toolbar->addStretch();
    auto *clearBtn = new QPushButton("Clear");
    clearBtn->setObjectName("btnDanger"); clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearLogs);
    toolbar->addWidget(clearBtn);
    lay->addLayout(toolbar);

    m_logTable = new QTableWidget(0, 5);
    m_logTable->setHorizontalHeaderLabels({"TIME","APP","CTX","SEVERITY","MESSAGE"});
    m_logTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_logTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_logTable->verticalHeader()->setVisible(false);
    m_logTable->setEditTriggers(QTableWidget::NoEditTriggers);
    m_logTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_logTable->setAlternatingRowColors(true);
    m_logTable->verticalHeader()->setDefaultSectionSize(26);
    lay->addWidget(m_logTable, 1);
    return card;
}

QWidget* MainWindow::buildInfoPanel() {
    auto *card = makeCard("🔗  Connection Info");
    auto *lay  = qobject_cast<QVBoxLayout*>(card->layout());

    auto addRow = [&](const QString &key, QLabel *&ref, const QString &val) {
        auto *row = new QHBoxLayout;
        auto *k = new QLabel(key + ":"); k->setObjectName("infoKey");
        ref = new QLabel(val); ref->setObjectName("infoValue"); ref->setWordWrap(true);
        row->addWidget(k,0); row->addWidget(ref,1); lay->addLayout(row);
    };
    auto addStatic = [&](const QString &key, const QString &val) {
        auto *row = new QHBoxLayout;
        auto *k = new QLabel(key + ":"); k->setObjectName("infoKey");
        auto *v = new QLabel(val); v->setStyleSheet("color:#3A2030;font-size:11px;"); v->setWordWrap(true);
        row->addWidget(k,0); row->addWidget(v,1); lay->addLayout(row);
    };

    addRow("Service ID",  m_serviceIdLabel,  "0x1234");
    addRow("Instance ID", m_clientConnLabel, "0x5678");
    addStatic("Method ID",    "0x0125");
    addStatic("Server IP",    "192.168.1.40");
    addStatic("Client IP",    "192.168.1.50 (RPi)");
    addStatic("Discovery",    "UDP multicast");
    addStatic("SD Port",      "30490");
    addStatic("Service port", "UDP 30501");
    lay->addWidget(makeSep());

    auto *note = new QLabel(
        "Start server here, then run ./Logging_app on the RPi. "
        "CPU readings will stream into the log above.");
    note->setObjectName("infoKey"); note->setWordWrap(true);
    lay->addWidget(note);
    lay->addStretch();
    return card;
}

// ─── Real data slot ───────────────────────────────────────────────────────────

void MainWindow::onMessageReceived(const QString &app, const QString &ctx,
                                    const QString &sev, const QString &time,
                                    const QString &text) {
    m_total++;
    if (sev == "CRITICAL") m_critical++;
    if (sev == "WARNING")  m_warning++;
    updateStats();
    if (ctx == "CPU") m_statCpuLatest->setText(text);

    QString filter = m_filterCombo->currentText();
    if (filter != "All Levels" && filter != sev) return;
    addLogRow(app, ctx, sev, time, text);
}

void MainWindow::addLogRow(const QString &app, const QString &ctx,
                            const QString &sev, const QString &time,
                            const QString &text) {
    int row = m_logTable->rowCount(); m_logTable->insertRow(row);

    QColor sevBg, sevFg;
    if      (sev == "CRITICAL") { sevBg = {255,240,240}; sevFg = {201,64,64};  }
    else if (sev == "WARNING")  { sevBg = {255,250,230}; sevFg = {196,135,58}; }
    else                        { sevBg = {235,250,242}; sevFg = {93,184,122}; }

    auto *timeItem = new QTableWidgetItem(time);
    auto *appItem  = new QTableWidgetItem(app);
    auto *ctxItem  = new QTableWidgetItem(ctx);
    auto *sevItem  = new QTableWidgetItem(sev);
    auto *txtItem  = new QTableWidgetItem(text);

    sevItem->setBackground(sevBg); sevItem->setForeground(sevFg);
    QFont bf; bf.setBold(true); bf.setPointSize(8);
    sevItem->setFont(bf); sevItem->setTextAlignment(Qt::AlignCenter);
    timeItem->setForeground(QColor(154,112,128));
    appItem->setForeground(QColor(212,96,126));
    ctxItem->setForeground(QColor(154,112,128));

    m_logTable->setItem(row,0,timeItem); m_logTable->setItem(row,1,appItem);
    m_logTable->setItem(row,2,ctxItem);  m_logTable->setItem(row,3,sevItem);
    m_logTable->setItem(row,4,txtItem);

    if (m_logTable->rowCount() > 500) m_logTable->removeRow(0);
    m_logTable->scrollToBottom();
}

// ─── Poll timer ───────────────────────────────────────────────────────────────

void MainWindow::onPollTimer() {
    if (!m_server->isRunning()) return;
    updateBufferViz();
    m_totalLoggedLabel->setText(QString::number(m_server->totalLogged()));
}

void MainWindow::updateBufferViz() {
    size_t used = m_server->bufferUsed();
    size_t cap  = m_server->bufferCap();
    int    pct  = (cap > 0) ? (int)(100.0 * used / cap) : 0;
    m_bufferBar->setValue(pct);
    m_bufUsedLabel->setText(QString("%1 / %2").arg(used).arg(cap));
    m_bufPctLabel->setText(QString::number(pct) + "%");
}

void MainWindow::updateStats() {
    m_statTotal->setText(QString::number(m_total));
    m_statCritical->setText(QString::number(m_critical));
    m_statWarning->setText(QString::number(m_warning));
}

// ─── Server lifecycle ─────────────────────────────────────────────────────────

void MainWindow::onToggleServer() {
    if (!m_serverRunning) {
        m_server->start(
            m_vsomeipConfigEdit->text(),
            m_consoleSinkCb->isChecked(),
            m_fileSinkCb->isChecked(),
            m_filePathEdit->text(),
            static_cast<size_t>(m_bufferSlider->value()),
            m_guiSink
        );
        setServerRunning(true);
        statusBar()->showMessage("  Server started — offering SERVICE 0x1234 / INSTANCE 0x5678");
    } else {
        m_server->stop();
        setServerRunning(false);
        statusBar()->showMessage("  Server stopped.");
    }
}

void MainWindow::onApplyRestart() {
    if (m_serverRunning) { m_server->stop(); setServerRunning(false); }
    onToggleServer();
}

void MainWindow::setServerRunning(bool running) {
    m_serverRunning = running;
    if (running) {
        m_statusDot->setStyleSheet("color:#5DB87A;font-size:10px;");
        m_statusLabel->setText("RUNNING");
        m_statusLabel->setStyleSheet("color:#D4607E;font-size:10px;letter-spacing:1.5px;font-weight:700;");
        m_toggleBtn->setText("⏹  Stop Server");
        m_toggleBtn->setObjectName("btnStop");
    } else {
        m_statusDot->setStyleSheet("color:#BBA0AA;font-size:10px;");
        m_statusLabel->setText("STOPPED");
        m_statusLabel->setStyleSheet("color:#9A7080;font-size:10px;letter-spacing:1.5px;font-weight:700;");
        m_toggleBtn->setText("▶  Start Server");
        m_toggleBtn->setObjectName("btnPrimary");
    }
    m_toggleBtn->style()->unpolish(m_toggleBtn);
    m_toggleBtn->style()->polish(m_toggleBtn);
}

void MainWindow::onServerStopped() { setServerRunning(false); }

void MainWindow::onServerError(const QString &msg) {
    setServerRunning(false);
    QMessageBox::critical(this, "Server Error", msg);
    statusBar()->showMessage("  Error: " + msg);
}

// ─── UI control slots ─────────────────────────────────────────────────────────

void MainWindow::onClearLogs() {
    m_logTable->setRowCount(0);
    m_total = 0; m_critical = 0; m_warning = 0;
    m_statCpuLatest->setText("–");
    updateStats();
}

void MainWindow::onFilterChanged(int /*index*/) {}

void MainWindow::onConsoleSinkToggled(bool /*checked*/) {}

void MainWindow::onFileSinkToggled(bool checked) {
    m_filePathEdit->setEnabled(checked);
}

void MainWindow::onBufferSizeChanged(int val) {
    m_bufferValLabel->setText(QString::number(val));
}
