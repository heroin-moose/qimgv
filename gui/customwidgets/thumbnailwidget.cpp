#include "thumbnailwidget.h"

ThumbnailWidget::ThumbnailWidget(QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    state(EMPTY),
    thumbnail(nullptr),
    highlighted(false),
    hovered(false),
    mDrawLabel(true),
    thumbnailSize(100),
    marginY(3),
    marginX(1),
    textHeight(5)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    // ! this breaks HiDPI !
    //setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setAcceptHoverEvents(true);
    nameColor.setRgb(20, 20, 20, 255);
    qreal fntSz = font.pointSizeF();
    if(fntSz > 0) {
        font.setPointSizeF(font.pointSizeF() * 0.9);
        fontSmall.setPointSizeF(fontSmall.pointSizeF() * 0.85);
    }
    font.setBold(true);
    fontSmall.setBold(true);
    fm = new QFontMetrics(font);
    fmSmall = new QFontMetrics(fontSmall);
    textHeight = fm->height();

    setThumbnailSize(100);

    readSettings();
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(readSettings()));
}

void ThumbnailWidget::readSettings() {
    highlightColor.setRgb(settings->accentColor().rgb());
}

void ThumbnailWidget::setThumbnailSize(int size) {
    if(thumbnailSize != size && size > 0) {
        this->state = EMPTY;
        thumbnail = nullptr;
        thumbnailSize = size;
        updateGeometry();
        setupLayout();
        update();
    }
}

void ThumbnailWidget::setMargins(int x, int y) {
    marginX = x;
    marginY = y;
}

void ThumbnailWidget::setDrawLabel(bool mode) {
    if(mDrawLabel != mode) {
        mDrawLabel = mode;
        update();
    }
}

void ThumbnailWidget::updateGeometry() {
    QGraphicsWidget::updateGeometry();
}

void ThumbnailWidget::setGeometry(const QRectF &rect) {
    QGraphicsWidget::setGeometry(QRectF(rect.topLeft(), boundingRect().size()));
}

QRectF ThumbnailWidget::geometry() const {
    return QRectF(QGraphicsWidget::geometry().topLeft(), boundingRect().size());
}

QSizeF ThumbnailWidget::effectiveSizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    return sizeHint(which, constraint);
}

void ThumbnailWidget::setThumbnail(std::shared_ptr<Thumbnail> _thumbnail) {
    if(_thumbnail) {
        thumbnail = _thumbnail;
        setupLayout();
        updateThumbnailDrawPosition();
        update();
    }
}

void ThumbnailWidget::setupLayout() {
    highlightRect = QRectF(marginX, 0, width() - marginX * 2, marginY);
    nameRect = QRectF(highlightRect.left(), highlightRect.height(),
                      highlightRect.width(), textHeight * 1.7);

    if(thumbnail && !thumbnail->label().isEmpty()) {
        nameTextRect = nameRect.adjusted(4, 0, -4, 0);
        if(!thumbnail->label().isEmpty()) {
            labelTextRect.setWidth(fmSmall->width(thumbnail->label()));
            labelTextRect.setHeight(nameRect.height());
            labelTextRect.moveTop(nameTextRect.top());
            labelTextRect.moveRight(nameTextRect.right());
            nameTextRect.adjust(0, 0, -labelTextRect.width() - 4, 0);
        }
    }
}

void ThumbnailWidget::setHighlighted(bool mode, bool smooth) {
    if(highlighted != mode) {
        highlighted = mode;
        update();
    }
}

bool ThumbnailWidget::isHighlighted() {
    return highlighted;
}

QRectF ThumbnailWidget::boundingRect() const {
    return QRectF(0, 0, thumbnailSize + marginX * 2, thumbnailSize + marginY * 2);
}

qreal ThumbnailWidget::width() {
    return boundingRect().width();
}

qreal ThumbnailWidget::height() {
    return boundingRect().height();
}

void ThumbnailWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)
    Q_UNUSED(option)
    qreal dpr = painter->paintEngine()->paintDevice()->devicePixelRatioF();
    drawHighlight(painter);
    if(!thumbnail) {
        QPixmap* loadingIcon = shrRes->loadingIcon72();
        drawIcon(painter, dpr, loadingIcon);
    } else {
        if(thumbnail->pixmap().get()->width() == 0) {
            QPixmap* errorIcon = shrRes->loadingErrorIcon72();
            drawIcon(painter, dpr, errorIcon);
        } else {
            drawThumbnail(painter, dpr, thumbnail->pixmap().get());
        }
        if(mDrawLabel) {
            drawLabel(painter);
        }
    }
    drawHover(painter);
}

void ThumbnailWidget::drawHighlight(QPainter *painter) {
    if(isHighlighted()) {
        painter->fillRect(highlightRect, highlightColor);
    }
}

void ThumbnailWidget::drawHover(QPainter *painter) {
    if(isHovered()) {
        painter->setOpacity(0.6);
        painter->fillRect(highlightRect, highlightColor);
        painter->setOpacity(1.0);
        painter->fillRect(boundingRect(), QColor(255,255,255, 10));
    }
}

void ThumbnailWidget::drawLabel(QPainter *painter) {
    // text background
    painter->setOpacity(0.95);
    painter->fillRect(nameRect, nameColor);
    painter->setOpacity(1.0);
    // filename
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    painter->setFont(font);
    painter->setPen(QColor(230, 230, 230, 255));
    painter->drawText(nameTextRect, flags, thumbnail->name());
    // additional info
    painter->setFont(fontSmall);
    painter->setPen(QColor(160, 160, 160, 255));
    painter->drawText(labelTextRect, flags, thumbnail->label());
}

void ThumbnailWidget::drawThumbnail(QPainter* painter, qreal dpr, const QPixmap *pixmap) {
    painter->drawPixmap(drawPosCentered, *pixmap);
}

void ThumbnailWidget::drawIcon(QPainter* painter, qreal dpr, const QPixmap *pixmap) {
    QPointF pos = QPointF(width()  / 2 - pixmap->width()  / (2 * dpr),
                          height() / 2 - pixmap->height() / (2 * dpr));
    painter->drawPixmap(pos, *pixmap);
}

QSizeF ThumbnailWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
    Q_UNUSED(which);
    Q_UNUSED(constraint);

    return boundingRect().size();
}

void ThumbnailWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(true);
}

void ThumbnailWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    event->ignore();
    setHovered(false);
}

void ThumbnailWidget::setHovered(bool mode) {
    if(hovered != mode) {
        hovered = mode;
        update();
    }
}

bool ThumbnailWidget::isHovered() {
    return hovered;
}

void ThumbnailWidget::updateThumbnailDrawPosition() {
    if(thumbnail) {
        qreal dpr = qApp->devicePixelRatio();
        drawPosCentered = QPointF(width()  / 2 - thumbnail->pixmap()->width()  / (2 * dpr),
                                  height() / 2 - thumbnail->pixmap()->height() / (2 * dpr));
    }
}

ThumbnailWidget::~ThumbnailWidget() {
    delete fm;
    delete fmSmall;
}
