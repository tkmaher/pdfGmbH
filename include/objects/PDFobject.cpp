#include <objects/PDFobject.h>
#include <curl/curl.h>

size_t BookObject::writeCallback(char *contents, size_t size, size_t nmemb, string *output) {
    size_t total_size = size * nmemb;
    output->append(contents, total_size);
    return total_size;
}

void ParentObject::parseCategory(const QString &key1, const QString &key2, const QString &dataKey, const nlohmann::json &data) {
    for (auto &i : data[key1])
        setData(dataKey, to_string(i[key2]).c_str());
}

BookObject::BookObject(string isbn_in) {
    setData("Title", "");
    setData("Subtitle", "");
    setData("Author", "");
    setData("Series", "");
    setData("Publisher", "");
    setData("Publish location", "");
    setData("Year", "");
    setData("Subject", "");
    setData("Languages", "");
    setData("ISBN", isbn_in.c_str());

    CURL *curl = curl_easy_init();

    type = DocType::Book;
    if (curl) {
        string readBuffer;

        curl_easy_setopt(curl, CURLOPT_URL, (ISBNendpoint + isbn_in + ".json").c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        try {
            nlohmann::json json_data = nlohmann::json::parse(readBuffer);
            //qDebug() << json_data.dump(4).c_str();
            json_data = json_data["records"];
            if (json_data.contains("data")) {
                auto data = json_data["data"];
                if (data.contains("title")) 
                    setData("Title", to_string(data["title"]).c_str());
                if (data.contains("subtitle")) 
                    setData("Subtitle", to_string(data["subtitle"]).c_str());
                if (data.contains("authors"))
                    parseCategory("authors", "name", "Author", data);
                if (data.contains("series")) 
                    parseCategory("series", "name", "Series", data);
                if (data.contains("publishers")) 
                    parseCategory("publishers", "name", "Publisher", data);
                if (data.contains("publish_places")) 
                    parseCategory("publish_places", "name", "Publish location", data);
                if (data.contains("publish_date")) 
                    setData("Year", "publish_date");
                if (data.contains("subjects")) 
                    parseCategory("subjects", "name", "Subject", data);
            }
            if (json_data.contains("details")) {
                auto d1 = json_data["details"];
                if (d1.contains("details")) {   // BECAUSE OF THE STUPID OPENLIBRARY API
                    auto details = d1["details"];
                    if (details.contains("languages")) 
                        parseCategory("languages", "key", "Languages", details);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            cerr << "JSON parse error: " << e.what() << endl;
            cerr << "Failed JSON content:\n" << readBuffer << endl;
        }
        curl_easy_cleanup(curl);
    }

}

PDFobject::PDFobject(QString path_in) : path(path_in) {
    QPdfDocument *pdf = new QPdfDocument;
    pdf->load(path);

    setData("Title", pdf->metaData(QPdfDocument::MetaDataField::Title).toString());
    setData("Author", pdf->metaData(QPdfDocument::MetaDataField::Author).toString());
    setData("Keywords", pdf->metaData(QPdfDocument::MetaDataField::Keywords).toString());
    setData("Subject", pdf->metaData(QPdfDocument::MetaDataField::Subject).toString());
    setData("Pages", QString::number(pdf->pageCount()));
    setData("Date Added", QDateTime::currentDateTime().toString());
    setData("Date Modified", QDateTime::currentDateTime().toString());
    
    QFuture<void> future1 = QtConcurrent::run( handleParsing, this, pdf );

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

    for (int i = 0; i < pagesToScan; i++) {
        string text = pdf->getAllText(i).text().toStdString();
        isbn = getISBN(text);
        if (isbn != "") {
            //qDebug() << isbn << "\n";
            parent = BookObject(isbn);
            break;
        }
    }

    delete pdf;
    pdf = nullptr;
}