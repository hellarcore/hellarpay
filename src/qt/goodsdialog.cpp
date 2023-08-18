// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2023 The Hellar developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <iostream>

#include "walletmodel.h"
#include "helmodel.h"
#include "goodsdialog.h"
#include "ui_goodsdialog.h"

#include <QFile>
#include <QTextStream>
#include <QResource>
#include <QByteArray>
#include <QTextDocument>


/** Googs&Service page */
GoodsDialog::GoodsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoodsDialog),
    page_name(":/dev/html/goods.html")
{
    ui->setupUi(this);
}

GoodsDialog::~GoodsDialog()
{
    // TODO:
    //unsibscribeModelSignals();
    delete ui;
}

void GoodsDialog::setModel(WalletModel *model)
{
    this->model = model;
}

void GoodsDialog::setHelModel(HelModel *helModel)
{
    this->helModel = helModel;
    connect(helModel, SIGNAL(resourceReady(std::string)), 
            this, SLOT(on_resourceReady(std::string)));
    helModel->fetch();
}

void GoodsDialog::on_resourceReady(std::string res_root) 
{
    QString htmlContent;

    QFile  htmlFile(page_name);
    if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QTextStream in(&htmlFile);
    in.setCodec("UTF-8");
    htmlContent = in.readAll();

    QTextDocument *document = new QTextDocument();
    document->setHtml(htmlContent);
    ui->textBrowser->setDocument(document);    
}
