#include <exception>

#include "resource.h"

namespace game {

Resource::Resource(ResourceType type, std::string name, GLuint resource, GLsizei size){
    mType = type;
    mName = name;
    mResource = resource;
    mSize = size;
}


Resource::Resource(ResourceType type, std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size){
    mType = type;
    mName = name;
    mArrayBuffer = array_buffer;
    mElementArrayBuffer = element_array_buffer;
    mSize = size;
}


Resource::~Resource(){

}


ResourceType Resource::getType(void) const {

    return mType;
}


const std::string Resource::getName(void) const {

    return mName;
}


GLuint Resource::getResource(void) const {

    return mResource;
}


GLuint Resource::getArrayBuffer(void) const {

    return mArrayBuffer;
}


GLuint Resource::getElementArrayBuffer(void) const {

    return mElementArrayBuffer;
}


GLsizei Resource::getSize(void) const {

    return mSize;
}

} // namespace game
