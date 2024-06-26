#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include "mediadropwidget.h"
#include "json.hpp"
#include <QActionGroup>
#include <filesystem>
#include <fstream>
#include <codecvt>
#include <locale> 
#include <QInputDialog>

using json = nlohmann::json;


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->showMaximized();

	//树控件初始化
	ui->treeWidgetProjectDirectory->header()->setFixedHeight(25);
	ui->treeWidgetDataDisplay->header()->setFixedHeight(25);
	ui->treeWidgetProjectConfiguration->header()->setFixedHeight(25);
	ui->treeWidgetTainData->header()->setFixedHeight(25);
	ui->treeWidgetTrainEvaluation->header()->setFixedHeight(25);



	//工程项目树初始化
	projectDataManager = new ProjectDataManager();



	//view窗口设置与排列
	QGridLayout* layout1 = new QGridLayout(ui->view1);
	mediaDropWidgetView1LeftTop = new MediaDropWidget(this);
	mediaDropWidgetView1RightTop = new MediaDropWidget(this);
	mediaDropWidgetView1LeftBottom = new MediaDropWidget(this);
	mediaDropWidgetView1RightBottom = new MediaDropWidget(this);
	layout1->addWidget(mediaDropWidgetView1LeftTop, 0, 0);
	layout1->addWidget(mediaDropWidgetView1RightTop, 0, 1);
	layout1->addWidget(mediaDropWidgetView1LeftBottom, 1, 0);
	layout1->addWidget(mediaDropWidgetView1RightBottom, 1, 1);

	QGridLayout* layout2 = new QGridLayout(ui->view2);
	mediaDropWidgetView2LeftTop = new MediaDropWidget(this);
	mediaDropWidgetView2RightTop = new MediaDropWidget(this);
	mediaDropWidgetView2LeftBottom = new MediaDropWidget(this);
	mediaDropWidgetView2RightBottom = new MediaDropWidget(this);
	layout2->addWidget(mediaDropWidgetView2LeftTop, 0, 0);
	layout2->addWidget(mediaDropWidgetView2RightTop, 0, 1);
	layout2->addWidget(mediaDropWidgetView2LeftBottom, 1, 0);
	layout2->addWidget(mediaDropWidgetView2RightBottom, 1, 1);

	QGridLayout* layout3 = new QGridLayout(ui->view3);
	mediaDropWidgetView3LeftTop = new MediaDropWidget(this);
	mediaDropWidgetView3RightTop = new MediaDropWidget(this);
	mediaDropWidgetView3LeftBottom = new MediaDropWidget(this);
	mediaDropWidgetView3RightBottom = new MediaDropWidget(this);
	layout3->addWidget(mediaDropWidgetView3LeftTop, 0, 0);
	layout3->addWidget(mediaDropWidgetView3RightTop, 0, 1);
	layout3->addWidget(mediaDropWidgetView3LeftBottom, 1, 0);
	layout3->addWidget(mediaDropWidgetView3RightBottom, 1, 1);



	//工作区配置
	QMenu* workSpaceMenu = new QMenu("WorkSpace", this);
	projectAction = new QAction("工程项目", this);
	projectAction->setCheckable(true);
	configAction = new QAction("工程配置", this);
	configAction->setCheckable(true);
	dataAction = new QAction("训练数据", this);
	dataAction->setCheckable(true);
	evaluationAction = new QAction("训练评价", this);
	evaluationAction->setCheckable(true);
	displayAction = new QAction("数据展示", this);
	displayAction->setCheckable(true);
	workSpaceMenu->addAction(projectAction);
	workSpaceMenu->addAction(configAction);
	workSpaceMenu->addAction(dataAction);
	workSpaceMenu->addAction(evaluationAction);
	workSpaceMenu->addAction(displayAction);
	ui->actionWorkSpace->setMenu(workSpaceMenu);
	projectAction->setChecked(true);
	configAction->setChecked(true);
	dataAction->setChecked(true);
	evaluationAction->setChecked(true);
	displayAction->setChecked(true);

	connect(projectAction, &QAction::triggered, this, &MainWindow::onProjectActionTriggered);
	connect(configAction, &QAction::triggered, this, &MainWindow::onConfigActionTriggered);
	connect(dataAction, &QAction::triggered, this, &MainWindow::onDataActionTriggered);
	connect(evaluationAction, &QAction::triggered, this, &MainWindow::onEvaluationActionTriggered);
	connect(displayAction, &QAction::triggered, this, &MainWindow::onDisplayActionTriggered);



	//视图配置
	QMenu* viewArrangementMenu = new QMenu("View Arrangement", this);
	QActionGroup* viewArrangementGroup = new QActionGroup(this);
	viewArrangementGroup->setExclusive(true);
	fourSplitAction = new QAction("四分窗口", this);
	fourSplitAction->setCheckable(true);
	fourSplitAction->setChecked(true);
	twoSplitAction = new QAction("二分窗口", this);
	twoSplitAction->setCheckable(true);
	oneSplitAction = new QAction("一分窗口", this);
	oneSplitAction->setCheckable(true);
	viewArrangementGroup->addAction(fourSplitAction);
	viewArrangementGroup->addAction(twoSplitAction);
	viewArrangementGroup->addAction(oneSplitAction);
	viewArrangementMenu->addAction(fourSplitAction);
	viewArrangementMenu->addAction(twoSplitAction);
	viewArrangementMenu->addAction(oneSplitAction);
	ui->actionViewArrangement->setMenu(viewArrangementMenu);

	connect(fourSplitAction, &QAction::triggered, this, &MainWindow::onFourSplitActionTriggered);
	connect(twoSplitAction, &QAction::triggered, this, &MainWindow::onTwoSplitActionTriggered);
	connect(oneSplitAction, &QAction::triggered, this, &MainWindow::onOneSplitActionTriggered);
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabWidgetCurrentChanged);


	//test
	on_actionOpen_triggered();



}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::updatePanel()
{
	if (ProjectDataManager::projectCurrentIndex < 0) {
		ui->treeWidgetProjectDirectory->clear();
		ui->treeWidgetTainData->clear();
		ui->treeWidgetTrainEvaluation->clear();
		ui->treeWidgetProjectConfiguration->clear();
		ui->treeWidgetDataDisplay->clear();
		return;
	}

	//更新工程项目树
	ui->treeWidgetProjectDirectory->clear();
	for (unsigned int i = 0; i < ProjectDataManager::getProjectList().size(); ++i) {
		QTreeWidgetItem* projectItem = new QTreeWidgetItem(ui->treeWidgetProjectDirectory);
		projectItem->setText(0, QString::fromStdString(ProjectDataManager::getProjectList()[i].projectName));
		ui->treeWidgetProjectDirectory->addTopLevelItem(projectItem);
	}
	ui->treeWidgetProjectDirectory->setCurrentItem(ui->treeWidgetProjectDirectory->topLevelItem(static_cast<int>(ProjectDataManager::projectCurrentIndex)));

	//更新训练数据树
	ui->treeWidgetTainData->clear();
	projectUnit project = ProjectDataManager::getProjectList()[ProjectDataManager::projectCurrentIndex];
	DirectoryTree projectTree = project.projectTree;

	TreeNode* videoNode = projectTree.findNode(projectTree.getRoot(), "视频");
	if (videoNode != nullptr) {
		QTreeWidgetItem* videoItem = new QTreeWidgetItem(ui->treeWidgetTainData);
		videoItem->setText(0, "训练视频");
		ui->treeWidgetTainData->addTopLevelItem(videoItem);

		size_t videoNodeChildrenCount = videoNode->children.size();
		for (int j = 0; j < videoNodeChildrenCount; ++j) {
			TreeNode* videoNodeChild = videoNode->children[j];
			QTreeWidgetItem* videoSubItem = new QTreeWidgetItem(videoItem);
			videoSubItem->setText(0, QString::fromStdString(videoNodeChild->value));
			videoItem->addChild(videoSubItem);
		}
	}

	TreeNode* audioNode = projectTree.findNode(projectTree.getRoot(), "音频");
	if (audioNode != nullptr) {
		QTreeWidgetItem* audioItem = new QTreeWidgetItem(ui->treeWidgetTainData);
		audioItem->setText(0, "训练音频");
		ui->treeWidgetTainData->addTopLevelItem(audioItem);

		size_t audioNodeChildrenCount = audioNode->children.size();
		for (int j = 0; j < audioNodeChildrenCount; ++j) {
			TreeNode* audioNodeChild = audioNode->children[j];
			QTreeWidgetItem* audioSubItem = new QTreeWidgetItem(audioItem);
			audioSubItem->setText(0, QString::fromStdString(audioNodeChild->value));
			audioItem->addChild(audioSubItem);
		}
	}

	TreeNode* flightParameterNode = projectTree.findNode(projectTree.getRoot(), "飞行参数数据");
	if (flightParameterNode != nullptr) {
		QTreeWidgetItem* flightParameterItem = new QTreeWidgetItem(ui->treeWidgetTainData);
		flightParameterItem->setText(0, "飞行参数数据");
		ui->treeWidgetTainData->addTopLevelItem(flightParameterItem);

		size_t flightParameterNodeChildrenCount = flightParameterNode->children.size();
		for (int j = 0; j < flightParameterNodeChildrenCount; ++j) {
			TreeNode* flightParameterNodeChild = flightParameterNode->children[j];
			QTreeWidgetItem* flightParameterSubItem = new QTreeWidgetItem(flightParameterItem);
			flightParameterSubItem->setText(0, QString::fromStdString(flightParameterNodeChild->value));
			flightParameterItem->addChild(flightParameterSubItem);
		}
	}

	TreeNode* eventAndTimeNode = projectTree.findNode(projectTree.getRoot(), "事件及其时间信息");
	if (eventAndTimeNode != nullptr) {
		QTreeWidgetItem* eventAndTimeItem = new QTreeWidgetItem(ui->treeWidgetTainData);
		eventAndTimeItem->setText(0, "事件及其时间信息");
		ui->treeWidgetTainData->addTopLevelItem(eventAndTimeItem);

		size_t eventAndTimeNodeChildrenCount = eventAndTimeNode->children.size();
		for (int j = 0; j < eventAndTimeNodeChildrenCount; ++j) {
			TreeNode* eventAndTimeNodeChild = eventAndTimeNode->children[j];
			QTreeWidgetItem* eventAndTimeSubItem = new QTreeWidgetItem(eventAndTimeItem);
			eventAndTimeSubItem->setText(0, QString::fromStdString(eventAndTimeNodeChild->value));
			eventAndTimeItem->addChild(eventAndTimeSubItem);
		}
	}



	//更新训练评价树
	ui->treeWidgetTrainEvaluation->clear();

	TreeNode* reportDocumentNode = projectTree.findNode(projectTree.getRoot(), "报告文档");
	if (reportDocumentNode != nullptr) {
		QTreeWidgetItem* reportDocument = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
		reportDocument->setText(0, "报告文档");
		ui->treeWidgetTrainEvaluation->addTopLevelItem(reportDocument);

		size_t reportDocumentNodeChildrenCount = reportDocumentNode->children.size();
		for (int j = 0; j < reportDocumentNodeChildrenCount; ++j) {
			TreeNode* reportDocumentNodeChild = reportDocumentNode->children[j];
			QTreeWidgetItem* reportDocumentSubItem = new QTreeWidgetItem(reportDocument);
			reportDocumentSubItem->setText(0, QString::fromStdString(reportDocumentNodeChild->value));
			reportDocument->addChild(reportDocumentSubItem);
		}
	}


	TreeNode* scoreDataNode = projectTree.findNode(projectTree.getRoot(), "成绩数据");
	if (scoreDataNode != nullptr) {
		QTreeWidgetItem* scoreData = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
		scoreData->setText(0, "成绩数据");

		ui->treeWidgetTrainEvaluation->addTopLevelItem(scoreData);
		size_t scoreDataNodeChildrenCount = scoreDataNode->children.size();
		for (int j = 0; j < scoreDataNodeChildrenCount; ++j) {
			TreeNode* scoreDataNodeChild = scoreDataNode->children[j];
			QTreeWidgetItem* scoreDataSubItem = new QTreeWidgetItem(scoreData);
			scoreDataSubItem->setText(0, QString::fromStdString(scoreDataNodeChild->value));
			scoreData->addChild(scoreDataSubItem);
		}
	}


	TreeNode* imageNode = projectTree.findNode(projectTree.getRoot(), "图片");
	if (imageNode != nullptr) {
		QTreeWidgetItem* image = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
		image->setText(0, "图片");
		ui->treeWidgetTrainEvaluation->addTopLevelItem(image);

		size_t imageNodeChildrenCount = imageNode->children.size();
		for (int j = 0; j < imageNodeChildrenCount; ++j) {
			TreeNode* imageNodeChild = imageNode->children[j];
			QTreeWidgetItem* imageSubItem = new QTreeWidgetItem(image);
			imageSubItem->setText(0, QString::fromStdString(imageNodeChild->value));
			image->addChild(imageSubItem);
		}
	}


	//更新工程配置





	//更新数据展示




}

