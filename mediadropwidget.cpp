#include "mediadropwidget.h"
#include<QAudioOutput>
#include <iostream>
#include <QMessageBox>

MediaDropWidget::MediaDropWidget(QWidget* parent)
	: QWidget(parent),
	videoPlayer(new QMediaPlayer(this)),
	audioPlayer(new QMediaPlayer(this)),
	videoWidget(new QVideoWidget(this)),
	imageLabel(new QLabel(this)),
	audioAudioOutput(new QAudioOutput(this)),
	videoAudioOutput(new QAudioOutput(this))
{
	setAcceptDrops(true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	videoWidget->setVisible(false);
	imageLabel->setVisible(false);
	imageLabel->setAlignment(Qt::AlignCenter);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(false);

	layout->addWidget(videoWidget);
	layout->addWidget(imageLabel);
	videoPlayer->setVideoOutput(videoWidget);
	setLayout(layout);

	//设置背景窗口为黑色
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::black);
	this->setAutoFillBackground(true);
	this->setPalette(palette);
	videoWidget->setStyleSheet("background-color: black;");
	imageLabel->setStyleSheet("background-color: black; color: white;");

	videoPlayer->setAudioOutput(videoAudioOutput);
	videoPlayer->setVideoOutput(videoWidget);

	audioPlayer->setAudioOutput(audioAudioOutput);

	//设置视频窗口为透明
	videoWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
}

void MediaDropWidget::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	}
}

void MediaDropWidget::dropEvent(QDropEvent* event) {
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();
		QString filePath = urlList.at(0).toLocalFile();
		QFileInfo fileInfo(filePath);
		QString extension = fileInfo.suffix().toLower();

		if (extension == "mp4" || extension == "avi" || extension == "mkv") {
			playVideo(filePath);
		}
		else if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "bmp") {
			showImage(filePath);
		}
		else if (extension == "mp3" || extension == "wav" || extension == "aac") {
			playAudio(filePath);
		}
		else {
			QMessageBox::warning(this, "警告", "不支持该文件格式的打开: " + extension);
		}
	}
}

void MediaDropWidget::playVideo(const QString& filePath) {
	audioPlayer->stop();
	videoWidget->setVisible(true);
	imageLabel->setVisible(false);
	videoPlayer->setSource(QUrl::fromLocalFile(filePath));
	videoPlayer->play();
}

void MediaDropWidget::showImage(const QString& filePath) {
	audioPlayer->stop();
	videoPlayer->stop();
	videoWidget->setVisible(false);
	imageLabel->setVisible(true);
	currentPixmap = QPixmap(filePath);
	adjustImageLabelSize();
}

void MediaDropWidget::adjustImageLabelSize() {
	if (!currentPixmap.isNull()) {
		QSize widgetSize = imageLabel->size();
		QSize pixmapSize = currentPixmap.size();
		pixmapSize.scale(widgetSize, Qt::KeepAspectRatio);
		imageLabel->setPixmap(currentPixmap.scaled(pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
}

void MediaDropWidget::playAudio(const QString& filePath) {
	videoPlayer->stop();
	videoWidget->setVisible(false);
	imageLabel->setVisible(true);
	imageLabel->setText("Playing audio: " + filePath);
	audioPlayer->setSource(QUrl::fromLocalFile(filePath));
	audioPlayer->play();
}

void MediaDropWidget::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	adjustImageLabelSize();
}



