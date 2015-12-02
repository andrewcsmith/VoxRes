# VoxRes

A resonance-finder for SuperCollider. Modify the relevant line in CMakeLists.txt
to point to your personal VoxBox.dylib file.

## What?

This baby finds resonances in whatever input it's given (downsampled 4x for
voice purposes) and spits them out as a 4-channel control rate.

## Why does it crash my computer?

VoxRes calls the dynamic library VoxBox.dylib, written in Rust. You have to have
the VoxBox library compiled for this to work.

### But it's still crashing...

Open an issue!
