#include <state/globals.h>
#include <objects/PDFobject.h>
#include <curl/curl.h>

void PDFobject::parse(PDFobject *obj) {
    QPdfDocument *pdf = new QPdfDocument;
    pdf->load(obj->getPath());
    obj->determineType(pdf);
    appCache.writeOne(*obj);
}

PDFobject::PDFobject(QString path_in) : path(path_in) {
    QPdfDocument *pdf = new QPdfDocument;
    pdf->load(path);
    QFileInfo fileInfo(path);

    setData("Filename", fileInfo.fileName());
    setData("Pages", QString::number(pdf->pageCount()));
    setData("Date Added", QDateTime::currentDateTime().toString("MM-dd-yyyy"));
    setData("Date Modified", QDateTime::currentDateTime().toString("MM-dd-yyyy"));

}

PDFobject::PDFobject(QJsonObject json_in) {
    path = json_in["path"].toString();
    QJsonArray dataArr = json_in["data"].toArray();
    for (const QJsonValue &value : dataArr) {
        QStringList keyValue = value.toString().split('\t');
        if (keyValue.size() == 2)
            setData(keyValue[0], keyValue[1]);
        else if (keyValue.size() == 1)
            setData(keyValue[0], "");
    }
    parent.fromJSON(json_in["parent"].toObject());
}

void PDFobject::displayInfo(QTableWidget *tableWidget) {
    int row = 0;
    tableWidget->clear();
    for (auto it = data.begin(); it != data.end(); ++it) {
        QTableWidgetItem *keyItem = new QTableWidgetItem(it->first);
        QTableWidgetItem *valueItem = new QTableWidgetItem(it->second);

        // Make the key read-only
        keyItem->setFlags(keyItem->flags() & ~Qt::ItemIsEditable);

        tableWidget->setItem(row, 0, keyItem);  // Key Column
        tableWidget->setItem(row, 1, valueItem); // Editable Value Column
        row++;

    }

}

const string PDFobject::getISBN(const string &text) {
    int pos = text.find("ISBN");
    string isbn = "";
    if (pos != string::npos) {
        pos += 4;
        string curr = text.substr(pos, 1);
        while (regex_match(curr, regex("^[0-9 -:]$"))) {
            if (regex_match(curr, regex("^[0-9-]$")))
                isbn += curr;
            ++pos;
            curr = text.substr(pos, 1);
        }
    }
    isbn = std::regex_replace(isbn, std::regex("[^0-9]"), "");
    return isbn;
}

void PDFobject::determineType(QPdfDocument *pdf) {

    string isbn;
    // TODO: add other types besides book
    for (int i = 0; i < pagesToScan; i++) {
        string text = pdf->getAllText(i).text().toStdString();
        isbn = getISBN(text);
        if (isbn != "") {
            parent = BookObject(isbn);
            break;
        }
    }

    if (parent.type == DocType::Unwritten)
        parent.type = DocType::Null;

    delete pdf;
    pdf = nullptr;
}