void createDirectoriesFromJson(const nlohmann::json& j, const QString& basePath) {
	json projectTree = j["projectTree"];
	for (auto& topLevelElement : projectTree.items()) {
		fs::path topLevelPath = fs::path(basePath.toStdWString()) / fs::u8path(topLevelElement.key());
		fs::create_directories(topLevelPath);
		for (auto& subElement : topLevelElement.value().items()) {
			fs::path subPath = topLevelPath / fs::u8path(subElement.key());
			fs::create_directories(subPath);
		}
	}

	QString projectName = QString::fromStdString(j["projectName"].get<std::string>());
	std::wstring fileNameW = (projectName + ".xljp").toStdWString();
	std::filesystem::path filePath = std::filesystem::path(basePath.toStdWString()) / fileNameW;
	std::ofstream file(filePath, std::ios::out | std::ios::binary); 
	if (file.is_open()) {
		file << j.dump();
		file.close();
	}
	else {
		throw std::runtime_error("无法打开文件：" + filePath.string());
	}
}


void MainWindow::on_actionNew_triggered()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("选择新建工程的目录"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty()) {
		return;
	}
	QString projectName = QInputDialog::getText(this, "新建工程", "请输入工程名称");
	if (projectName.isEmpty()) {
		return;
	}

	json jsonProjectTree = {
	{"训练数据", {
		{"视频", {}},
		{"音频", {}},
		{"飞行参数数据", {}},
		{"事件及其时间信息", {}}
	}},
	{"训练评价", {
		{"报告文档", {}},
		{"成绩数据", {}},
		{"图片", {}}
	}}
	};

	json j;
	j["projectName"] = projectName.toStdString();
	j["airplaneNumber"] = "0001";
	j["readWritePermission"] = "rw";
	j["applicableTerminals"] = "pc";
	j["softwareVersion"] = "1.0.0";
	j["databaseVersion"] = "1.0.0";
	j["projectTree"] = jsonProjectTree;
	j["projectPath"] = dir.toStdString();

	projectDataManager->addProject(j, dir);
	ProjectDataManager::projectCurrentIndex = (int)ProjectDataManager::getProjectList().size() - 1;
	updatePanel();
	ui->treeWidgetProjectDirectory->setCurrentItem(ui->treeWidgetProjectDirectory->topLevelItem(static_cast<int>(ProjectDataManager::projectCurrentIndex)));
	createDirectoriesFromJson(j, dir);
}

