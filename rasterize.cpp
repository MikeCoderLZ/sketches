
struct Triangle {
    Vec4 a,b,c; // Vertices
};

rasterize( Triangle tri )
{
    // Member this->mBounds is a rectangle that represents the dimensions of
    // the screen in pixels

    // Generate bounding box of the triangle in screen space coordinates

    auto width{mBounds.width()};
    auto height{mBounds.height()};

    // Naturally, these are intrinsic-accelerated tests
    // Don't attempt to draw outside the screen
    unsigned int xMin{ accel_cast<unsigned int>(++least(a.x,b.x,c.x,-1.0) * 0.5f * (width - 1.0f)) };
    unsigned int yMin{ accel_cast<unsigned int>(++least(a.y,b.y,c.y,-1.0) * 0.5f * (height - 1.0f)) };
    unsigned int xMax{ accel_cast<unsigned int>(++greatest(a.x,b.x,c.x,1.0) * 0.5f * (width - 1.0f)) };
    unsigned int yMax{ accel_cast<unsigned int>(++greatest(a.y,b.y,c.y,1.0) * 0.5f * (height - 1.0f)) };

    // <x,y,z,w> in screen space.
    
    for( unsigned int xp = xMin; xp < width; ++xp ) {
        for( unsigned int yp = yMin; yp < height; ++yp ) {
            
        }   
    }




}