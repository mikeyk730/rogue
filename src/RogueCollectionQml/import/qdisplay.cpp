#include <QColor>
#include <QRectF>
#include "qdisplay.h"
#include "dos_to_unicode.h"
#include "environment.h"
#include "qrogue.h"

namespace
{
    const int kMaxQueueSize = 0;

    uint32_t CharText(uint32_t ch)
    {
        return ch & 0x0000ffff;
    }

    uint32_t CharColor(uint32_t ch)
    {
        return (ch >> 24) & 0xff;
    }
}

namespace Colors
{
    QColor black()    { return QColor(   0,   0,   0, 255 ); }
    QColor white()    { return QColor( 255, 255, 255, 255 ); }
    QColor grey()     { return QColor( 170, 170, 170, 255 ); }
    QColor d_grey()   { return QColor(  65,  65,  65, 255 ); }
    QColor l_grey()   { return QColor( 205, 205, 205, 255 ); }
    QColor red()      { return QColor( 170,   0,   0, 255 ); }
    QColor l_red()    { return QColor( 255,  85,  85, 255 ); }
    QColor green()    { return QColor(   0, 170,   0, 255 ); }
    QColor l_green()  { return QColor(  85,  255, 85, 255 ); }
    QColor blue()     { return QColor(   0,   0, 170, 255 ); }
    QColor l_blue()   { return QColor(  85,  85, 255, 255 ); }
    QColor cyan()     { return QColor(   0, 170, 170, 255 ); }
    QColor l_cyan()   { return QColor(  25, 255, 255, 255 ); }
    QColor magenta()  { return QColor( 170,   0, 170, 255 ); }
    QColor l_magenta(){ return QColor( 255,  25, 255, 255 ); }
    QColor yellow()   { return QColor( 255, 255,  25, 255 ); }
    QColor brown()    { return QColor( 170,  85,   0, 255 ); }
    QColor orange()   { return QColor( 234, 118,   2, 255 ); }
}

QColor colors[] = {
    Colors::black(),
    Colors::blue(),
    Colors::green(),
    Colors::cyan(),
    Colors::red(),
    Colors::magenta(),
    Colors::brown(),
    Colors::grey(),
    Colors::d_grey(),
    Colors::l_blue(),
    Colors::l_green(),
    Colors::l_cyan(),
    Colors::l_red(),
    Colors::l_magenta(),
    Colors::yellow(),
    Colors::white()
};

QColor GetColor(int color)
{
    return colors[color];
}

QColor GetFg(int color)
{
    return GetColor(color & 0x0f);
}

QColor GetBg(int color)
{
    return GetColor(color >> 4);
}

QRogueDisplay::QRogueDisplay(QRogue* parent, Coord screen_size)
    : parent_(parent)
{
    screen_size_ = QSize(screen_size.x, screen_size.y);

    auto font = QFont("Px437 IBM VGA8");
    font.setPixelSize(32);
    font.setStyleStrategy(QFont::NoAntialias);

    SetFont(font);
}

QSize QRogueDisplay::ScreenSize() const
{
    return screen_size_;
}

QFont QRogueDisplay::Font() const
{
    return font_;
}

void QRogueDisplay::SetFont(const QFont &font)
{
    font_ = font;

    QFontMetrics font_metrics(font);
    font_size_.setWidth(font_metrics.width("W"));
    font_size_.setHeight(font_metrics.height());
}

QSize QRogueDisplay::FontSize() const
{
    return font_size_;
}

void QRogueDisplay::Render(QPainter *painter)
{ 
    std::unique_lock<std::mutex> lock(mutex_);

    if (!shared_.data)
        return;

    ThreadData copy(shared_);
    shared_.render_regions.clear();

    lock.unlock();

    //todo:
    copy.render_regions.clear();
    copy.render_regions.push_back(FullRegion());

    painter->setFont(font_);
    for (auto i = copy.render_regions.begin(); i != copy.render_regions.end(); ++i)
    {
        RenderRegion(painter, copy.data.get(), *i);
    }

//    if (show_cursor) {
//        RenderCursor(cursor_pos);
//    }

    //std::string counter;
    //if (input_ && input_->GetRenderText(&counter))
    //    RenderCounterOverlay(counter, 0);
}

void QRogueDisplay::paintChar(QPainter *painter, int x, int y, QChar ch, QColor fg, QColor bg)
{
    auto w = font_size_.width();
    auto h = font_size_.height();
    QRectF r(w*x, h*y, w, h);

    painter->fillRect(r, bg);
    painter->setPen(fg);
    painter->drawText(r, 0, ch);
}

void QRogueDisplay::RenderRegion(QPainter *painter, uint32_t *data, Region rect)
{
    for (int y = rect.Top; y <= rect.Bottom; ++y) {
        for (int x = rect.Left; x <= rect.Right; ++x) {
            uint32_t info = data[y*screen_size_.width() + x];
            int color = CharColor(info);
            int ch = CharText(info);
            ch = DosToUnicode(ch);
            //todo: --more-- standout hack
            paintChar(painter, x, y, ch, GetFg(color), GetBg(color));
        }
    }
}

void QRogueDisplay::PostRenderEvent()
{
    parent_->postRender();
}

void QRogueDisplay::SetDimensions(Coord dimensions)
{

}

void QRogueDisplay::UpdateRegion(uint32_t *buf)
{
    UpdateRegion(buf, FullRegion());
}

void QRogueDisplay::UpdateRegion(uint32_t *buf, Region rect)
{
    std::lock_guard<std::mutex> lock(mutex_);

    //If we're behind on rendering, clear the queue and do a single full render.
    if ((int)shared_.render_regions.size() > kMaxQueueSize)
    {
        shared_.render_regions.clear();
        shared_.render_regions.push_back(FullRegion());
    }
    else {
        shared_.render_regions.push_back(rect);
        PostRenderEvent();
    }

    if(!shared_.data) {
        shared_.data.reset(new uint32_t[TotalChars()]);
        shared_.dimensions = { screen_size_.width(), screen_size_.height() };
    }
    memcpy(shared_.data.get(), buf, TotalChars() * sizeof(int32_t));
}

void QRogueDisplay::MoveCursor(Coord pos)
{

}

void QRogueDisplay::SetCursor(bool enable)
{

}

Region QRogueDisplay::FullRegion() const
{
    Region r;
    r.Left = 0;
    r.Top = 0;
    r.Right = short(screen_size_.width() - 1);
    r.Bottom = short(screen_size_.height() - 1);
    return r;
}

int QRogueDisplay::TotalChars() const
{
    return screen_size_.width() * screen_size_.height();
}


QRogueDisplay::ThreadData::ThreadData(QRogueDisplay::ThreadData &other)
{
    dimensions = other.dimensions;

    int total = dimensions.x * dimensions.y;
    data.reset(new uint32_t[total]);
    memcpy(data.get(), other.data.get(), total * sizeof(uint32_t));

    show_cursor = other.show_cursor;
    cursor_pos = other.cursor_pos;
    render_regions = other.render_regions;
}