void MainWindow::on_actionOpen_triggered()
{
	//test
	//QString fileName = QFileDialog::getOpenFileName(this, "Open .xljp File", "", "Xljp Files (*.xljp)");
	QString fileName = QString("C:/Users/Aiopr/Desktop/工程/测试.xljp");

	if (fileName.isEmpty()) {
		return;
	}
	QFile file(fileName);
	json doc;

	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		QString firstLine = in.readLine();
		file.close();

		try {
			doc = json::parse(firstLine.toStdString());
		}
		catch (json::parse_error&) {
			QMessageBox::warning(this, "警告", "JSON格式不合法！");
			return;
		}
	}
	else {
		QMessageBox::warning(this, "警告", "打开文件失败!");
		return;
	}


	json projectTree = doc["projectTree"];
	QString projectPath = fileName.left(fileName.lastIndexOf("/"));
	if (!isProjectTreeFilesExist(projectPath, projectTree)) {
		return;
	}

	projectDataManager->addProject(doc, projectPath);
	ProjectDataManager::projectCurrentIndex = (int)ProjectDataManager::getProjectList().size() - 1;
	updatePanel();
	ui->treeWidgetProjectDirectory->setCurrentItem(ui->treeWidgetProjectDirectory->topLevelItem(static_cast<int>(ProjectDataManager::projectCurrentIndex)));

}


