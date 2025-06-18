# VirtualTextureDemo

Copyright(c) 2020, Andy Du.

License type: MIT

Contact: eaglehorn58@gmail.com, eaglehorn58@163.com

---------------------------------------------
This is a demo written a few years ago to showcase the basic concept of Virtual Texture. Since it was created quite a while ago, there's no guarantee that the source code can still be compiled and built successfully. However, a previously compiled executable can be found in the Apps directory.

How to run:
1. Run: Apps\VirtualTextureSample\VirtualTextureSample.exe
2. File->Open Virtual Texture...
3. Select the file: Apps\VirtualTextureSample\Textures\TestVt.vt
4. Click the "自动运行" button on the right side panel (sorry for the Chinese words).

The video of this demo can be found at:
https://youtu.be/pRvfpCx3qm0

---------------------------------------------
The demo is based on DX11 and was written by C++ in VS 2017, below Qt libs may be needed if you want to compile the source code:
• qt-opensource-windows-x86-5.14.0.exe
• qt-vsaddin-msvc2017-2.4.3.vsix

---------------------------------------------
• The demo implements virtual texture in the style of Mega Texture, it doesn’t procedurally generate virtual texture pages at runtime.

• The conversion of mesh UV from individual texture space to virtual texture space is done when meshes are being loaded, by simply adding the offsets in the virtual texture to the original UVs.

• Two pixels are added to virtual texture pages on each side as borders, this enables built-in hardware bilinear filtering can be applied. The page size is reduced by the border so that the sum of both to be power-of-two, the downscale is preprocessed by a compute shader. 

• In the simple demo, virtual page requests are sent out from pixel shader through a downscaled UAV in the lighting pass. The UAV is only about 1/64 of screen size but a dither factor is applied so that every screen pixel can have a chance to send out page requests in a sequence of frames. One screen pixel can send out at most one page request in one frame because a Z-pre pass is rendered before the lighting pass.

• Virtual page data are stored in a custom file with streaming friendly format. At present, the demo loads page data in the same thread of rendering. Each frame, at most 32 requested pages are loaded into memory and stored into a texture array, then one draw call updates all the pages to the page cache. In real projects, streaming page data definitely should be done in additional threads, and it’s not difficult to be implemented.

• In the video, the page cache is set to a small size (2048 x 2048) so that the page replacement that occurs when the cache gets full could be clearly seen.
