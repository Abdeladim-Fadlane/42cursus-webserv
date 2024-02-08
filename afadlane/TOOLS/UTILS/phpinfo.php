#!/usr/bin/php-cgi

<?php

// Set HTTP response headers
header("HTTP/1.1 404 Not Found");
header("Content-Type: text/html");
header("Content-Length: 122");

// HTML content to be returned
$html_content = "
<html>
<head>
<title>404 Not Found</title>
</head>
<body>
<h1>404 Not Found</h1>
<p>The requested resource was not found on this server.</p>
</body>
</html>
";

// Output HTML content
echo $html_content;
?>