bool MainWindow::isProjectTreeFilesExist(const QString& base_path, const json& structure) {
	for (auto& el : structure.items()) {
		QString key = QString::fromUtf8(el.key().c_str());
		QString current_path = QDir(base_path).filePath(key);

		if (!QFileInfo::exists(current_path)) {
			QString message = "文件不存在: " + key;
			QMessageBox::warning(this, "警告", message);
			return false;
		}
		if (el.value().is_object()) {
			if (!isProjectTreeFilesExist(current_path, el.value())) {
				return false;
			}
		}
	}
	return true;
}



void MainWindow::on_actionClose_triggered()
{
	on_actionSave_triggered();
	ProjectDataManager::removeProject(ProjectDataManager::getProjectList()[ProjectDataManager::projectCurrentIndex].projectName);
	ProjectDataManager::projectCurrentIndex -= 1;
	updatePanel();
}

void MainWindow::onProjectActionTriggered(bool checked)
{
	if (checked) {
		ui->treeWidgetProjectDirectory->show();
	}
	else {
		ui->treeWidgetProjectDirectory->hide();
	}
}

void MainWindow::onConfigActionTriggered(bool checked)
{
	if (checked) {
		ui->treeWidgetProjectConfiguration->show();
	}
	else {
		ui->treeWidgetProjectConfiguration->hide();
	}
}

