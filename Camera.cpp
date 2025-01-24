#include "Camera.hpp"

namespace gps {
    float axeX, axeY;

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        // Update the rest of camera parameters
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        axeX = glm::degrees(atan2(cameraFrontDirection.z, cameraFrontDirection.x));
        axeY = glm::degrees(asin(cameraFrontDirection.y));

        this->cameraTarget = cameraPosition + cameraFrontDirection;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, cameraUpDirection));

        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        // Actualizam axa X si y
        axeX += yaw;
        axeY += pitch;

        // Constrangem axeY intre -89 si 89 grade pentru a evita problemele legate de inversarea imaginii
        if (axeY > 89.0f) axeY = 89.0f;
        if (axeY < -89.0f) axeY = -89.0f;

        // Calculam noua directe
        glm::vec3 front;
        front.x = cos(glm::radians(axeX));
        front.y = sin(glm::radians(axeY));
        front.z = sin(glm::radians(axeX)) * cos(glm::radians(axeY));

        // Normalizam directia 
        cameraFrontDirection = glm::normalize(front);

        // Actualizam target si right direction
        cameraTarget = cameraPosition + cameraFrontDirection;
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    // Getter and setter for camera position/target
    glm::vec3 Camera::getCameraPosition() const {
        return cameraPosition;
    }

    glm::vec3 Camera::getCameraTarget() const {
        return cameraTarget;
    }

    void Camera::setCameraPosition(const glm::vec3& position) {
        cameraPosition = position;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    void Camera::setCameraTarget(const glm::vec3& target) {
        cameraTarget = target;
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

}