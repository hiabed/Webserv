<?php
// Set the content type header to indicate that the response is an image
header('Content-Type: image/jpeg');

// // Path to the image file
$imagePath = '/home/mallaoui/Desktop/web/website/outfile_1714256966-862344.jpeg';

// // Output the image file
readfile($imagePath);
echo "hello"
?>