void MainWindow::onDataActionTriggered(bool checked)
{
	if (checked) {
		ui->treeWidgetTainData->show();
	}
	else {
		ui->treeWidgetTainData->hide();
	}
}

void MainWindow::onEvaluationActionTriggered(bool checked)
{
	if (checked) {
		ui->treeWidgetTrainEvaluation->show();
	}
	else {
		ui->treeWidgetTrainEvaluation->hide();
	}
}

void MainWindow::onDisplayActionTriggered(bool checked)
{
	if (checked) {
		ui->treeWidgetDataDisplay->show();
	}
	else {
		ui->treeWidgetDataDisplay->hide();
	}
}

void MainWindow::onFourSplitActionTriggered()
{
	int currentIndex = ui->tabWidget->currentIndex();
	if (currentIndex == 0) {
		mediaDropWidgetView1LeftTop->show();
		mediaDropWidgetView1RightTop->show();
		mediaDropWidgetView1LeftBottom->show();
		mediaDropWidgetView1RightBottom->show();
	}
	else if (currentIndex == 1) {
		mediaDropWidgetView2LeftTop->show();
		mediaDropWidgetView2RightTop->show();
		mediaDropWidgetView2LeftBottom->show();
		mediaDropWidgetView2RightBottom->show();
	}
	else if (currentIndex == 2) {
		mediaDropWidgetView3LeftTop->show();
		mediaDropWidgetView3RightTop->show();
		mediaDropWidgetView3LeftBottom->show();
		mediaDropWidgetView3RightBottom->show();
	}

}

void MainWindow::onTwoSplitActionTriggered()
{
	int currentIndex = ui->tabWidget->currentIndex();
	if (currentIndex == 0) {
		mediaDropWidgetView1LeftTop->show();
		mediaDropWidgetView1RightTop->show();
		mediaDropWidgetView1LeftBottom->hide();
		mediaDropWidgetView1RightBottom->hide();
	}
	else if (currentIndex == 1) {
		mediaDropWidgetView2LeftTop->show();
		mediaDropWidgetView2RightTop->show();
		mediaDropWidgetView2LeftBottom->hide();
		mediaDropWidgetView2RightBottom->hide();
	}
	else if (currentIndex == 2) {
		mediaDropWidgetView3LeftTop->show();
		mediaDropWidgetView3RightTop->show();
		mediaDropWidgetView3LeftBottom->hide();
		mediaDropWidgetView3RightBottom->hide();
	}
}

