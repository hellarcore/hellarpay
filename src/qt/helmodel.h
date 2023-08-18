// Copyright (c) 2023 The Hellar developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HELMODEL_H
#define	HELMODEL_H

#include "heldb.h"

#include <QObject>

#include <QNetworkReply>

class QNetworkAccessManager;
class QResource;


/** Interface to Hellar DB from Qt view code. */
class HelModel : public QObject
{
    Q_OBJECT

public:
    explicit HelModel(CHelDB *heldb, QObject *parent = 0);
    ~HelModel();

    void fetch(); 
    void fetch_url(int _idx);
    bool saveResourceWithMD5();
    bool readResourceWithMD5();
    
private:
    enum ST {
        ST_INIT,
        ST_LOADING_RCC,
        ST_LOADING_MD5,
        ST_LOADED,
        ST_ERROR,
    };
    
    QString res_prefix;
    CHelDB *helDB;    
    QNetworkAccessManager* net_manager;
    QByteArray rccData;
    QString rccMD5;
    ST state;
    int try_idx;
    QString data_url;
    
Q_SIGNALS:
    void resourceReady(std::string res_root);
    

public Q_SLOTS:
    void replyFinished(QNetworkReply* p_reply);
    
private:
    void loadLocalResource();
    void loadFromDB();
    bool registerRes();
        
};

#endif	/* HELMODEL_H */

