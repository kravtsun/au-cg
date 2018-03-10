#ifndef SLOT_MACHINE_ALLWRAPPERSFWD_H
#define SLOT_MACHINE_ALLWRAPPERSFWD_H

#include <memory>

struct Program;
using ProgramWrapper = std::shared_ptr<Program>;

struct Framebuffer;
using FramebufferWrapper = std::shared_ptr<Framebuffer>;

struct Texture;
using TextureWrapper = std::shared_ptr<Texture>;

struct VertexArray;
using VertexArrayWrapper = std::shared_ptr<VertexArray>;

struct VertexBuffer;
using VertexBufferWrapper = std::shared_ptr<VertexBuffer>;

#endif //SLOT_MACHINE_ALLWRAPPERSFWD_H
