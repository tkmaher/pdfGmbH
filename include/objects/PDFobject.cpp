#include <objects/PDFobject.h>
#include <curl/curl.h>

size_t BookObject::writeCallback(char *contents, size_t size, size_t nmemb, string *output) {
    size_t total_size = size * nmemb;
    output->append(contents, total_size);
    return total_size;
}

BookObject::BookObject(string isbn_in) : ISBN(isbn_in) {
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
            json_data = json_data["records"];
            if (json_data.contains("data")) {
                auto data = json_data["data"];
                if (data.contains("title")) 
                    title = data["title"];
                if (data.contains("subtitle")) 
                    subtitle = data["subtitle"];
                if (data.contains("authors")) 
                    for (auto &i : data["authors"])
                        authors.push_back(i["name"]);
                if (data.contains("series")) 
                    for (auto &i : data["series"])
                        series.push_back(i["name"]);
                if (data.contains("publishers")) 
                    for (auto &i : data["pubishers"])
                        publishers.push_back(i["name"]);
                if (data.contains("publish_places")) 
                    for (auto &i : data["publish_places"])
                        pubLocs.push_back(i["name"]);
                if (data.contains("publish_date")) 
                    pubYear = data["publish_date"];
                if (data.contains("subjects")) 
                    for (auto &i : data["subjects"])
                        subjects.push_back(i["name"]);
            }
            if (json_data.contains("details")) {
                auto d1 = json_data["details"];
                if (d1.contains("details")) {
                    auto details = d1["details"];
                    if (details.contains("languages")) 
                        for (auto &i : details["languages"])
                            langs.push_back(i["key"]);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            cerr << "JSON parse error: " << e.what() << endl;
            cerr << "Failed JSON content:\n" << readBuffer << endl;
        }
        curl_easy_cleanup(curl);
    }
}

PDFobject::PDFobject(QPdfDocument *pdf_in, QString path_in) : pdf(pdf_in), path(path_in) {
    data.append(QPair<QString, QString>("Title", pdf->metaData(QPdfDocument::MetaDataField::Title).toString()));
    data.append(QPair<QString, QString>("Author", pdf->metaData(QPdfDocument::MetaDataField::Author).toString()));
    data.append(QPair<QString, QString>("Keywords", pdf->metaData(QPdfDocument::MetaDataField::Keywords).toString()));
    data.append(QPair<QString, QString>("Subject", pdf->metaData(QPdfDocument::MetaDataField::Subject).toString()));
    data.append(QPair<QString, QString>("Pages", QString::number(pdf->pageCount())));
    data.append(QPair<QString, QString>("Date Added", QDateTime::currentDateTime().toString()));
    data.append(QPair<QString, QString>("Date Modified", QDateTime::currentDateTime().toString()));
    determineType(pdf);
}

void PDFobject::displayInfo(QTableWidget *tableWidget) {
    int row = 0;
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
    return isbn;
}

void PDFobject::determineType(QPdfDocument *pdf) {

    string isbn;

    for (int i = 0; i < pagesToScan; i++) {
        string text = pdf->getAllText(i).text().toStdString();
        isbn = getISBN(text);
        if (isbn != "") {
            qDebug() << isbn << "\n";
            parent = BookObject(isbn);
            break;
        }
    }
}