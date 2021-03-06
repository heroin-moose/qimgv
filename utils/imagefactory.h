#ifndef IMAGEFACTORY_H
#define IMAGEFACTORY_H

#include "utils/imagelib.h"
#include "sourcecontainers/documentinfo.h"
#include "sourcecontainers/image.h"
#include "sourcecontainers/imageanimated.h"
#include "sourcecontainers/imagestatic.h"
#include "sourcecontainers/video.h"

class ImageFactory {
public:
    static std::shared_ptr<Image> createImage(QString path);
};

#endif // IMAGEFACTORY_H