void MainWindow::onOneSplitActionTriggered()
{
	int currentIndex = ui->tabWidget->currentIndex();
	if (currentIndex == 0) {
		mediaDropWidgetView1LeftTop->show();
		mediaDropWidgetView1RightTop->hide();
		mediaDropWidgetView1LeftBottom->hide();
		mediaDropWidgetView1RightBottom->hide();
	}
	else if (currentIndex == 1) {
		mediaDropWidgetView2LeftTop->show();
		mediaDropWidgetView2RightTop->hide();
		mediaDropWidgetView2LeftBottom->hide();
		mediaDropWidgetView2RightBottom->hide();
	}
	else if (currentIndex == 2) {
		mediaDropWidgetView3LeftTop->show();
		mediaDropWidgetView3RightTop->hide();
		mediaDropWidgetView3LeftBottom->hide();
		mediaDropWidgetView3RightBottom->hide();
	}
}

void MainWindow::onTabWidgetCurrentChanged(int index)
{
	if (index == 0) {
		int count = 0;
		if (mediaDropWidgetView1LeftTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView1RightTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView1LeftBottom->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView1RightBottom->isVisible()) {
			count++;
		}

		if (count == 4) {
			fourSplitAction->setChecked(true);
		}
		if (count == 2) {
			twoSplitAction->setChecked(true);
		}
		if (count == 1) {
			oneSplitAction->setChecked(true);
		}
	}
	else if (index == 1) {
		int count = 0;
		if (mediaDropWidgetView2LeftTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView2RightTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView2LeftBottom->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView2RightBottom->isVisible()) {
			count++;
		}

		if (count == 4) {
			fourSplitAction->setChecked(true);
		}
		if (count == 2) {
			twoSplitAction->setChecked(true);
		}
		if (count == 1) {
			oneSplitAction->setChecked(true);
		}
	}
	else if (index == 2) {
		int count = 0;
		if (mediaDropWidgetView3LeftTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView3RightTop->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView3LeftBottom->isVisible()) {
			count++;
		}
		if (mediaDropWidgetView3RightBottom->isVisible()) {
			count++;
		}

		if (count == 4) {
			fourSplitAction->setChecked(true);
		}
		if (count == 2) {
			twoSplitAction->setChecked(true);
		}
		if (count == 1) {
			oneSplitAction->setChecked(true);
		}
	}
}





