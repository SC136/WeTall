#!/bin/bash
# Script to download all gallery images
cd "$(dirname "$0")"

echo "Downloading gallery images..."

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

echo "Note: freeimage.host links need direct image URLs."
echo "Please visit these URLs in a browser and download manually:"
echo "  - https://freeimage.host/i/KozqfQ2 -> fea-stress-map.jpg"
echo "  - https://freeimage.host/i/KozCOes -> vtol-frame-motor-holder.jpg"
echo "  - https://freeimage.host/i/KozoxMQ -> pcb-layout.jpg"
echo ""
echo "Download complete!"
