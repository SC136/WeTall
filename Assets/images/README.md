# Gallery Images

This directory contains images for the gallery page. 

## Quick Start

To populate this folder with the gallery images, run the download script:

```bash
cd Assets/images
./download_images.sh
```

Or download the images manually from the URLs listed below.

**Important:** The gallery.html file has been updated to reference these local image paths. Once the images are downloaded to this folder, the gallery page will display them properly.

## Image Sources

The following images need to be downloaded:

## Images to Download:

1. **gallery-image-15.jpg** - https://pbs.twimg.com/media/Gr1Pi_7XsAAQ7Vb?format=jpg&name=large
2. **gallery-image-14.jpg** - https://pbs.twimg.com/media/GqaUU63WQAAMW-z?format=jpg&name=medium
3. **gallery-image-13-3.jpg** - https://pbs.twimg.com/media/GmAXavSWwAASxOv?format=jpg&name=4096x4096
4. **gallery-image-13-2.jpg** - https://pbs.twimg.com/media/GmAXXYgakAAQmH0?format=jpg&name=4096x4096
5. **gallery-image-13-1.jpg** - https://pbs.twimg.com/media/GmAXMHmXcAAn2oS?format=jpg&name=4096x4096
6. **gallery-image-12.jpg** - https://pbs.twimg.com/media/Gl-wA76XIAA7MzT?format=jpg&name=900x900
7. **gallery-image-11.png** - https://i.imgur.com/FejlC4e.png (from https://imgur.com/FejlC4e)
8. **gallery-image-10.png** - https://i.imgur.com/tPvD44n.png (from https://imgur.com/tPvD44n)
9. **gallery-image-09.png** - https://i.imgur.com/ylfd5oD.png (from https://imgur.com/ylfd5oD)
10. **gallery-image-08.png** - https://i.imgur.com/YOXTKkN.png
11. **gallery-image-07.png** - https://i.imgur.com/GNoUhbZ.png
12. **gallery-image-06.jpeg** - https://i.imgur.com/j4ytFGS.jpeg
13. **gallery-image-05.jpeg** - https://i.imgur.com/nh90uR9.jpeg
14. **gallery-image-04.png** - https://i.imgur.com/u11HjK7.png
15. **vtol-design-overview.jpg** - https://i.imgur.com/ewZxY0z.jpg
16. **vtol-top-view-dimensions-2.png** - https://i.imgur.com/LTjFZEg.png
17. **vtol-top-view-dimensions-3.png** - https://i.imgur.com/8EA1OCB.png
18. **fea-stress-map.jpg** - https://freeimage.host/i/KozqfQ2
19. **vtol-frame-motor-holder.jpg** - https://freeimage.host/i/KozCOes
20. **pcb-layout.jpg** - https://freeimage.host/i/KozoxMQ

## Download Script

Run this script to download all images:

```bash
#!/bin/bash
cd "$(dirname "$0")"

curl -L "https://pbs.twimg.com/media/Gr1Pi_7XsAAQ7Vb?format=jpg&name=large" -o gallery-image-15.jpg
curl -L "https://pbs.twimg.com/media/GqaUU63WQAAMW-z?format=jpg&name=medium" -o gallery-image-14.jpg
curl -L "https://pbs.twimg.com/media/GmAXavSWwAASxOv?format=jpg&name=4096x4096" -o gallery-image-13-3.jpg
curl -L "https://pbs.twimg.com/media/GmAXXYgakAAQmH0?format=jpg&name=4096x4096" -o gallery-image-13-2.jpg
curl -L "https://pbs.twimg.com/media/GmAXMHmXcAAn2oS?format=jpg&name=4096x4096" -o gallery-image-13-1.jpg
curl -L "https://pbs.twimg.com/media/Gl-wA76XIAA7MzT?format=jpg&name=900x900" -o gallery-image-12.jpg
curl -L "https://i.imgur.com/FejlC4e.png" -o gallery-image-11.png
curl -L "https://i.imgur.com/tPvD44n.png" -o gallery-image-10.png
curl -L "https://i.imgur.com/ylfd5oD.png" -o gallery-image-09.png
curl -L "https://i.imgur.com/YOXTKkN.png" -o gallery-image-08.png
curl -L "https://i.imgur.com/GNoUhbZ.png" -o gallery-image-07.png
curl -L "https://i.imgur.com/j4ytFGS.jpeg" -o gallery-image-06.jpeg
curl -L "https://i.imgur.com/nh90uR9.jpeg" -o gallery-image-05.jpeg
curl -L "https://i.imgur.com/u11HjK7.png" -o gallery-image-04.png
curl -L "https://i.imgur.com/ewZxY0z.jpg" -o vtol-design-overview.jpg
curl -L "https://i.imgur.com/LTjFZEg.png" -o vtol-top-view-dimensions-2.png
curl -L "https://i.imgur.com/8EA1OCB.png" -o vtol-top-view-dimensions-3.png

# Note: freeimage.host links need to be converted to direct image URLs
# Visit the URLs and get the direct image links, then download manually
```
