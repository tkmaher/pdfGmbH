#include <state/globals.h>
#include <objects/PDFobject.h>
#include <curl/curl.h>

size_t BookObject::writeCallback(char *contents, size_t size, size_t nmemb, string *output) {
    size_t total_size = size * nmemb;
    output->append(contents, total_size);
    return total_size;
}

BookObject::BookObject(string isbn_in) {

    CURL *curl = curl_easy_init();

    type = DocType::Book;
    if (curl) {
        string readBuffer;

        string url = ISBNendpoint + isbn_in + ".json";

        qDebug() << "Fetching data from URL: " << QString::fromStdString(url);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        try {
            QJsonDocument json_data = QJsonDocument::fromJson(readBuffer.c_str());
            QJsonObject jData = json_data.object();
            QJsonObject records = jData["records"].toObject().begin().value().toObject(); // ugly

            QJsonDocument doc(records["data"].toObject());
            qDebug() << doc.toJson(QJsonDocument::Compact) << "test";

            if (records.contains("data")) {
                QJsonObject data = records["data"].toObject();
                setData("Title", data["title"].toString());
                setData("Subtitle", data["subtitle"].toString());
                parseCategory("authors", "name", "Author", data);
                parseCategory("series", "name", "Series", data);
                parseCategory("publishers", "name", "Publisher", data);
                parseCategory("publish_places", "name", "Publish location", data);
                setData("Year", data["publish_date"].toString());
                parseCategory("subjects", "name", "Subject", data);
            }
            if (records.contains("details")) {
                auto d1 = records["details"].toObject();
                if (d1.contains("details")) {   // BECAUSE OF THE STUPID OPENLIBRARY API
                    auto details = d1["details"].toObject();
                    parseCategory("languages", "key", "Languages", details);
                }
            }
        } catch (const std::exception &e) {
            cerr << "Error parsing JSON: " << e.what() << endl;
        }
        curl_easy_cleanup(curl);
    }

}

void ParentObject::parseCategory(const QString &key1, const QString &key2, 
                                 const QString &dataKey, const QJsonObject &data) {
    const QJsonArray arr = data[key1].toArray();
    QString str = "";
    for (auto &i : arr) {
        str += i.toObject()[key2].toString() + "\t";
    }
    setData(dataKey, str);
}

ParentObject::ParentObject() {
    type = DocType::Unwritten;
}

void ParentObject::fromJSON(const QJsonObject &json_in) {
    pData.clear();
    type = static_cast<DocType>(json_in["type"].toInt());
    if (type == DocType::Null)
        return;
    QJsonArray dataArr = json_in["data"].toArray();
    for (const QJsonValue &value : dataArr) {
        QStringList keyValue = value.toString().split('\t');
        if (keyValue.size() == 2)
            setData(keyValue[0], keyValue[1]);
        else if (keyValue.size() == 1)
            setData(keyValue[0], "");
    }
}

QJsonObject ParentObject::qJSONify() const {
    QJsonObject obj;
    obj["type"] = static_cast<int>(type);

    QJsonArray dataArr;
    for (const QPair<QString, QString> &j : pData)
        dataArr.append(j.first + '\t' + j.second);
        
    obj["data"] = dataArr;

    return obj;
}

const DocType ParentObject::getType() const {
    return type;
}

const QList<QPair<QString, QString>> ParentObject::getData() {
    return pData;
}

void ParentObject::setData(const QString &key, const QString &value) {
    for (int i = 0; i < pData.size(); ++i) {
        if (pData[i].first == key) {
            pData[i].second = value;
            return;
        }
    }
    pData.append(QPair<QString, QString>(key, value));
}