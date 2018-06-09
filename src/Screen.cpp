#include <map>
#include "Log.h"
#include "core/Screen.h"

using namespace core;

Screen::Screen(const Screen::Roi &roi, int z_order, SDL_Renderer *renderer)
        :
        _roi(roi),
        _renderer(renderer),
        _z_order(z_order)
{
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, roi.width(),
                                 roi.height());
    if (_texture == nullptr)
    {
        LOG_E("Unable to create texture: %s", SDL_GetError())
        /* Nothing i can do right now to handle this in some reasonable way... */
    }
}

Screen::~Screen()
{
    if (_texture != nullptr)
    {
        SDL_DestroyTexture(_texture);
    }
}

int Screen::z_order() const
{
    return _z_order;
}

void Screen::change_z_order(int z_order)
{
    _z_order = z_order;
}

const SDL_Texture* Screen::render()
{
    if (_camera != nullptr && _texture != nullptr)
    {
        SDL_SetRenderTarget(_renderer, _texture);
        SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(_renderer);
        auto map = std::multimap<uint32_t, const behavior::Renderable *>();
        for (const auto &object: _camera->get_visible_objects())
        {
            map.emplace(object->z_order(), object);
        }
        for (const auto &item: map)
        {
            const auto &object = item.second;
            float scale = std::max(_camera->roi().width() / _roi.width(), _camera->roi().height() / _roi.height());
            render(object->drawable(), object->position() - _camera->position(), scale);
        }
        SDL_SetRenderTarget(_renderer, nullptr);
    }
    return _texture;
}

void Screen::render(const drawable::Drawable *drawable, const Point &position, float scale)
{
    auto single = dynamic_cast<const drawable::SingleDrawable *>(drawable);
    if (single != nullptr)
    {
        // just draw, based on shape
        auto rect = dynamic_cast<const drawable::RectShape *>(single->shape());
        if (rect != nullptr)
        {
            auto[x, y] = rect->rect().top_left;
            x = (uint32_t)(scale * x) + position.x;
            y = (uint32_t)(scale * y) + position.y;
            auto w = (uint32_t)(scale * rect->rect().width());
            auto h = (uint32_t)(scale * rect->rect().height());
            SDL_Rect fill_rect = {x, y, w, h};
            const auto& color = rect->color();
            SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(_renderer, &fill_rect);
            return;
        }
        return;
    }
    auto compound = dynamic_cast<const drawable::CompoundDrawable *>(drawable);
    if (compound != nullptr)
    {
        for (const auto &item: compound->get_drawables())
        {
            render(item, position, scale);
        }
    }
}