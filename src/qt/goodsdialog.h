// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2023 The Hellar developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOODSDIALOG_H
#define	GOODSDIALOG_H

#include "walletmodel.h"
#include "helmodel.h"

#include <QDialog>
#include <QObject>

namespace Ui {
    class GoodsDialog;
}

/** Dialog to show the HTML page with sales points that accept hellars */
class GoodsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoodsDialog(QWidget *parent=0);
    ~GoodsDialog();

    void setModel(WalletModel *model);
    void setHelModel(HelModel *helModel);
    
private:
    Ui::GoodsDialog *ui;
    QString header;
    QString coreOptions;
    QString uiOptions;
    
    WalletModel *model;
    HelModel *helModel;
    
    const QString page_name;
    
public Q_SLOTS:
    void on_resourceReady(std::string res_root);

};


#endif	/* GOODSDIALOG_H */

