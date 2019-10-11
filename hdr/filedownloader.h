#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <qt5/QtCore/QObject>
#include <qt5/QtCore/QByteArray>
#include <qt5/QtNetwork/QNetworkAccessManager>
#include <qt5/QtNetwork/QNetworkRequest>
#include <qt5/QtNetwork/QNetworkReply>

class FileDownloader : public QObject
{
 Q_OBJECT

public:
 explicit FileDownloader(QUrl imageUrl, QObject *parent);
 virtual ~FileDownloader();
 QByteArray downloadedData() const;

signals:
 void downloaded();

private slots:
 void fileDownloaded(QNetworkReply* pReply);

private:
 QNetworkAccessManager m_WebCtrl;
 QByteArray m_DownloadedData;
};

#endif // FILEDOWNLOADER_H