void MainWindow::on_treeWidgetProjectDirectory_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	Q_UNUSED(previous);

	int index = ui->treeWidgetProjectDirectory->indexOfTopLevelItem(current);
	if (index >= 0) {
		ProjectDataManager::projectCurrentIndex = index;


		//更新训练数据树
		ui->treeWidgetTainData->clear();
		projectUnit project = ProjectDataManager::getProjectList()[ProjectDataManager::projectCurrentIndex];
		DirectoryTree projectTree = project.projectTree;


		TreeNode* videoNode = projectTree.findNode(projectTree.getRoot(), "视频");
		if (videoNode != nullptr) {
			QTreeWidgetItem* videoItem = new QTreeWidgetItem(ui->treeWidgetTainData);
			videoItem->setText(0, "训练视频");
			ui->treeWidgetTainData->addTopLevelItem(videoItem);

			size_t videoNodeChildrenCount = videoNode->children.size();
			for (int j = 0; j < videoNodeChildrenCount; ++j) {
				TreeNode* videoNodeChild = videoNode->children[j];
				QTreeWidgetItem* videoSubItem = new QTreeWidgetItem(videoItem);
				videoSubItem->setText(0, QString::fromStdString(videoNodeChild->value));
				videoItem->addChild(videoSubItem);
			}
		}



		TreeNode* audioNode = projectTree.findNode(projectTree.getRoot(), "音频");
		if (audioNode != nullptr) {
			QTreeWidgetItem* audioItem = new QTreeWidgetItem(ui->treeWidgetTainData);
			audioItem->setText(0, "训练音频");
			ui->treeWidgetTainData->addTopLevelItem(audioItem);

			size_t audioNodeChildrenCount = audioNode->children.size();
			for (int j = 0; j < audioNodeChildrenCount; ++j) {
				TreeNode* audioNodeChild = audioNode->children[j];
				QTreeWidgetItem* audioSubItem = new QTreeWidgetItem(audioItem);
				audioSubItem->setText(0, QString::fromStdString(audioNodeChild->value));
				audioItem->addChild(audioSubItem);
			}
		}



		TreeNode* flightParameterNode = projectTree.findNode(projectTree.getRoot(), "飞行参数数据");
		if (flightParameterNode != nullptr) {
			QTreeWidgetItem* flightParameterItem = new QTreeWidgetItem(ui->treeWidgetTainData);
			flightParameterItem->setText(0, "飞行参数数据");
			ui->treeWidgetTainData->addTopLevelItem(flightParameterItem);

			size_t flightParameterNodeChildrenCount = flightParameterNode->children.size();
			for (int j = 0; j < flightParameterNodeChildrenCount; ++j) {
				TreeNode* flightParameterNodeChild = flightParameterNode->children[j];
				QTreeWidgetItem* flightParameterSubItem = new QTreeWidgetItem(flightParameterItem);
				flightParameterSubItem->setText(0, QString::fromStdString(flightParameterNodeChild->value));
				flightParameterItem->addChild(flightParameterSubItem);
			}
		}



		TreeNode* eventAndTimeNode = projectTree.findNode(projectTree.getRoot(), "事件及其时间信息");
		if (eventAndTimeNode != nullptr) {
			QTreeWidgetItem* eventAndTimeItem = new QTreeWidgetItem(ui->treeWidgetTainData);
			eventAndTimeItem->setText(0, "事件及其时间信息");
			ui->treeWidgetTainData->addTopLevelItem(eventAndTimeItem);

			size_t eventAndTimeNodeChildrenCount = eventAndTimeNode->children.size();
			for (int j = 0; j < eventAndTimeNodeChildrenCount; ++j) {
				TreeNode* eventAndTimeNodeChild = eventAndTimeNode->children[j];
				QTreeWidgetItem* eventAndTimeSubItem = new QTreeWidgetItem(eventAndTimeItem);
				eventAndTimeSubItem->setText(0, QString::fromStdString(eventAndTimeNodeChild->value));
				eventAndTimeItem->addChild(eventAndTimeSubItem);
			}
		}



		//更新训练评价树
		ui->treeWidgetTrainEvaluation->clear();

		TreeNode* reportDocumentNode = projectTree.findNode(projectTree.getRoot(), "报告文档");
		if (reportDocumentNode != nullptr) {
			QTreeWidgetItem* reportDocument = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
			reportDocument->setText(0, "报告文档");
			ui->treeWidgetTrainEvaluation->addTopLevelItem(reportDocument);

			size_t reportDocumentNodeChildrenCount = reportDocumentNode->children.size();
			for (int j = 0; j < reportDocumentNodeChildrenCount; ++j) {
				TreeNode* reportDocumentNodeChild = reportDocumentNode->children[j];
				QTreeWidgetItem* reportDocumentSubItem = new QTreeWidgetItem(reportDocument);
				reportDocumentSubItem->setText(0, QString::fromStdString(reportDocumentNodeChild->value));
				reportDocument->addChild(reportDocumentSubItem);
			}
		}


		TreeNode* scoreDataNode = projectTree.findNode(projectTree.getRoot(), "成绩数据");
		if (scoreDataNode != nullptr) {
			QTreeWidgetItem* scoreData = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
			scoreData->setText(0, "成绩数据");

			ui->treeWidgetTrainEvaluation->addTopLevelItem(scoreData);
			size_t scoreDataNodeChildrenCount = scoreDataNode->children.size();
			for (int j = 0; j < scoreDataNodeChildrenCount; ++j) {
				TreeNode* scoreDataNodeChild = scoreDataNode->children[j];
				QTreeWidgetItem* scoreDataSubItem = new QTreeWidgetItem(scoreData);
				scoreDataSubItem->setText(0, QString::fromStdString(scoreDataNodeChild->value));
				scoreData->addChild(scoreDataSubItem);
			}
		}


		TreeNode* imageNode = projectTree.findNode(projectTree.getRoot(), "图片");
		if (imageNode != nullptr) {
			QTreeWidgetItem* image = new QTreeWidgetItem(ui->treeWidgetTrainEvaluation);
			image->setText(0, "图片");
			ui->treeWidgetTrainEvaluation->addTopLevelItem(image);

			size_t imageNodeChildrenCount = imageNode->children.size();
			for (int j = 0; j < imageNodeChildrenCount; ++j) {
				TreeNode* imageNodeChild = imageNode->children[j];
				QTreeWidgetItem* imageSubItem = new QTreeWidgetItem(image);
				imageSubItem->setText(0, QString::fromStdString(imageNodeChild->value));
				image->addChild(imageSubItem);
			}
		}


		//更新工程配置





		//更新数据展示





	}
}

