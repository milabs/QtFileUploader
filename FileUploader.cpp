#include "FileUploader.hpp"

ChooseFileDlg::ChooseFileDlg(QWidget *parent)
	: QDialog(parent)
{
	new QVBoxLayout(this);

	QPushButton *upload = new QPushButton(tr("Choose file to upload..."), this);
	connect(upload, SIGNAL(clicked(bool)), this, SLOT(onFileSelect(bool)));
	layout()->addWidget(upload);

	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	buttons->setCenterButtons(true);
	connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
		done(QDialog::Accepted);
	});
	connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
		done(QDialog::Rejected);
	});
	layout()->addWidget(buttons);
}

void ChooseFileDlg::onFileSelect(bool)
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open File"), NULL, tr("All files *.*"));

	QFile *file = new QFile(path);
	if (!file->exists() || !file->open(QIODevice::ReadOnly))
		return;

	QProgressDialog progress(tr("Uploading..."), tr("Abort"), 0, 100, this);
	progress.setMinimumDuration(0);

	QHttpPart httpFilePart;
	httpFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file\"; filename=\"%1\"").arg(QFileInfo(path).fileName())));
	httpFilePart.setBodyDevice(file);

	QHttpMultiPart *httpMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	httpMultiPart->append(httpFilePart);

	QNetworkAccessManager manager;
	QNetworkReply *reply = manager.post(QNetworkRequest(QUrl("http://127.0.0.1:3000/media/upload")), httpMultiPart);
	connect(reply, &QNetworkReply::uploadProgress, [&](qint64 bytesSent, qint64 bytesTotal){
		if (bytesSent && bytesTotal) progress.setValue((100 * bytesSent) / bytesTotal);
	});

	file->setParent(httpMultiPart);
	httpMultiPart->setParent(reply);

	connect(&progress, &QProgressDialog::canceled, [&]() {
		reply->abort();
	});

	while (reply->isRunning()) {
		QCoreApplication::processEvents();
	}

	if (reply->error() != QNetworkReply::NoError) {
		if (reply->error() != QNetworkReply::OperationCanceledError)
			QMessageBox::warning(this, tr("Failed to upload"), reply->errorString());
		return;
	}

	QByteArray content = reply->readAll();
	qDebug() << content;
}

#include "moc_FileUploader.cpp"
