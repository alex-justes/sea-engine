#include "core/Camera.h"

using namespace core;

Camera::Camera(const Point &position, const Size &size)
{
    this->set_position(position);
    this->size() = size;
    LOG_D("Camera %d created", unique_id())
}

Camera::~Camera()
{
    LOG_D("Camera %d removed", unique_id())
}

const Camera::List& Camera::get_visible_objects() const
{
    return _objects;
}

const Size &Camera::size() const
{
    return _size;
}

Size& Camera::size()
{
    return _size;
}

bool Camera::active()
{
    return _screens_attached > 0;
}

void Camera::update_visible_objects(List &&list)
{
    _objects = std::move(list);
}

CameraManager::const_iterator CameraManager::cbegin() const
{
    return _cameras.cbegin();
}

CameraManager::const_iterator CameraManager::cend() const
{
    return _cameras.cend();
}

CameraManager::iterator CameraManager::begin()
{
    return _cameras.begin();
}

CameraManager::iterator CameraManager::end()
{
    return _cameras.end();
}

Camera* CameraManager::create_camera(const Point& position, const Size& size)
{
    Item camera {new Camera(position, size)};
    auto ptr = camera.get();
    _cameras[camera->unique_id()] = std::move(camera);
    return ptr;
}

Camera* CameraManager::get_camera(Id camera)
{
    if (_cameras.count(camera) == 0)
    {
        return nullptr;
    }
    return _cameras[camera].get();
}

// TODO: Make it safe to any screen
void CameraManager::remove_camera(Id camera)
{
    if (_cameras.count(camera) == 0)
    {
        return;
    }
    _cameras.erase(camera);
}