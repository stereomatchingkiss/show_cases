#include "image_utils.hpp"

#include <QBuffer>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QUuid>

namespace flt{

namespace{

QString create_unique_image_name(QStringView target_folder,
                                 QDate const &date,
                                 QStringView category)
{
    QString const folder = QString("%1/%2/%3").
            arg(target_folder.toString(), date.toString(QStringLiteral("yyyy.MM.dd")), category.toString());
    if(!QDir(folder).exists()){
        QDir().mkpath(folder);
    }

    QString unique_name = QStringLiteral("%1/%2.jpg").arg(folder, QUuid::createUuid().toString());
    do{
        if(!QFile::exists(unique_name)){
            break;
        }
        unique_name = QStringLiteral("%1/%2.jpg").arg(folder, QUuid::createUuid().toString());
    }while(true);

    return unique_name;
}

}

QImage decode_qbyte_array_to(const QByteArray &input, const char *image_type)
{
    QImage result;
    result.loadFromData(input, image_type);

    return result;
}

QByteArray encode_qimage_to(const QImage &input, const char *image_type)
{
    QByteArray ba;
    QBuffer bu(&ba);
    bu.open(QIODevice::WriteOnly);
    input.save(&bu, image_type);

    return ba;
}



QByteArray to_base64_img(QImage const &input)
{
    auto result = encode_qimage_to(input, "JPG");
    if(!result.isEmpty()){
        return result.toBase64();
    }else{
        qDebug()<<__func__<<"cannot convert to base64 imgage";
        return {};
    }
}

QByteArray to_base64_img(const QString &input)
{
    QImage img(input);
    if(!img.isNull()){
        return to_base64_img(img);
    }

    return {};
}

QByteArray to_base64_img_smaller_or_equal(const QImage &input, int bytes)
{
    auto input_temp = input;
    auto output = to_base64_img(input_temp);
    while(output.size() > bytes){
        input_temp = input_temp.scaledToWidth(input_temp.width() / 2, Qt::SmoothTransformation);
        output = to_base64_img(input_temp);
    }

    return output;
}

QString save_as_unique_image(QImage const &img, QStringView folder, QDate const &date, QStringView category)
{
    QString const unique_img_name = create_unique_image_name(folder, date, category);
    if(!img.isNull()){
        img.save(unique_img_name);
        return unique_img_name;
    }

    return {};
}

}
