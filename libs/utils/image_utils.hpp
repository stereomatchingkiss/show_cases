#pragma once

#include <QImage>
#include <QString>

#include <tuple>

class QDate;

namespace flt{

QImage decode_qbyte_array_to(QByteArray const &input, char const *image_type = nullptr);
QByteArray encode_qimage_to(QImage const &input, char const *image_type = "JPG");

/**
 * Save img as unique image based  on date and category.
 * The image will be save in the folder "database/images/date/category"
 * with unique name
 * @param date Date associate with the image, like the date of image created
 * @param img Image want to save
 * @param category Category of the image
 * @return Absolute path of the image if can save as unique image, else return
 * empty string
 */
QString save_as_unique_image(QImage const &img, QStringView folder, QDate const &date, QStringView category);

/**
 * Conver qimage to base 64 string
 * @param input Image want to convert to base 64 string
 * @return base 64 string of the image encoded as jpg
 */
QByteArray to_base64_img(QImage const &input);

QByteArray to_base64_img(QString const &input);

QByteArray to_base64_img_smaller_or_equal(QImage const &input, int bytes);

}
