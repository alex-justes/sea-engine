#include <map>
#include "Log.h"
#include "core/Screen.h"

using namespace core;

Screen::Screen(const Roi &roi, int32_t z_order, SDL_Renderer *renderer, const RGBA &base_color)
        :
        _roi(roi),
        _renderer(renderer),
        _base_color(base_color)
{
    set_collision_shape(roi);
    set_z_order(z_order);
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
    float scale_x = (float) _roi.width() / camera->size().x;
    float scale_y = (float) _roi.height() / camera->size().y;
    float scale = 1.f;
    PointI32 offset{0, 0};
    if (scale_x < scale_y)
    {
        scale = scale_x;
        offset.y = (int32_t) (_roi.height() / scale - camera->size().y) / 2;
    }
    else
    {
        scale = scale_y;
        offset.x = (int32_t) (_roi.width() / scale - camera->size().x) / 2;
    }
    _camera_to_screen.scale({scale, scale});
    _camera_to_screen.shift(offset);
    _screen_to_camera = _camera_to_screen.inverse();
}

void Screen::detach_camera()
{
    LOG_D("Camera %d detached from Screen %d", _camera->unique_id(), unique_id())
    _camera = nullptr;
}

const Camera *Screen::camera() const
{
    return _camera;
}

const Roi &Screen::roi() const
{
    return _roi;
}

SDL_Texture *Screen::render()
{
    if (_camera != nullptr && _texture != nullptr)
    {
        auto camera = const_cast<const Camera *>(_camera);
        SDL_SetRenderTarget(_renderer, _texture);
        SDL_SetRenderDrawColor(_renderer, _base_color.r, _base_color.g, _base_color.b, _base_color.a);
        SDL_RenderClear(_renderer);
        auto map = std::multimap<uint32_t, const complex::behavior::Renderable *>();
        for (const auto &object: camera->get_visible_objects())
        {
            map.emplace(object->z_order(), object);
        }

        auto scale = _camera_to_screen.scale();
        auto offset = _camera_to_screen.offset();

        float old_scale_x(1.f), old_scale_y(1.f);
        SDL_RenderGetScale(_renderer, &old_scale_x, &old_scale_y);
        SDL_RenderSetScale(_renderer, scale.x, scale.y);
        for (const auto &item: map)
        {
            const auto &object = item.second;
            PointI32 obj_pos = object->position();
            PointI32 cam_pos = camera->position();
            render(object->drawable(), obj_pos - cam_pos + offset);
        }
        SDL_SetRenderTarget(_renderer, nullptr);
    }
    return _texture;
}

void Screen::render(const drawable::Drawable *drawable, const PointI32 &position)
{
    auto single = dynamic_cast<const drawable::SingleDrawable *>(drawable);
    if (single != nullptr)
    {
        // just draw, based on shape
        auto rect = dynamic_cast<const drawable::DrawableRect *>(single);
        if (rect != nullptr)
        {
            SDL_Rect fill_rect = {position.x, position.y, (int32_t) rect->box_size().x, (int32_t) rect->box_size().y};
            const auto &fill_color = rect->fill_color();
            SDL_SetRenderDrawColor(_renderer, fill_color.r, fill_color.g, fill_color.b, fill_color.a);
            SDL_RenderFillRect(_renderer, &fill_rect);
            const auto &border_color = rect->border_color();
            SDL_SetRenderDrawColor(_renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderDrawRect(_renderer, &fill_rect);
            return;
        }
        return;
    }
    auto compound = dynamic_cast<const drawable::CompoundDrawable *>(drawable);
    if (compound != nullptr)
    {
        for (const auto &item: compound->get_drawables())
        {
            render(item, position);
        }
    }
}

Point Screen::to_camera_coords(const PointF &pt) const
{
    return _screen_to_camera*pt;
}

bool Screen::camera_attached()
{
    return _camera != nullptr;
}

bool Screen::accept_mouse_input() const
{
    return _accept_mouse_input;
}

void Screen::set_accept_mouse_input(bool flag)
{
    _accept_mouse_input = flag;
}
