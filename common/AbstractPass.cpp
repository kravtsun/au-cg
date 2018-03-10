#include <cassert>
#include <stdexcept>
#include "AbstractPass.h"
#include "wrappers/AllWrappers.h"

AbstractPass::AbstractPass(int width, int height)
        : width(width)
        , height(height)
{}

AbstractPass::~AbstractPass() = default;
