// Copied verbatim from Qt doumentation.
#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
  QObject(parent)
{
    // Set up cnnection to call-back function.
    connect(
     &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
    this, SLOT (fileDownloaded(QNetworkReply*))
        );
  QNetworkRequest request(imageUrl);
  m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    // Emit a signal to tell that image is available.
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    return m_DownloadedData;
}