bool saveCurrentProject() {
	std::string projectName = ProjectDataManager::getProject(ProjectDataManager::projectCurrentIndex).projectName;
	json j = ProjectDataManager::toJson(projectName);
	std::string path = ProjectDataManager::getProject(ProjectDataManager::projectCurrentIndex).projectPath;
	path += "/" + projectName + ".xljp";
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wPath = converter.from_bytes(path);
	std::ofstream file(wPath);
	file << j.dump();
	file.close();
	return true;
}

bool saveAllProject() {
	std::vector< projectUnit> projectList = ProjectDataManager::getProjectList();
	for (size_t i = 0; i < projectList.size(); ++i) {
		json j = ProjectDataManager::toJson(i);
		std::string path = projectList[i].projectPath;
		path += "/" + projectList[i].projectName + ".xljp";
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wPath = converter.from_bytes(path);
		std::ofstream file(wPath);
		file << j.dump();
		file.close();
	}
	return true;
}


void MainWindow::on_actionSave_triggered()
{
	std::vector< projectUnit> projectList = ProjectDataManager::getProjectList();

	if (projectList.size() == 0) {
		QMessageBox::warning(this, "警告", "没有打开的工程!");
		return;
	}

	if (projectList.size() == 1) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "保存工程", "您想要保存对当前工程的更改吗？", QMessageBox::Yes | QMessageBox::Cancel);
		if (reply == QMessageBox::Yes) {
			saveCurrentProject();
			QMessageBox::information(this, "提示", "保存成功!");
		}
		else if (reply == QMessageBox::Cancel) {
			return;
		}
	}
	else {
		QMessageBox msgBox;
		msgBox.setWindowTitle("保存工程");
		msgBox.setText("您想要保存当前工程还是所有工程？");
		msgBox.setIcon(QMessageBox::Question);
		QAbstractButton* saveCurrentButton = msgBox.addButton("当前工程", QMessageBox::YesRole);
		QAbstractButton* saveAllButton = msgBox.addButton("所有工程", QMessageBox::NoRole);
		QAbstractButton* cancelButton = msgBox.addButton("取消", QMessageBox::RejectRole);
		msgBox.exec();
		if (msgBox.clickedButton() == saveCurrentButton) {
			saveCurrentProject();
			QMessageBox::information(this, "提示", "保存成功!");
		}
		else if (msgBox.clickedButton() == saveAllButton) {
			saveAllProject();
		}
		else if (msgBox.clickedButton() == cancelButton) {
			return;
		}
	}
}


void MainWindow::on_actionDelete_triggered()
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "删除工程", "当前工程删除后不可恢复，您确定要删除吗？", QMessageBox::Yes | QMessageBox::Cancel);
	if (reply == QMessageBox::Yes) {
		std::string projectPath = ProjectDataManager::getProject(ProjectDataManager::projectCurrentIndex).projectPath;
		try {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wPath = converter.from_bytes(projectPath);
			std::filesystem::remove_all(wPath);
			ProjectDataManager::removeProject(ProjectDataManager::getProjectList()[ProjectDataManager::projectCurrentIndex].projectName);
			ProjectDataManager::projectCurrentIndex = (int)ProjectDataManager::getProjectList().size() - 1;
			updatePanel();
		}
		catch (const std::filesystem::filesystem_error&) {
			QMessageBox::critical(this, "错误", "删除工程文件时发生错误。");
		}
	}
	else if (reply == QMessageBox::Cancel) {
		return;
	}
}


void MainWindow::on_actionSaveAs_triggered()
{

}


void MainWindow::on_actionImport_triggered()
{

}


void MainWindow::on_actionExport_triggered()
{

}


void MainWindow::on_actionExit_triggered()
{

}


void MainWindow::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton reply = QMessageBox::question(this, "退出确认", "是否保存所有项目？",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	switch (reply) {
	case QMessageBox::Yes:
		saveAllProject();
		event->accept(); 
		break;
	case QMessageBox::No:
		event->accept(); 
		break;
	case QMessageBox::Cancel:
		event->ignore(); 
		break;
	default:
		event->ignore(); 
		break;
	}
}
