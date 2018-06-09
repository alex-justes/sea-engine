#include <map>
#include "Log.h"
#include "core/Screen.h"

using namespace core;

Screen::Screen(const Roi &roi, uint32_t z_order, SDL_Renderer *renderer)
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

void Screen::attach_camera(Camera *camera)
{
    LOG_D("Camera %d attached to Screen %d", camera->unique_id(), unique_id())
    _camera = camera;
}

void Screen::detach_camera()
{
    LOG_D("Camera %d detached to Screen %d", _camera->unique_id(), unique_id())
    _camera = nullptr;
}

int Screen::z_order() const
{
    return _z_order;
}

const Roi& Screen::roi() const
{
    return _roi;
}

void Screen::change_z_order(uint32_t z_order)
{
    _z_order = z_order;
}

SDL_Texture* Screen::render()
{
    if (_camera != nullptr && _texture != nullptr)
    {
        auto camera = const_cast<const Camera *>(_camera);
        SDL_SetRenderTarget(_renderer, _texture);
        SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(_renderer);
        auto map = std::multimap<uint32_t, const behavior::Renderable *>();
        for (const auto &object: camera->get_visible_objects())
        {
            map.emplace(object->z_order(), object);
        }
        float scale = std::max((float)_roi.width() / camera->size().x, (float)_roi.height() / camera->size().y);
        float old_scale_x(1.f), old_scale_y(1.f);
        SDL_RenderGetScale(_renderer, &old_scale_x, &old_scale_y);
        SDL_RenderSetScale(_renderer, scale, scale);
        for (const auto &item: map)
        {
            const auto &object = item.second;
            PointI32 obj_pos = {object->position().x, object->position().y};
            PointI32 cam_pos = {camera->position().x, camera->position().y};
            render(object->drawable(), obj_pos - cam_pos, scale);
        }
        SDL_RenderSetScale(_renderer, old_scale_x, old_scale_y);
        SDL_SetRenderTarget(_renderer, nullptr);
    }
    return _texture;
}

void Screen::render(const drawable::Drawable *drawable, const PointI32 &position, float scale)
{
    auto single = dynamic_cast<const drawable::SingleDrawable *>(drawable);
    if (single != nullptr)
    {
        // just draw, based on shape
        auto rect = dynamic_cast<const drawable::DrawableRect *>(single);
        if (rect != nullptr)
        {
//            auto w = (int32_t)(scale * rect->size().x);
//            auto h = (int32_t)(scale * rect->size().y);
            SDL_Rect fill_rect = {position.x, position.y, (int32_t)rect->size().x, (int32_t)rect->size().y};
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

bool Screen::camera_attached()
{
    return _camera != nullptr;
}